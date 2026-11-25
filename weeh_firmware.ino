#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

extern "C" {
#include "user_interface.h"
bool wifi_softap_deauth(uint8 mac[6]);
}

#include "index_html.h"

// ==================== CONFIGURATION ====================
static const char*    PROJECT_NAME        = "Wireless Electromagnetic Energy Harvester";
static const char*    AP_SSID             = "Electromagnetic Energy Harvester"; // 32 chars, open AP
static const uint8_t  AP_CHANNEL          = 6;      // WiFi channel 6
static const uint8_t  MAX_CLIENTS         = 8;      // Max simultaneous SoftAP clients
static const uint8_t  PRIORITY_SLOTS      = 3;      // First 3 unique clients get priority
static const char*    ADMIN_PIN           = "9610"; // Fixed admin PIN
static const uint32_t SCAN_PERIOD_MS      = 8000;   // Async RF scan interval (ms)
static const float    HARVEST_EFFICIENCY  = 0.20f;  // Assumed harvester efficiency (estimated)
static const float    VOLT_CAL            = 1.00f;  // A0 voltage calibration multiplier
static const uint16_t RATE_LIMIT_PER_10S  = 40;     // Max /api/* requests per device per 10 s
static const uint32_t MEM_RESERVE_BYTES   = 12000;  // Minimum free heap for guest API requests

// ==================== CONSTANTS ====================
static const uint8_t  MAX_NETS            = 12;
static const uint8_t  MAX_BLOCKLIST       = 8;
static const uint8_t  NUM_TEMPLATES       = 5;
static const uint32_t TOKEN_TTL_MS        = 600000UL; // 10 minutes
static const uint32_t LOCKOUT_MS          = 60000UL;  // 60 seconds
static const uint32_t HYSTERESIS_MS       = 3000UL;   // 3 seconds health mode hold
static const uint32_t DEAUTH_CHECK_MS     = 3000UL;   // Re-check blocklist every 3 s
static const uint32_t SERIAL_PERIOD_MS    = 10000UL;  // Status line every 10 s
static const uint32_t ADC_PERIOD_MS       = 50UL;     // 50 ms A0 sample cadence
static const uint32_t CFG_MAGIC           = 0x45485631UL;

static const IPAddress AP_IP(192, 168, 4, 1);
static const IPAddress AP_MASK(255, 255, 255, 0);

// ==================== TYPES ====================
enum LedMode : uint8_t {
  LED_MODE_SYSTEM = 0,
  LED_MODE_ADMIN  = 1
};

enum HealthMode : uint8_t {
  HEALTH_HEALTHY = 0,
  HEALTH_BUSY,
  HEALTH_SLOWING,
  HEALTH_LOW_MEMORY,
  HEALTH_TROUBLE
};

struct NetInfo {
  char    ssid[25];
  int32_t rssi;
  uint8_t channel;
  uint8_t encrypted;
};

struct DeviceRecord {
  bool     active;
  uint8_t  mac[6];
  uint32_t ip;
  uint32_t first_seen_ms;
  uint32_t first_seen_s;
  uint32_t request_count;
  uint16_t window_count;
  uint32_t window_start_ms;
  uint32_t last_rate_hit_ms;
  bool     is_priority;
  bool     is_heavy;
};

struct LedTemplate {
  char    name[17];
  uint8_t blinks;
  float   cycle_s;
  uint8_t brightness;
};

struct PersistConfig {
  uint32_t    magic;
  uint8_t     ledMode;
  uint8_t     ledSlot; // 1..5
  uint8_t     masterBrightness;
  LedTemplate templates[NUM_TEMPLATES];
  uint8_t     blockCount;
  uint8_t     blocklist[MAX_BLOCKLIST][6];
  uint32_t    checksum;
};

// ==================== GLOBALS ====================
static ESP8266WebServer server(80);
static DNSServer        dnsServer;

// RF scan state
static NetInfo  g_nets[MAX_NETS];
static uint8_t  g_netCount          = 0;
static int32_t  g_strongestDbm      = -100;
static double   g_totalUw           = 0.0;
static double   g_harvestUw         = 0.0;
static double   g_energyUj          = 0.0;
static uint16_t g_channels[13]      = {0};
static uint32_t g_scanCount         = 0;
static uint32_t g_lastScanStartMs   = 0;
static uint32_t g_lastScanDoneMs    = 0;
static uint32_t g_lastEnergyMs      = 0;
static bool     g_scanInProgress    = false;
static bool     g_scanFailed        = false;
static volatile bool g_scanReady    = false;
static volatile int  g_scanRawCount = 0;

// ADC state
static uint16_t g_adcRing[16]       = {0};
static uint8_t  g_adcIdx            = 0;
static uint32_t g_adcSum            = 0;
static float    g_adcPrevV          = 0.0f;
static float    g_adcV              = 0.0f;
static bool     g_adcOk             = true;
static uint32_t g_adcLastMs         = 0;

// Device tracking
static DeviceRecord g_devices[MAX_CLIENTS];

// Persistence & LED state
static PersistConfig g_cfg;
static PersistConfig g_savedCfg;
static bool          g_previewActive   = false;
static uint8_t       g_previewMode     = LED_MODE_SYSTEM;
static uint8_t       g_previewSlot     = 1;
static uint8_t       g_previewMaster   = 100;
static LedTemplate   g_previewTpl      = {"Preview", 1, 2.0f, 100};

// Health & loop metrics
static HealthMode g_healthCurrent      = HEALTH_HEALTHY;
static HealthMode g_healthPending      = HEALTH_HEALTHY;
static uint32_t   g_healthPendingMs    = 0;
static uint32_t   g_lastLoopMs         = 0;
static uint32_t   g_lastHighLoopMs     = 0;
static uint32_t   g_lastBusyLoopMs     = 0;
static uint32_t   g_lastRateHitMs      = 0;
static uint32_t   g_troubleBootUntilMs = 0;
static uint32_t   g_reqBurstStartMs    = 0;
static uint16_t   g_reqBurstCount      = 0;
static uint32_t   g_lastBurstActiveMs  = 0;
static int        g_lastPwmOut         = -1;

// Admin auth
static char     g_adminToken[9]        = {0};
static uint32_t g_tokenExpiresMs       = 0;
static uint8_t  g_wrongPinCount        = 0;
static uint32_t g_lockoutUntilMs       = 0;

// Timers
static uint32_t g_lastDeauthMs         = 0;
static uint32_t g_lastSerialMs         = 0;
static uint32_t g_lastStationSyncMs    = 0;

// ==================== HELPERS ====================
static uint32_t calcChecksum(const PersistConfig& c) {
  const uint8_t* p = reinterpret_cast<const uint8_t*>(&c);
  size_t len = sizeof(PersistConfig) - sizeof(uint32_t);
  uint32_t h = 2166136261UL;
  for (size_t i = 0; i < len; ++i) {
    h ^= p[i];
    h *= 16777619UL;
  }
  return h;
}

static void setDefaultConfig(PersistConfig& c) {
  memset(&c, 0, sizeof(c));
  c.magic = CFG_MAGIC;
  c.ledMode = LED_MODE_SYSTEM;
  c.ledSlot = 1;
  c.masterBrightness = 100;

  const char* names[NUM_TEMPLATES] = {
    "Heartbeat", "Sleepy", "Morse SOS", "Breathing", "Alarm"
  };
  const uint8_t blinks[NUM_TEMPLATES] = {2, 1, 3, 1, 6};
  const float   cycles[NUM_TEMPLATES] = {2.0f, 4.0f, 3.0f, 3.0f, 1.5f};
  const uint8_t bright[NUM_TEMPLATES] = {100, 30, 100, 60, 100};

  for (uint8_t i = 0; i < NUM_TEMPLATES; ++i) {
    strlcpy(c.templates[i].name, names[i], sizeof(c.templates[i].name));
    c.templates[i].blinks = blinks[i];
    c.templates[i].cycle_s = cycles[i];
    c.templates[i].brightness = bright[i];
  }
  c.blockCount = 0;
  c.checksum = calcChecksum(c);
}

static void saveConfigIfChanged() {
  g_cfg.magic = CFG_MAGIC;
  g_cfg.checksum = calcChecksum(g_cfg);
  if (memcmp(&g_cfg, &g_savedCfg, sizeof(PersistConfig)) == 0) {
    return;
  }
  EEPROM.put(0, g_cfg);
  EEPROM.commit();
  g_savedCfg = g_cfg;
}

static void loadConfig() {
  EEPROM.begin(sizeof(PersistConfig) + 16);
  EEPROM.get(0, g_cfg);
  if (g_cfg.magic != CFG_MAGIC || g_cfg.checksum != calcChecksum(g_cfg) ||
      g_cfg.ledSlot < 1 || g_cfg.ledSlot > NUM_TEMPLATES ||
      g_cfg.masterBrightness > 100 || g_cfg.blockCount > MAX_BLOCKLIST) {
    setDefaultConfig(g_cfg);
    memset(&g_savedCfg, 0, sizeof(g_savedCfg));
    saveConfigIfChanged();
  } else {
    g_savedCfg = g_cfg;
  }
}

static void formatMac(const uint8_t mac[6], char* out, size_t outSize) {
  snprintf(out, outSize, "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static bool parseMac(const char* str, uint8_t mac[6]) {
  if (!str) return false;
  uint8_t tmp[6];
  int idx = 0;
  const char* p = str;
  while (*p && idx < 6) {
    if (!isxdigit(static_cast<unsigned char>(p[0])) ||
        !isxdigit(static_cast<unsigned char>(p[1]))) {
      return false;
    }
    char hex[3] = {p[0], p[1], '\0'};
    tmp[idx++] = static_cast<uint8_t>(strtoul(hex, nullptr, 16));
    p += 2;
    if (idx < 6) {
      if (*p == ':' || *p == '-') {
        ++p;
      } else {
        return false;
      }
    }
  }
  if (idx != 6 || *p != '\0') return false;
  memcpy(mac, tmp, 6);
  return true;
}

static bool isMacBlocked(const uint8_t mac[6]) {
  for (uint8_t i = 0; i < g_cfg.blockCount; ++i) {
    if (memcmp(g_cfg.blocklist[i], mac, 6) == 0) return true;
  }
  return false;
}

static void jsonEscape(char* dst, size_t dstSize, const char* src) {
  if (dstSize == 0) return;
  size_t j = 0;
  for (size_t i = 0; src[i] != '\0' && j + 1 < dstSize; ++i) {
    unsigned char ch = static_cast<unsigned char>(src[i]);
    if (ch == '"' || ch == '\\') {
      if (j + 2 >= dstSize) break;
      dst[j++] = '\\';
      dst[j++] = static_cast<char>(ch);
    } else if (ch < 0x20) {
      dst[j++] = ' ';
    } else {
      dst[j++] = static_cast<char>(ch);
    }
  }
  dst[j] = '\0';
}

static bool extractParam(const char* key, char* out, size_t outSize) {
  if (outSize == 0) return false;
  out[0] = '\0';
  if (server.hasArg(key)) {
    strlcpy(out, server.arg(key).c_str(), outSize);
    return true;
  }
  if (!server.hasArg("plain")) return false;
  String bodyStr = server.arg("plain");
  const char* body = bodyStr.c_str();
  size_t keyLen = strlen(key);
  const char* p = body;
  while ((p = strchr(p, '"')) != nullptr) {
    ++p;
    if (strncmp(p, key, keyLen) == 0 && p[keyLen] == '"') {
      const char* q = p + keyLen + 1;
      while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
      if (*q != ':') {
        p += keyLen;
        continue;
      }
      ++q;
      while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
      size_t j = 0;
      if (*q == '"') {
        ++q;
        while (*q && *q != '"' && j + 1 < outSize) {
          if (*q == '\\' && q[1] != '\0') ++q;
          out[j++] = *q++;
        }
      } else {
        while (*q && *q != ',' && *q != '}' && *q != ' ' &&
               *q != '\r' && *q != '\n' && j + 1 < outSize) {
          out[j++] = *q++;
        }
      }
      out[j] = '\0';
      return true;
    }
  }
  return false;
}

static float calcRealCycleS(uint8_t blinks, float reqCycleS) {
  if (blinks < 1) blinks = 1;
  if (blinks > 10) blinks = 10;
  if (reqCycleS < 0.5f) reqCycleS = 0.5f;
  if (reqCycleS > 10.0f) reqCycleS = 10.0f;
  float minCycle = blinks * 0.10f; // >= 50 ms on + 50 ms off per blink
  return (reqCycleS < minCycle) ? minCycle : reqCycleS;
}

static void writeLedDuty(uint8_t brightness255) {
  int pwm = 255 - static_cast<int>(brightness255); // Active LOW
  if (pwm != g_lastPwmOut) {
    analogWrite(LED_BUILTIN, pwm);
    g_lastPwmOut = pwm;
  }
}

// ==================== DEVICE TRACKING & BLOCKLIST ====================
static void enforceBlocklistNow() {
  struct station_info* head = wifi_softap_get_station_info();
  for (struct station_info* it = head; it != nullptr; it = STAILQ_NEXT(it, next)) {
    if (isMacBlocked(it->bssid)) {
      uint8_t mac[6];
      memcpy(mac, it->bssid, 6);
      wifi_softap_deauth(mac);
    }
  }
  wifi_softap_free_station_info();
}

static void syncStations() {
  uint32_t now = millis();
  struct station_info* head = wifi_softap_get_station_info();

  bool stillHere[MAX_CLIENTS] = {false};

  for (struct station_info* it = head; it != nullptr; it = STAILQ_NEXT(it, next)) {
    if (isMacBlocked(it->bssid)) {
      uint8_t mac[6];
      memcpy(mac, it->bssid, 6);
      wifi_softap_deauth(mac);
      continue;
    }

    int found = -1;
    for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
      if (g_devices[i].active && memcmp(g_devices[i].mac, it->bssid, 6) == 0) {
        found = i;
        break;
      }
    }

    if (found >= 0) {
      stillHere[found] = true;
      if (it->ip.addr != 0) {
        g_devices[found].ip = it->ip.addr;
      }
    } else {
      for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
        if (!g_devices[i].active) {
          memset(&g_devices[i], 0, sizeof(DeviceRecord));
          g_devices[i].active = true;
          memcpy(g_devices[i].mac, it->bssid, 6);
          g_devices[i].ip = it->ip.addr;
          g_devices[i].first_seen_ms = now;
          g_devices[i].first_seen_s = now / 1000UL;
          g_devices[i].window_start_ms = now;
          stillHere[i] = true;
          break;
        }
      }
    }
  }
  wifi_softap_free_station_info();

  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (g_devices[i].active && !stillHere[i]) {
      g_devices[i].active = false;
    }
  }

  uint8_t prioCount = 0;
  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (g_devices[i].active && g_devices[i].is_priority) {
      ++prioCount;
    }
  }

  while (prioCount < PRIORITY_SLOTS) {
    int best = -1;
    for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
      if (g_devices[i].active && !g_devices[i].is_priority) {
        if (best < 0 || static_cast<int32_t>(g_devices[i].first_seen_ms - g_devices[best].first_seen_ms) < 0) {
          best = i;
        }
      }
    }
    if (best < 0) break;
    g_devices[best].is_priority = true;
    ++prioCount;
  }
}

static DeviceRecord* findDeviceByIp(uint32_t ip) {
  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (g_devices[i].active && g_devices[i].ip == ip) {
      return &g_devices[i];
    }
  }
  syncStations();
  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (g_devices[i].active && g_devices[i].ip == ip) {
      return &g_devices[i];
    }
  }
  return nullptr;
}

// ==================== RF SCAN ====================
static void onScanComplete(int networksFound) {
  g_scanRawCount = networksFound;
  g_scanReady = true;
}

static void processCompletedScan() {
  g_scanReady = false;
  g_scanInProgress = false;

  int n = g_scanRawCount;
  if (n < 0) {
    g_scanFailed = true;
    WiFi.scanDelete();
    return;
  }

  g_scanFailed = false;
  uint32_t now = millis();
  double elapsedS = (g_lastEnergyMs > 0) ? (static_cast<double>(now - g_lastEnergyMs) / 1000.0) : 0.0;
  g_lastEnergyMs = now;
  g_lastScanDoneMs = now;
  ++g_scanCount;

  memset(g_channels, 0, sizeof(g_channels));
  g_netCount = 0;
  g_strongestDbm = -100;
  g_totalUw = 0.0;

  for (int i = 0; i < n; ++i) {
    String ssidStr = WiFi.SSID(i);
    if (strcmp(ssidStr.c_str(), AP_SSID) == 0) {
      continue;
    }

    int32_t rssi = WiFi.RSSI(i);
    int32_t ch = WiFi.channel(i);
    uint8_t enc = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? 0 : 1;

    if (rssi > g_strongestDbm) {
      g_strongestDbm = rssi;
    }
    if (ch >= 1 && ch <= 13) {
      g_channels[ch - 1]++;
    }
    g_totalUw += pow(10.0, static_cast<double>(rssi) / 10.0) * 1000.0;

    NetInfo item;
    strlcpy(item.ssid, ssidStr.c_str(), sizeof(item.ssid));
    item.rssi = rssi;
    item.channel = static_cast<uint8_t>(ch);
    item.encrypted = enc;

    if (g_netCount < MAX_NETS) {
      uint8_t pos = g_netCount;
      while (pos > 0 && g_nets[pos - 1].rssi < item.rssi) {
        g_nets[pos] = g_nets[pos - 1];
        --pos;
      }
      g_nets[pos] = item;
      ++g_netCount;
    } else if (item.rssi > g_nets[MAX_NETS - 1].rssi) {
      uint8_t pos = MAX_NETS - 1;
      while (pos > 0 && g_nets[pos - 1].rssi < item.rssi) {
        g_nets[pos] = g_nets[pos - 1];
        --pos;
      }
      g_nets[pos] = item;
    }
  }

  g_harvestUw = g_totalUw * static_cast<double>(HARVEST_EFFICIENCY);
  g_energyUj += g_harvestUw * elapsedS;

  WiFi.scanDelete();
}

static void startAsyncScan() {
  if (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
    return;
  }
  g_lastScanStartMs = millis();
  g_scanInProgress = true;
  WiFi.scanNetworksAsync(onScanComplete, true);
}

// ==================== ADC ====================
static void initAdc() {
  uint32_t sum = 0;
  uint16_t last = 0;
  for (uint8_t i = 0; i < 16; ++i) {
    last = static_cast<uint16_t>(analogRead(A0));
    g_adcRing[i] = last;
    sum += last;
  }
  g_adcSum = sum;
  g_adcIdx = 0;
  g_adcPrevV = (static_cast<float>(last) / 1023.0f) * 3.3f * VOLT_CAL;
  g_adcV = (static_cast<float>(sum) / 16.0f / 1023.0f) * 3.3f * VOLT_CAL;
  g_adcOk = true;
  g_adcLastMs = millis();
}

static void updateAdc(uint32_t now) {
  if (now - g_adcLastMs < ADC_PERIOD_MS) return;
  g_adcLastMs = now;

  uint16_t raw = static_cast<uint16_t>(analogRead(A0));
  g_adcSum -= g_adcRing[g_adcIdx];
  g_adcRing[g_adcIdx] = raw;
  g_adcSum += raw;
  g_adcIdx = (g_adcIdx + 1) & 0x0F;

  g_adcV = (static_cast<float>(g_adcSum) / 16.0f / 1023.0f) * 3.3f * VOLT_CAL;
  float vNow = (static_cast<float>(raw) / 1023.0f) * 3.3f * VOLT_CAL;
  g_adcOk = (fabsf(vNow - g_adcPrevV) <= 0.3f);
  g_adcPrevV = vNow;
}

// ==================== LED & HEALTH ====================
static const char* healthName(HealthMode m) {
  switch (m) {
    case HEALTH_HEALTHY:    return "healthy";
    case HEALTH_BUSY:       return "busy";
    case HEALTH_SLOWING:    return "slowing";
    case HEALTH_LOW_MEMORY: return "low_memory";
    case HEALTH_TROUBLE:    return "trouble";
  }
  return "healthy";
}

static HealthMode evaluateHealth(uint32_t now) {
  bool recentRestart = (static_cast<int32_t>(now - g_troubleBootUntilMs) < 0);
  bool wifiBad = (WiFi.getMode() != WIFI_AP_STA) || (WiFi.softAPIP() != AP_IP);
  if (recentRestart || wifiBad || g_scanFailed) {
    return HEALTH_TROUBLE;
  }

  if (ESP.getFreeHeap() < MEM_RESERVE_BYTES) {
    return HEALTH_LOW_MEMORY;
  }

  bool heavyActive = false;
  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (g_devices[i].active && g_devices[i].is_heavy) {
      heavyActive = true;
      break;
    }
  }
  bool rateHitRecent = (g_lastRateHitMs != 0 && (now - g_lastRateHitMs) < 10000UL);
  bool loopRising = (g_lastHighLoopMs != 0 && (now - g_lastHighLoopMs) < 5000UL);
  if (heavyActive || rateHitRecent || loopRising) {
    return HEALTH_SLOWING;
  }

  bool loopBusy = (g_lastBusyLoopMs != 0 && (now - g_lastBusyLoopMs) < 3000UL);
  bool burstBusy = (g_lastBurstActiveMs != 0 && (now - g_lastBurstActiveMs) < 3000UL);
  if (WiFi.softAPgetStationNum() >= 3 || loopBusy || burstBusy) {
    return HEALTH_BUSY;
  }

  return HEALTH_HEALTHY;
}

static void updateHealthAndLed(uint32_t now) {
  HealthMode desired = evaluateHealth(now);
  if (desired == g_healthPending) {
    if (g_healthCurrent != desired && (now - g_healthPendingMs) >= HYSTERESIS_MS) {
      g_healthCurrent = desired;
    }
  } else {
    g_healthPending = desired;
    g_healthPendingMs = now;
  }

  uint8_t activeMode = g_previewActive ? g_previewMode : g_cfg.ledMode;
  if (activeMode == LED_MODE_SYSTEM) {
    uint8_t duty = 255;
    switch (g_healthCurrent) {
      case HEALTH_HEALTHY:
        duty = 255;
        break;
      case HEALTH_BUSY:
        duty = 102; // 40% brightness
        break;
      case HEALTH_SLOWING:
        duty = ((now % 1000UL) < 100UL) ? 255 : 0;
        break;
      case HEALTH_LOW_MEMORY:
        duty = ((now % 2000UL) < 100UL) ? 255 : 0;
        break;
      case HEALTH_TROUBLE:
        duty = ((now % 200UL) < 100UL) ? 255 : 0; // 5 flashes/sec
        break;
    }
    writeLedDuty(duty);
  } else {
    const LedTemplate* tpl = nullptr;
    uint8_t master = g_previewActive ? g_previewMaster : g_cfg.masterBrightness;
    if (g_previewActive) {
      tpl = &g_previewTpl;
    } else {
      uint8_t idx = (g_cfg.ledSlot >= 1 && g_cfg.ledSlot <= NUM_TEMPLATES) ? (g_cfg.ledSlot - 1) : 0;
      tpl = &g_cfg.templates[idx];
    }

    if (tpl->brightness == 0 || master == 0) {
      writeLedDuty(0);
      return;
    }

    uint8_t blinks = (tpl->blinks < 1) ? 1 : (tpl->blinks > 10 ? 10 : tpl->blinks);
    float realCycleS = calcRealCycleS(blinks, tpl->cycle_s);
    uint32_t cycleMs = static_cast<uint32_t>(realCycleS * 1000.0f + 0.5f);

    uint32_t slotMs = cycleMs / (static_cast<uint32_t>(blinks) * 2UL);
    uint32_t onMs = (slotMs < 50UL) ? 50UL : (slotMs > 150UL ? 150UL : slotMs);
    uint32_t offMs = onMs;
    uint32_t periodMs = onMs + offMs;
    uint32_t burstMs = static_cast<uint32_t>(blinks) * periodMs;

    uint32_t t = now % cycleMs;
    bool on = (t < burstMs) && ((t % periodMs) < onMs);
    if (on) {
      uint32_t scaled = (static_cast<uint32_t>(tpl->brightness) * master * 255UL + 5000UL) / 10000UL;
      if (scaled == 0) scaled = 1;
      if (scaled > 255) scaled = 255;
      writeLedDuty(static_cast<uint8_t>(scaled));
    } else {
      writeLedDuty(0);
    }
  }
}

// ==================== HTTP GUARDS & CAPTIVE PORTAL ====================
static bool checkApiGuards(DeviceRecord** outDev) {
  uint32_t now = millis();
  if (now - g_reqBurstStartMs >= 2000UL) {
    g_reqBurstStartMs = now;
    g_reqBurstCount = 0;
  }
  ++g_reqBurstCount;
  if (g_reqBurstCount >= 10) {
    g_lastBurstActiveMs = now;
  }

  uint32_t clientIp = static_cast<uint32_t>(server.client().remoteIP());
  DeviceRecord* dev = findDeviceByIp(clientIp);
  if (outDev) *outDev = dev;

  if (dev != nullptr) {
    ++dev->request_count;
    if (now - dev->window_start_ms >= 10000UL) {
      dev->window_start_ms = now;
      dev->window_count = 0;
      if (dev->is_heavy && (now - dev->last_rate_hit_ms >= 30000UL)) {
        dev->is_heavy = false;
      }
    }
    ++dev->window_count;
    if (dev->window_count > RATE_LIMIT_PER_10S) {
      dev->is_heavy = true;
      dev->last_rate_hit_ms = now;
      g_lastRateHitMs = now;
      server.sendHeader("Cache-Control", "no-store");
      server.send(429, "text/plain", "slow down");
      return false;
    }
  }

  bool isPriority = (dev != nullptr && dev->is_priority);
  if (!isPriority && ESP.getFreeHeap() < MEM_RESERVE_BYTES) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(503, "text/plain", "busy, try again");
    return false;
  }

  return true;
}

static bool requireAdminToken() {
  if (g_adminToken[0] == '\0' || static_cast<int32_t>(g_tokenExpiresMs - millis()) <= 0) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(401, "application/json", "{\"ok\":false,\"error\":\"unauthorized\"}");
    return false;
  }
  if (!server.hasHeader("X-Token") || strcmp(server.header("X-Token").c_str(), g_adminToken) != 0) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(401, "application/json", "{\"ok\":false,\"error\":\"unauthorized\"}");
    return false;
  }
  return true;
}

static bool isHostLocal() {
  const String& h = server.hostHeader();
  if (h.length() == 0) return true;
  const char* host = h.c_str();
  return (strcmp(host, "192.168.4.1") == 0 || strcmp(host, "192.168.4.1:80") == 0);
}

static void sendCaptiveRedirect() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.sendHeader("Cache-Control", "no-store");
  server.send(302, "text/plain", "");
}

// ==================== ROUTE HANDLERS ====================
static void handleRoot() {
  if (!isHostLocal()) {
    sendCaptiveRedirect();
    return;
  }
  server.sendHeader("Cache-Control", "max-age=3600");
  server.send_P(200, "text/html", INDEX_HTML);
}

static void handleApiData() {
  DeviceRecord* dev = nullptr;
  if (!checkApiGuards(&dev)) return;

  uint32_t now = millis();
  uint32_t up = now / 1000UL;
  uint32_t scanAge = (g_scanCount > 0) ? ((now - g_lastScanDoneMs) / 1000UL) : up;
  bool priority = (dev != nullptr && dev->is_priority);
  bool heavy = (dev != nullptr && dev->is_heavy);
  uint8_t activeMode = g_previewActive ? g_previewMode : g_cfg.ledMode;
  const char* ledModeStr = (activeMode == LED_MODE_ADMIN) ? "admin" : "system";

  char buf[1664];
  size_t pos = 0;
  size_t rem = sizeof(buf);
  int n = snprintf(
    buf + pos, rem,
    "{\"strongest\":%d,\"total_uw\":%.6f,\"harvest_uw\":%.6f,\"energy_uj\":%.3f,"
    "\"count\":%u,\"channels\":[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u],"
    "\"adc_v\":%.2f,\"adc_ok\":%s,\"up\":%lu,\"scan_age\":%lu,"
    "\"priority\":%s,\"heavy\":%s,\"led_mode\":\"%s\",\"health\":\"%s\",\"nets\":[",
    static_cast<int>(g_strongestDbm),
    g_totalUw,
    g_harvestUw,
    g_energyUj,
    static_cast<unsigned int>(WiFi.softAPgetStationNum()),
    static_cast<unsigned int>(g_channels[0]),  static_cast<unsigned int>(g_channels[1]),
    static_cast<unsigned int>(g_channels[2]),  static_cast<unsigned int>(g_channels[3]),
    static_cast<unsigned int>(g_channels[4]),  static_cast<unsigned int>(g_channels[5]),
    static_cast<unsigned int>(g_channels[6]),  static_cast<unsigned int>(g_channels[7]),
    static_cast<unsigned int>(g_channels[8]),  static_cast<unsigned int>(g_channels[9]),
    static_cast<unsigned int>(g_channels[10]), static_cast<unsigned int>(g_channels[11]),
    static_cast<unsigned int>(g_channels[12]),
    static_cast<double>(g_adcV),
    g_adcOk ? "true" : "false",
    static_cast<unsigned long>(up),
    static_cast<unsigned long>(scanAge),
    priority ? "true" : "false",
    heavy ? "true" : "false",
    ledModeStr,
    healthName(g_healthCurrent)
  );
  if (n > 0 && static_cast<size_t>(n) < rem) {
    pos += static_cast<size_t>(n);
    rem = sizeof(buf) - pos;
  }

  for (uint8_t i = 0; i < g_netCount; ++i) {
    char escSsid[52];
    jsonEscape(escSsid, sizeof(escSsid), g_nets[i].ssid);
    n = snprintf(
      buf + pos, rem,
      "%s{\"s\":\"%s\",\"r\":%d,\"c\":%u,\"e\":%u}",
      (i > 0) ? "," : "",
      escSsid,
      static_cast<int>(g_nets[i].rssi),
      static_cast<unsigned int>(g_nets[i].channel),
      static_cast<unsigned int>(g_nets[i].encrypted)
    );
    if (n <= 0 || static_cast<size_t>(n) >= rem) break;
    pos += static_cast<size_t>(n);
    rem = sizeof(buf) - pos;
  }

  if (rem > 2) {
    buf[pos++] = ']';
    buf[pos++] = '}';
    buf[pos] = '\0';
  }

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", buf);
}

static void handleApiReset() {
  if (!checkApiGuards(nullptr)) return;
  g_energyUj = 0.0;
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", "{\"ok\":true}");
}

static void handleAdminLogin() {
  if (!checkApiGuards(nullptr)) return;

  uint32_t now = millis();
  if (g_lockoutUntilMs != 0 && static_cast<int32_t>(g_lockoutUntilMs - now) > 0) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(423, "text/plain", "locked");
    return;
  }
  g_lockoutUntilMs = 0;

  char pinBuf[16];
  if (!extractParam("pin", pinBuf, sizeof(pinBuf)) || strcmp(pinBuf, ADMIN_PIN) != 0) {
    ++g_wrongPinCount;
    if (g_wrongPinCount >= 5) {
      g_wrongPinCount = 0;
      g_lockoutUntilMs = now + LOCKOUT_MS;
      server.sendHeader("Cache-Control", "no-store");
      server.send(423, "text/plain", "locked");
      return;
    }
    server.sendHeader("Cache-Control", "no-store");
    server.send(401, "application/json", "{\"ok\":false,\"error\":\"invalid pin\"}");
    return;
  }

  g_wrongPinCount = 0;
  uint32_t rnd = static_cast<uint32_t>(ESP.random());
  if (rnd == 0) rnd = (now ^ 0xA5A55A5AUL);
  snprintf(g_adminToken, sizeof(g_adminToken), "%08lx", static_cast<unsigned long>(rnd));
  g_tokenExpiresMs = now + TOKEN_TTL_MS;

  char resp[64];
  snprintf(resp, sizeof(resp), "{\"ok\":true,\"token\":\"%s\"}", g_adminToken);
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", resp);
}

static void handleAdminDevices() {
  if (!checkApiGuards(nullptr)) return;
  if (!requireAdminToken()) return;

  syncStations();

  char buf[1536];
  size_t pos = 0;
  size_t rem = sizeof(buf);
  int n = snprintf(buf + pos, rem, "{\"devices\":[");
  if (n > 0 && static_cast<size_t>(n) < rem) {
    pos += static_cast<size_t>(n);
    rem = sizeof(buf) - pos;
  }

  bool first = true;
  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (!g_devices[i].active) continue;
    char macStr[18];
    formatMac(g_devices[i].mac, macStr, sizeof(macStr));
    n = snprintf(
      buf + pos, rem,
      "%s{\"mac\":\"%s\",\"first_seen_seconds\":%lu,\"request_count\":%lu,"
      "\"window_count\":%u,\"is_priority\":%s,\"is_heavy\":%s}",
      first ? "" : ",",
      macStr,
      static_cast<unsigned long>(g_devices[i].first_seen_s),
      static_cast<unsigned long>(g_devices[i].request_count),
      static_cast<unsigned int>(g_devices[i].window_count),
      g_devices[i].is_priority ? "true" : "false",
      g_devices[i].is_heavy ? "true" : "false"
    );
    if (n <= 0 || static_cast<size_t>(n) >= rem) break;
    pos += static_cast<size_t>(n);
    rem = sizeof(buf) - pos;
    first = false;
  }

  n = snprintf(buf + pos, rem, "],\"blocklist\":[");
  if (n > 0 && static_cast<size_t>(n) < rem) {
    pos += static_cast<size_t>(n);
    rem = sizeof(buf) - pos;
  }

  for (uint8_t i = 0; i < g_cfg.blockCount; ++i) {
    char macStr[18];
    formatMac(g_cfg.blocklist[i], macStr, sizeof(macStr));
    n = snprintf(buf + pos, rem, "%s\"%s\"", (i > 0) ? "," : "", macStr);
    if (n <= 0 || static_cast<size_t>(n) >= rem) break;
    pos += static_cast<size_t>(n);
    rem = sizeof(buf) - pos;
  }

  if (rem > 2) {
    buf[pos++] = ']';
    buf[pos++] = '}';
    buf[pos] = '\0';
  }

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", buf);
}

static void handleAdminRemove() {
  if (!checkApiGuards(nullptr)) return;
  if (!requireAdminToken()) return;

  char macBuf[24];
  uint8_t mac[6];
  if (!extractParam("mac", macBuf, sizeof(macBuf)) || !parseMac(macBuf, mac)) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid mac\"}");
    return;
  }

  wifi_softap_deauth(mac);
  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (g_devices[i].active && memcmp(g_devices[i].mac, mac, 6) == 0) {
      g_devices[i].active = false;
    }
  }
  syncStations();

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", "{\"ok\":true}");
}

static void handleAdminBlock() {
  if (!checkApiGuards(nullptr)) return;
  if (!requireAdminToken()) return;

  char macBuf[24];
  uint8_t mac[6];
  if (!extractParam("mac", macBuf, sizeof(macBuf)) || !parseMac(macBuf, mac)) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid mac\"}");
    return;
  }

  if (!isMacBlocked(mac)) {
    if (g_cfg.blockCount >= MAX_BLOCKLIST) {
      server.sendHeader("Cache-Control", "no-store");
      server.send(400, "application/json", "{\"ok\":false,\"error\":\"blocklist full\"}");
      return;
    }
    memcpy(g_cfg.blocklist[g_cfg.blockCount], mac, 6);
    ++g_cfg.blockCount;
    saveConfigIfChanged();
  }

  wifi_softap_deauth(mac);
  for (uint8_t i = 0; i < MAX_CLIENTS; ++i) {
    if (g_devices[i].active && memcmp(g_devices[i].mac, mac, 6) == 0) {
      g_devices[i].active = false;
    }
  }
  syncStations();

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", "{\"ok\":true}");
}

static void handleAdminUnblock() {
  if (!checkApiGuards(nullptr)) return;
  if (!requireAdminToken()) return;

  char macBuf[24];
  uint8_t mac[6];
  if (!extractParam("mac", macBuf, sizeof(macBuf)) || !parseMac(macBuf, mac)) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid mac\"}");
    return;
  }

  bool removed = false;
  for (uint8_t i = 0; i < g_cfg.blockCount; ++i) {
    if (memcmp(g_cfg.blocklist[i], mac, 6) == 0) {
      for (uint8_t j = i; j + 1 < g_cfg.blockCount; ++j) {
        memcpy(g_cfg.blocklist[j], g_cfg.blocklist[j + 1], 6);
      }
      --g_cfg.blockCount;
      removed = true;
      break;
    }
  }
  if (removed) {
    saveConfigIfChanged();
  }

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", "{\"ok\":true}");
}

static void handleAdminLed() {
  if (!checkApiGuards(nullptr)) return;
  if (!requireAdminToken()) return;

  char modeBuf[16];
  if (!extractParam("mode", modeBuf, sizeof(modeBuf))) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing mode\"}");
    return;
  }

  uint8_t newMode = LED_MODE_SYSTEM;
  uint8_t newSlot = g_cfg.ledSlot;
  if (strcmp(modeBuf, "system") == 0) {
    newMode = LED_MODE_SYSTEM;
  } else if (strcmp(modeBuf, "admin") == 0) {
    newMode = LED_MODE_ADMIN;
    char slotBuf[8];
    if (extractParam("slot", slotBuf, sizeof(slotBuf))) {
      int s = atoi(slotBuf);
      if (s < 1 || s > NUM_TEMPLATES) {
        server.sendHeader("Cache-Control", "no-store");
        server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid slot\"}");
        return;
      }
      newSlot = static_cast<uint8_t>(s);
    }
  } else {
    server.sendHeader("Cache-Control", "no-store");
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid mode\"}");
    return;
  }

  char prevBuf[8];
  bool preview = extractParam("preview", prevBuf, sizeof(prevBuf)) &&
                 (strcmp(prevBuf, "1") == 0 || strcmp(prevBuf, "true") == 0);

  if (preview) {
    g_previewActive = true;
    g_previewMode = newMode;
    g_previewSlot = newSlot;
    g_previewMaster = g_cfg.masterBrightness;
    g_previewTpl = g_cfg.templates[newSlot - 1];
  } else {
    g_previewActive = false;
    g_cfg.ledMode = newMode;
    g_cfg.ledSlot = newSlot;
    saveConfigIfChanged();
  }

  char resp[96];
  snprintf(resp, sizeof(resp),
           "{\"ok\":true,\"mode\":\"%s\",\"slot\":%u,\"master\":%u}",
           (newMode == LED_MODE_ADMIN) ? "admin" : "system",
           static_cast<unsigned int>(newSlot),
           static_cast<unsigned int>(g_cfg.masterBrightness));
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", resp);
}

static void handleAdminTemplate() {
  if (!checkApiGuards(nullptr)) return;
  if (!requireAdminToken()) return;

  char slotBuf[8];
  int slot = 0;
  if (extractParam("slot", slotBuf, sizeof(slotBuf))) {
    slot = atoi(slotBuf);
  }
  if (slot < 1 || slot > NUM_TEMPLATES) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid slot\"}");
    return;
  }

  LedTemplate tpl = g_cfg.templates[slot - 1];
  char tmp[32];

  if (extractParam("name", tmp, sizeof(tmp))) {
    strlcpy(tpl.name, tmp, sizeof(tpl.name));
  }
  if (extractParam("blinks", tmp, sizeof(tmp))) {
    int b = atoi(tmp);
    if (b < 1) b = 1;
    if (b > 10) b = 10;
    tpl.blinks = static_cast<uint8_t>(b);
  }
  if (extractParam("cycle", tmp, sizeof(tmp)) || extractParam("cycle_s", tmp, sizeof(tmp))) {
    float c = static_cast<float>(atof(tmp));
    if (c < 0.5f) c = 0.5f;
    if (c > 10.0f) c = 10.0f;
    tpl.cycle_s = c;
  }
  if (extractParam("brightness", tmp, sizeof(tmp))) {
    int br = atoi(tmp);
    if (br < 0) br = 0;
    if (br > 100) br = 100;
    tpl.brightness = static_cast<uint8_t>(br);
  }

  float realCycle = calcRealCycleS(tpl.blinks, tpl.cycle_s);
  tpl.cycle_s = realCycle;

  char prevBuf[8];
  bool preview = extractParam("preview", prevBuf, sizeof(prevBuf)) &&
                 (strcmp(prevBuf, "1") == 0 || strcmp(prevBuf, "true") == 0);

  if (preview) {
    g_previewActive = true;
    g_previewMode = LED_MODE_ADMIN;
    g_previewSlot = static_cast<uint8_t>(slot);
    g_previewMaster = g_cfg.masterBrightness;
    g_previewTpl = tpl;
  } else {
    g_previewActive = false;
    g_cfg.templates[slot - 1] = tpl;
    saveConfigIfChanged();
  }

  char escName[36];
  jsonEscape(escName, sizeof(escName), tpl.name);
  char resp[160];
  snprintf(
    resp, sizeof(resp),
    "{\"ok\":true,\"slot\":%d,\"name\":\"%s\",\"blinks\":%u,\"cycle\":%.2f,\"real_cycle\":%.2f,\"brightness\":%u}",
    slot, escName,
    static_cast<unsigned int>(tpl.blinks),
    static_cast<double>(tpl.cycle_s),
    static_cast<double>(realCycle),
    static_cast<unsigned int>(tpl.brightness)
  );
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", resp);
}

static void handleAdminMaster() {
  if (!checkApiGuards(nullptr)) return;
  if (!requireAdminToken()) return;

  char valBuf[16];
  if (!extractParam("value", valBuf, sizeof(valBuf)) &&
      !extractParam("master", valBuf, sizeof(valBuf)) &&
      !extractParam("brightness", valBuf, sizeof(valBuf))) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing value\"}");
    return;
  }

  int v = atoi(valBuf);
  if (v < 0) v = 0;
  if (v > 100) v = 100;
  uint8_t master = static_cast<uint8_t>(v);

  char prevBuf[8];
  bool preview = extractParam("preview", prevBuf, sizeof(prevBuf)) &&
                 (strcmp(prevBuf, "1") == 0 || strcmp(prevBuf, "true") == 0);

  if (preview) {
    g_previewActive = true;
    g_previewMaster = master;
    if (g_previewMode != LED_MODE_ADMIN) {
      g_previewMode = LED_MODE_ADMIN;
      g_previewSlot = g_cfg.ledSlot;
      g_previewTpl = g_cfg.templates[g_cfg.ledSlot - 1];
    }
  } else {
    g_previewActive = false;
    g_cfg.masterBrightness = master;
    saveConfigIfChanged();
  }

  char resp[48];
  snprintf(resp, sizeof(resp), "{\"ok\":true,\"master\":%u}", static_cast<unsigned int>(master));
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", resp);
}

static void handleNotFound() {
  if (!isHostLocal()) {
    sendCaptiveRedirect();
    return;
  }
  server.sendHeader("Cache-Control", "no-store");
  server.send(404, "text/plain", "Not found");
}

// ==================== SETUP & LOOP ====================
void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(LED_BUILTIN, OUTPUT);
  analogWriteRange(255);
  writeLedDuty(0);

  loadConfig();
  initAdc();
  memset(g_devices, 0, sizeof(g_devices));

  struct rst_info* ri = ESP.getResetInfoPtr();
  if (ri != nullptr && (ri->reason == REASON_WDT_RST ||
                        ri->reason == REASON_EXCEPTION_RST ||
                        ri->reason == REASON_SOFT_WDT_RST ||
                        ri->reason == REASON_SOFT_RESTART)) {
    g_troubleBootUntilMs = millis() + 20000UL;
  } else {
    g_troubleBootUntilMs = millis() + 6000UL;
  }

  // Start open hotspot first, then start non-blocking RF scan
  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(AP_IP, AP_IP, AP_MASK);
  WiFi.softAP(AP_SSID, nullptr, AP_CHANNEL, 0, MAX_CLIENTS);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", AP_IP);

  server.collectHeaders(String("X-Token"));

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/data", HTTP_GET, handleApiData);
  server.on("/api/reset", HTTP_GET, handleApiReset);

  server.on("/api/admin/login", HTTP_POST, handleAdminLogin);
  server.on("/api/admin/devices", HTTP_GET, handleAdminDevices);
  server.on("/api/admin/remove", HTTP_POST, handleAdminRemove);
  server.on("/api/admin/block", HTTP_POST, handleAdminBlock);
  server.on("/api/admin/unblock", HTTP_POST, handleAdminUnblock);
  server.on("/api/admin/led", HTTP_POST, handleAdminLed);
  server.on("/api/admin/template", HTTP_POST, handleAdminTemplate);
  server.on("/api/admin/master", HTTP_POST, handleAdminMaster);

  server.on("/generate_204", HTTP_GET, sendCaptiveRedirect);
  server.on("/gen_204", HTTP_GET, sendCaptiveRedirect);
  server.on("/hotspot-detect.html", HTTP_GET, sendCaptiveRedirect);
  server.on("/library/test/success.html", HTTP_GET, sendCaptiveRedirect);
  server.on("/connecttest.txt", HTTP_GET, sendCaptiveRedirect);
  server.on("/ncsi.txt", HTTP_GET, sendCaptiveRedirect);
  server.on("/redirect", HTTP_GET, sendCaptiveRedirect);
  server.on("/fwlink", HTTP_GET, sendCaptiveRedirect);

  server.onNotFound(handleNotFound);
  server.begin();

  g_lastEnergyMs = millis();
  g_lastLoopMs = millis();
  g_healthPendingMs = millis();
  startAsyncScan();

  if (g_cfg.ledMode == LED_MODE_ADMIN) {
    uint8_t idx = g_cfg.ledSlot - 1;
    Serial.printf("%s | AP: %s | IP: 192.168.4.1 | Max clients: %u | LED: admin (slot %u: %s)\n",
                  PROJECT_NAME, AP_SSID, static_cast<unsigned int>(MAX_CLIENTS),
                  static_cast<unsigned int>(g_cfg.ledSlot), g_cfg.templates[idx].name);
  } else {
    Serial.printf("%s | AP: %s | IP: 192.168.4.1 | Max clients: %u | LED: system\n",
                  PROJECT_NAME, AP_SSID, static_cast<unsigned int>(MAX_CLIENTS));
  }
}

void loop() {
  uint32_t now = millis();
  uint32_t loopDt = now - g_lastLoopMs;
  g_lastLoopMs = now;
  if (loopDt > 80UL) {
    g_lastHighLoopMs = now;
  } else if (loopDt > 25UL) {
    g_lastBusyLoopMs = now;
  }

  dnsServer.processNextRequest();
  server.handleClient();

  if (g_scanReady) {
    processCompletedScan();
  } else if (g_scanInProgress && (now - g_lastScanStartMs) > (SCAN_PERIOD_MS * 2UL)) {
    g_scanFailed = true;
    g_scanInProgress = false;
    WiFi.scanDelete();
  }

  if (!g_scanInProgress && (now - g_lastScanStartMs) >= SCAN_PERIOD_MS) {
    startAsyncScan();
  }

  updateAdc(now);

  if (now - g_lastStationSyncMs >= 500UL) {
    g_lastStationSyncMs = now;
    syncStations();
  }

  if (now - g_lastDeauthMs >= DEAUTH_CHECK_MS) {
    g_lastDeauthMs = now;
    if (g_cfg.blockCount > 0) {
      enforceBlocklistNow();
    }
  }

  updateHealthAndLed(now);

  if (now - g_lastSerialMs >= SERIAL_PERIOD_MS) {
    g_lastSerialMs = now;
    uint8_t activeMode = g_previewActive ? g_previewMode : g_cfg.ledMode;
    Serial.printf("scans=%lu strongest=%ddBm heap=%lu clients=%u led=%s\n",
                  static_cast<unsigned long>(g_scanCount),
                  static_cast<int>(g_strongestDbm),
                  static_cast<unsigned long>(ESP.getFreeHeap()),
                  static_cast<unsigned int>(WiFi.softAPgetStationNum()),
                  (activeMode == LED_MODE_ADMIN) ? "admin" : "system");
  }
}
