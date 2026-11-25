const char INDEX_HTML[] PROGMEM = R"WEEH(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<title>Wireless Electromagnetic Energy Harvester</title>
<style>
:root{
  --bg:#F5F5F7;
  --card:#FFFFFF;
  --card-2:#EFEFF4;
  --text:#1D1D1F;
  --muted:#6E6E73;
  --border:rgba(0,0,0,0.08);
  --glass:rgba(255,255,255,0.78);
  --accent:#007AFF;
  --accent-soft:rgba(0,122,255,0.12);
  --green:#28CD41;
  --amber:#FF9F0A;
  --red:#FF3B30;
  --radius:22px;
  --radius-sm:14px;
  --shadow:0 6px 24px rgba(0,0,0,0.05),0 1px 3px rgba(0,0,0,0.04);
  --nav-h:64px;
  --font:-apple-system,BlinkMacSystemFont,"SF Pro Text","Helvetica Neue",Arial,sans-serif;
}
@media(prefers-color-scheme:dark){
  :root:not([data-theme="light"]){
    --bg:#000000;
    --card:#1C1C1E;
    --card-2:#2C2C2E;
    --text:#F5F5F7;
    --muted:#98989D;
    --border:rgba(255,255,255,0.12);
    --glass:rgba(28,28,30,0.80);
    --accent:#0A84FF;
    --accent-soft:rgba(10,132,255,0.18);
    --green:#30D158;
    --amber:#FF9F0A;
    --red:#FF453A;
    --shadow:0 0 0 1px rgba(255,255,255,0.09);
  }
}
:root[data-theme="dark"]{
  --bg:#000000;
  --card:#1C1C1E;
  --card-2:#2C2C2E;
  --text:#F5F5F7;
  --muted:#98989D;
  --border:rgba(255,255,255,0.12);
  --glass:rgba(28,28,30,0.80);
  --accent:#0A84FF;
  --accent-soft:rgba(10,132,255,0.18);
  --green:#30D158;
  --amber:#FF9F0A;
  --red:#FF453A;
  --shadow:0 0 0 1px rgba(255,255,255,0.09);
}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
html,body{margin:0;padding:0;background:var(--bg);color:var(--text);font-family:var(--font);line-height:1.45;-webkit-font-smoothing:antialiased}
body{min-height:100vh;padding-bottom:calc(var(--nav-h) + env(safe-area-inset-bottom,0px) + 20px)}
button,input,select{font:inherit;color:inherit}
:focus-visible{outline:2px solid var(--accent);outline-offset:2px}
.num{font-variant-numeric:tabular-nums}

/* Header */
.app-header{
  position:sticky;top:0;z-index:30;
  background:var(--glass);
  -webkit-backdrop-filter:saturate(180%) blur(20px);
  backdrop-filter:saturate(180%) blur(20px);
  border-bottom:1px solid var(--border);
}
.header-inner{
  max-width:1160px;margin:0 auto;padding:12px 16px;
  display:flex;flex-wrap:wrap;align-items:center;justify-content:space-between;gap:10px;
}
.brand{min-width:0;flex:1 1 240px}
.brand h1{margin:0;font-size:clamp(1.05rem,2.4vw,1.35rem);font-weight:700;letter-spacing:-0.02em;line-height:1.2}
.brand-sub{display:flex;align-items:center;flex-wrap:wrap;gap:8px;margin-top:4px;font-size:12px;color:var(--muted)}
.dot{width:8px;height:8px;border-radius:50%;background:var(--muted);display:inline-block;flex-shrink:0}
.dot.ok{background:var(--green)}
.dot.warn{background:var(--amber)}
.dot.bad{background:var(--red)}
.header-actions{display:flex;align-items:center;gap:8px;flex-shrink:0}

/* Segmented control */
.seg{
  display:inline-flex;background:var(--card-2);padding:3px;border-radius:999px;border:1px solid var(--border);
}
.seg button{
  border:0;background:transparent;color:var(--muted);padding:6px 12px;min-height:36px;min-width:44px;
  border-radius:999px;font-size:13px;font-weight:600;cursor:pointer;transition:background .2s,color .2s;
}
.seg button[aria-pressed="true"]{background:var(--card);color:var(--text);box-shadow:0 1px 4px rgba(0,0,0,0.1)}

.icon-btn{
  min-width:44px;min-height:44px;border-radius:999px;border:1px solid var(--border);
  background:var(--card);display:inline-flex;align-items:center;justify-content:center;cursor:pointer;
  transition:transform .15s,background .2s;
}
.icon-btn:hover{background:var(--card-2)}
.icon-btn:active{transform:scale(0.96)}

/* Navigation */
.nav-bar{
  position:fixed;left:0;right:0;bottom:0;z-index:40;
  background:var(--glass);
  -webkit-backdrop-filter:saturate(180%) blur(20px);
  backdrop-filter:saturate(180%) blur(20px);
  border-top:1px solid var(--border);
  padding:6px 8px calc(6px + env(safe-area-inset-bottom,0px));
}
.nav-list{
  max-width:1160px;margin:0 auto;display:grid;grid-template-columns:repeat(6,1fr);gap:4px;
}
.nav-btn{
  border:0;background:transparent;color:var(--muted);border-radius:999px;
  min-height:48px;padding:4px 6px;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:2px;
  font-size:10.5px;font-weight:600;cursor:pointer;transition:background .2s,color .2s;
}
.nav-btn svg{width:20px;height:20px;stroke:currentColor;fill:none;stroke-width:1.8;stroke-linecap:round;stroke-linejoin:round}
.nav-btn[aria-selected="true"]{background:var(--accent-soft);color:var(--accent)}

@media(min-width:840px){
  body{padding-bottom:40px}
  .nav-bar{
    position:sticky;top:71px;bottom:auto;border-top:0;border-bottom:1px solid var(--border);
    padding:8px 16px;z-index:25;
  }
  .nav-list{display:flex;justify-content:flex-start;gap:8px}
  .nav-btn{flex-direction:row;min-height:44px;padding:0 16px;font-size:13.5px;gap:7px}
  .nav-btn svg{width:17px;height:17px}
}

/* Main container */
.main{max-width:1160px;margin:0 auto;padding:20px 16px}
.panel{display:none;animation:fadeIn .22s ease}
.panel.active{display:block}
@keyframes fadeIn{from{opacity:0;transform:translateY(4px)}to{opacity:1;transform:none}}

.page-title{margin:0 0 6px;font-size:clamp(1.5rem,3.5vw,2.1rem);font-weight:700;letter-spacing:-0.025em}
.page-lead{margin:0 0 18px;color:var(--muted);font-size:15px;max-width:68ch}

/* Cards & Grid */
.card{
  background:var(--card);border-radius:var(--radius);padding:20px;box-shadow:var(--shadow);
  border:1px solid var(--border);
}
.cards-4{display:grid;grid-template-columns:1fr;gap:14px;margin-bottom:16px}
@media(min-width:560px){.cards-4{grid-template-columns:repeat(2,1fr)}}
@media(min-width:960px){.cards-4{grid-template-columns:repeat(4,1fr)}}

.kpi-label{font-size:12px;font-weight:600;color:var(--muted);text-transform:uppercase;letter-spacing:0.04em;display:flex;justify-content:space-between;align-items:center}
.kpi-val{font-size:clamp(1.7rem,3vw,2.15rem);font-weight:700;letter-spacing:-0.03em;margin:10px 0 4px}
.kpi-sub{font-size:13px;color:var(--muted);display:flex;align-items:center;gap:8px;min-height:20px}

/* Badges & Chips */
.badge{
  display:inline-flex;align-items:center;gap:6px;padding:4px 10px;border-radius:999px;
  font-size:12px;font-weight:600;background:var(--card-2);color:var(--text);border:1px solid var(--border);
}
.badge.sim{background:rgba(255,159,10,0.14);color:var(--amber);border-color:rgba(255,159,10,0.3)}
.badge.real{background:var(--accent-soft);color:var(--accent);border-color:transparent}
.badge.ok{background:rgba(48,209,88,0.14);color:var(--green);border-color:transparent}
.badge.warn{background:rgba(255,159,10,0.14);color:var(--amber);border-color:transparent}
.badge.bad{background:rgba(255,59,48,0.14);color:var(--red);border-color:transparent}

.status-strip{
  display:flex;flex-wrap:wrap;align-items:center;justify-content:space-between;gap:10px;
  margin-bottom:16px;padding:14px 18px;background:var(--card);border-radius:var(--radius);
  box-shadow:var(--shadow);border:1px solid var(--border);
}
.status-main{font-size:16px;font-weight:600;letter-spacing:-0.01em}
.status-tags{display:flex;flex-wrap:wrap;align-items:center;gap:8px}

/* Signal 5-bar meter */
.bars{display:inline-flex;align-items:flex-end;gap:3px;height:16px}
.bars span{width:4px;border-radius:2px;background:var(--card-2);transition:background .2s}
.bars span:nth-child(1){height:4px}
.bars span:nth-child(2){height:7px}
.bars span:nth-child(3){height:10px}
.bars span:nth-child(4){height:13px}
.bars span:nth-child(5){height:16px}
.bars span.on{background:var(--accent)}

/* Chart */
.chart-card{margin-bottom:16px}
.chart-head{display:flex;flex-wrap:wrap;align-items:center;justify-content:space-between;gap:10px;margin-bottom:12px}
.chart-title{font-size:16px;font-weight:600;margin:0}
.canvas-wrap{position:relative;width:100%;height:240px}
.canvas-wrap canvas{width:100%;height:100%;display:block;touch-action:none}
.tooltip{
  position:absolute;pointer-events:none;background:var(--card);color:var(--text);
  border:1px solid var(--border);box-shadow:var(--shadow);border-radius:12px;
  padding:6px 10px;font-size:12px;white-space:nowrap;opacity:0;transform:translate(-50%,-115%);
  transition:opacity .12s;z-index:5;
}

/* Collapsible Nearby Networks */
.collapse-btn{
  width:100%;min-height:48px;background:transparent;border:0;padding:0;
  display:flex;align-items:center;justify-content:space-between;cursor:pointer;text-align:left;
  font-size:16px;font-weight:600;
}
.collapse-body{margin-top:14px;display:none}
.collapse-body.open{display:block}
.net-table{width:100%;border-collapse:collapse;font-size:14px}
.net-table th,.net-table td{padding:10px 8px;border-bottom:1px solid var(--border);text-align:left}
.net-table th{font-size:12px;color:var(--muted);font-weight:600;text-transform:uppercase}
.ch-bars{display:grid;grid-template-columns:repeat(13,1fr);gap:4px;align-items:end;height:76px;margin-top:16px;padding-top:10px;border-top:1px solid var(--border)}
.ch-col{display:flex;flex-direction:column;align-items:center;gap:4px;height:100%;justify-content:flex-end}
.ch-fill{width:100%;max-width:22px;background:var(--accent);border-radius:4px 4px 2px 2px;min-height:2px;opacity:0.85;transition:height .25s}
.ch-lbl{font-size:10px;color:var(--muted)}

/* How it works SVG & Flow */
.flow-wrap{overflow-x:auto;padding-bottom:6px}
.flow-svg{width:100%;min-width:680px;height:auto;display:block}
.flow-pulse{stroke-dasharray:6 8;animation:dashMove 1.4s linear infinite}
@keyframes dashMove{to{stroke-dashoffset:-28}}

/* Timeline */
.timeline{position:relative;padding-left:28px;margin:8px 0;display:flex;flex-direction:column;gap:18px}
.timeline::before{content:"";position:absolute;left:9px;top:8px;bottom:8px;width:2px;background:var(--border)}
.tl-item{position:relative}
.tl-dot{position:absolute;left:-25px;top:22px;width:12px;height:12px;border-radius:50%;background:var(--accent);border:3px solid var(--bg)}
.tl-item h3{margin:0 0 6px;font-size:16px}
.tl-item p{margin:0;color:var(--muted);font-size:14.5px}

/* Schematics */
.schem-grid{display:flex;flex-direction:column;gap:16px}
.schem-head{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-bottom:12px}
.schem-head h3{margin:0;font-size:17px}
.schem-svg{width:100%;height:auto;display:block;color:var(--text)}
.note-box{margin-top:10px;padding:10px 14px;border-radius:var(--radius-sm);background:var(--card-2);font-size:13px;color:var(--muted)}

/* Team */
.team-grid{display:grid;grid-template-columns:1fr;gap:14px}
@media(min-width:520px){.team-grid{grid-template-columns:repeat(2,1fr)}}
@media(min-width:960px){.team-grid{grid-template-columns:repeat(5,1fr)}}
.team-card{
  min-height:96px;display:flex;align-items:center;justify-content:center;text-align:center;
  font-size:16px;font-weight:600;letter-spacing:-0.01em;padding:20px 14px;
}

/* Admin */
.keypad-wrap{max-width:320px;margin:10px auto;text-align:center}
.pin-dots{display:flex;justify-content:center;gap:12px;margin:16px 0}
.pin-dot{width:14px;height:14px;border-radius:50%;border:2px solid var(--muted);transition:background .15s,border-color .15s}
.pin-dot.filled{background:var(--accent);border-color:var(--accent)}
.keypad{display:grid;grid-template-columns:repeat(3,1fr);gap:10px}
.key-btn{
  min-height:56px;border-radius:16px;border:1px solid var(--border);background:var(--card-2);
  font-size:20px;font-weight:600;cursor:pointer;transition:background .15s,transform .1s;
}
.key-btn:active{transform:scale(0.95);background:var(--border)}
.key-btn.sm{font-size:14px}

.btn{
  min-height:44px;padding:0 16px;border-radius:12px;border:1px solid var(--border);
  background:var(--card-2);font-size:14px;font-weight:600;cursor:pointer;
  display:inline-flex;align-items:center;justify-content:center;gap:6px;transition:opacity .15s,background .15s;
}
.btn.primary{background:var(--accent);color:#fff;border-color:transparent}
.btn.danger{background:rgba(255,59,48,0.14);color:var(--red);border-color:transparent}
.btn:disabled{opacity:0.45;cursor:not-allowed}

.admin-grid{display:grid;grid-template-columns:1fr;gap:16px}
@media(min-width:900px){.admin-grid{grid-template-columns:1.1fr 1fr}}
.dev-item{
  padding:12px 0;border-bottom:1px solid var(--border);display:flex;flex-wrap:wrap;
  align-items:center;justify-content:space-between;gap:10px;
}
.dev-item:last-child{border-bottom:0}
.tpl-card{
  padding:14px;border-radius:var(--radius-sm);background:var(--card-2);border:1px solid var(--border);margin-bottom:10px;
}
.tpl-row{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:8px}
.field{display:flex;flex-direction:column;gap:4px;font-size:12px;color:var(--muted)}
.field input{
  min-height:44px;padding:0 10px;border-radius:10px;border:1px solid var(--border);
  background:var(--card);color:var(--text);font-size:14px;width:100%;
}
.led-preview-lamp{
  width:18px;height:18px;border-radius:50%;background:var(--card-2);border:1px solid var(--border);
  display:inline-block;vertical-align:middle;transition:background .05s,box-shadow .05s;
}
.range-input{width:100%;min-height:44px;accent-color:var(--accent)}

/* Settings Modal */
.modal-back{
  position:fixed;inset:0;z-index:60;background:rgba(0,0,0,0.42);
  display:none;align-items:center;justify-content:center;padding:16px;
}
.modal-back.open{display:flex}
.modal{width:100%;max-width:420px;background:var(--card);border-radius:var(--radius);padding:22px;box-shadow:var(--shadow);border:1px solid var(--border)}
.modal-head{display:flex;align-items:center;justify-content:space-between;margin-bottom:14px}
.modal-head h2{margin:0;font-size:19px}
.set-row{padding:12px 0;border-bottom:1px solid var(--border);display:flex;align-items:center;justify-content:space-between;gap:10px}
.set-row:last-child{border-bottom:0}

@media(prefers-reduced-motion:reduce){
  *{animation:none!important;transition:none!important}
}
</style>
</head>
<body>

<header class="app-header">
  <div class="header-inner">
    <div class="brand">
      <h1>Wireless Electromagnetic Energy Harvester</h1>
      <div class="brand-sub">
        <span class="dot" id="connDot" aria-hidden="true"></span>
        <span id="modeBadge" class="badge real">Live data from device</span>
        <span id="connText">Connecting</span>
      </div>
    </div>
    <div class="header-actions">
      <div class="seg" role="group" aria-label="Data source mode">
        <button type="button" id="btnModeReal" aria-pressed="true">Real</button>
        <button type="button" id="btnModeSim" aria-pressed="false">Simulated</button>
      </div>
      <button type="button" class="icon-btn" id="btnSettings" aria-label="Settings">
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
          <circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"/>
        </svg>
      </button>
    </div>
  </div>
</header>

<nav class="nav-bar" aria-label="Primary sections">
  <div class="nav-list" role="tablist">
    <button class="nav-btn" role="tab" data-tab="live" aria-selected="true">
      <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M2 12h4l3-7 4 14 3-7h6"/></svg>
      <span>Live</span>
    </button>
    <button class="nav-btn" role="tab" data-tab="how" aria-selected="false">
      <svg viewBox="0 0 24 24" aria-hidden="true"><circle cx="12" cy="12" r="9"/><path d="M9.5 9a2.5 2.5 0 0 1 5 0c0 2-2.5 2-2.5 4"/><circle cx="12" cy="17" r=".6" fill="currentColor"/></svg>
      <span>How it works</span>
    </button>
    <button class="nav-btn" role="tab" data-tab="built" aria-selected="false">
      <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M12 3v18M5 8h7M12 16h7"/><circle cx="12" cy="8" r="2"/><circle cx="12" cy="16" r="2"/></svg>
      <span>How we built it</span>
    </button>
    <button class="nav-btn" role="tab" data-tab="schematics" aria-selected="false">
      <svg viewBox="0 0 24 24" aria-hidden="true"><rect x="3" y="4" width="18" height="16" rx="2"/><path d="M7 12h3l2-4 2 8 2-4h2"/></svg>
      <span>Schematics</span>
    </button>
    <button class="nav-btn" role="tab" data-tab="team" aria-selected="false">
      <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M17 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"/><circle cx="9" cy="7" r="4"/><path d="M23 21v-2a4 4 0 0 0-3-3.87M16 3.13a4 4 0 0 1 0 7.75"/></svg>
      <span>Team</span>
    </button>
    <button class="nav-btn" role="tab" data-tab="admin" aria-selected="false">
      <svg viewBox="0 0 24 24" aria-hidden="true"><rect x="5" y="11" width="14" height="10" rx="2"/><path d="M8 11V7a4 4 0 0 1 8 0v4"/></svg>
      <span>Admin</span>
    </button>
  </div>
</nav>

<main class="main">
  <!-- LIVE TAB -->
  <section class="panel active" id="tab-live" role="tabpanel">
    <div class="status-strip">
      <div>
        <div class="status-main" id="liveStatusLine">Waiting for first scan</div>
        <div class="kpi-sub" id="adcLine" style="display:none;margin-top:4px"></div>
      </div>
      <div class="status-tags">
        <span class="badge" id="scanningTag">Scanning</span>
        <span class="badge" id="tierTag">Guest</span>
        <span class="badge bad" id="heavyTag" style="display:none">Heavy usage</span>
        <span class="badge" id="healthTag">healthy</span>
      </div>
    </div>

    <div class="cards-4">
      <div class="card">
        <div class="kpi-label"><span>RF signal</span><span class="bars" id="sigBars" aria-hidden="true"><span></span><span></span><span></span><span></span><span></span></span></div>
        <div class="kpi-val num" id="kpiStrongest">-- dBm</div>
        <div class="kpi-sub" id="kpiSigQuality">No signal yet</div>
      </div>
      <div class="card">
        <div class="kpi-label"><span>Harvestable power</span><span class="badge">Estimated</span></div>
        <div class="kpi-val num" id="kpiPower">--</div>
        <div class="kpi-sub num" id="kpiTotalPower">Total RF: --</div>
      </div>
      <div class="card">
        <div class="kpi-label"><span>Energy collected</span><span class="badge">Estimated</span></div>
        <div class="kpi-val num" id="kpiEnergy">--</div>
        <div class="kpi-sub num" id="kpiUptime">Uptime: --</div>
      </div>
      <div class="card">
        <div class="kpi-label"><span>Networks found</span></div>
        <div class="kpi-val num" id="kpiCount">--</div>
        <div class="kpi-sub num" id="kpiScanAge">Scan age: --</div>
      </div>
    </div>

    <div class="card chart-card">
      <div class="chart-head">
        <h2 class="chart-title">Harvestable power over time</h2>
        <div class="seg" role="group" aria-label="Chart range">
          <button type="button" class="range-btn" data-range="300" aria-pressed="true">5 min</button>
          <button type="button" class="range-btn" data-range="3600" aria-pressed="false">1 hour</button>
          <button type="button" class="range-btn" data-range="86400" aria-pressed="false">24 hours</button>
        </div>
      </div>
      <div class="canvas-wrap">
        <canvas id="powerCanvas" aria-label="Harvestable power history chart"></canvas>
        <div class="tooltip num" id="chartTip"></div>
      </div>
    </div>

    <div class="card">
      <button type="button" class="collapse-btn" id="btnToggleNets" aria-expanded="false">
        <span>Nearby networks</span>
        <span id="netsChevron" style="color:var(--muted);font-size:13px">Show</span>
      </button>
      <div class="collapse-body" id="netsBody">
        <div style="overflow-x:auto">
          <table class="net-table">
            <thead><tr><th>SSID</th><th>Signal</th><th>Channel</th><th>Security</th></tr></thead>
            <tbody id="netsTbody"></tbody>
          </table>
        </div>
        <div style="margin-top:16px;font-size:13px;font-weight:600;color:var(--muted)">Networks per 2.4 GHz channel (1–13)</div>
        <div class="ch-bars" id="chBars"></div>
      </div>
    </div>
  </section>

  <!-- HOW IT WORKS TAB -->
  <section class="panel" id="tab-how" role="tabpanel">
    <h2 class="page-title">How it works</h2>
    <p class="page-lead">
      Ambient WiFi routers constantly broadcast radio waves that carry small amounts of electromagnetic energy.
      An antenna captures those passing radio waves as high-frequency alternating signals.
      A rectifier circuit turns that alternating RF energy into steady direct current (this stage is planned and not yet built).
      Right now, the ESP8266 board measures real WiFi signal strength around you and estimates how much power is available to harvest.
    </p>
    <div class="card flow-wrap">
      <svg class="flow-svg" viewBox="0 0 880 190" role="img" aria-label="Signal to dashboard energy flow diagram">
        <defs>
          <marker id="arr" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
            <path d="M 0 1 L 10 5 L 0 9 z" fill="var(--accent)"/>
          </marker>
        </defs>
        <path d="M 125 90 L 170 90 M 295 90 L 340 90 M 465 90 L 510 90 M 635 90 L 680 90 M 805 90 L 840 90" stroke="var(--border)" stroke-width="2" fill="none"/>
        <path class="flow-pulse" d="M 125 90 L 166 90 M 295 90 L 336 90 M 465 90 L 506 90 M 635 90 L 676 90 M 805 90 L 836 90" stroke="var(--accent)" stroke-width="2.5" fill="none" marker-end="url(#arr)"/>

        <g transform="translate(10,35)">
          <rect width="115" height="110" rx="18" fill="var(--card-2)" stroke="var(--border)"/>
          <text x="57" y="34" text-anchor="middle" font-size="13" font-weight="700" fill="var(--text)">WiFi waves</text>
          <text id="svgNets" x="57" y="62" text-anchor="middle" font-size="12" fill="var(--accent)">-- nets</text>
          <text id="svgDbm" x="57" y="82" text-anchor="middle" font-size="12" fill="var(--muted)">-- dBm</text>
        </g>
        <g transform="translate(175,35)">
          <rect width="120" height="110" rx="18" fill="var(--card-2)" stroke="var(--border)"/>
          <text x="60" y="34" text-anchor="middle" font-size="13" font-weight="700" fill="var(--text)">Antenna</text>
          <text id="svgRf" x="60" y="64" text-anchor="middle" font-size="12" fill="var(--accent)">--</text>
          <text x="60" y="84" text-anchor="middle" font-size="11" fill="var(--muted)">2.4 GHz RF</text>
        </g>
        <g transform="translate(345,35)">
          <rect width="120" height="110" rx="18" fill="var(--card-2)" stroke="var(--amber)" stroke-dasharray="5 4"/>
          <text x="60" y="34" text-anchor="middle" font-size="13" font-weight="700" fill="var(--text)">Rectifier</text>
          <text x="60" y="58" text-anchor="middle" font-size="11" font-weight="700" fill="var(--amber)">Planned</text>
          <text id="svgEst" x="60" y="82" text-anchor="middle" font-size="12" fill="var(--muted)">--</text>
        </g>
        <g transform="translate(515,35)">
          <rect width="120" height="110" rx="18" fill="var(--card-2)" stroke="var(--border)"/>
          <text x="60" y="34" text-anchor="middle" font-size="13" font-weight="700" fill="var(--text)">Storage cap</text>
          <text id="svgCap" x="60" y="64" text-anchor="middle" font-size="12" fill="var(--accent)">--</text>
          <text x="60" y="84" text-anchor="middle" font-size="11" fill="var(--muted)">Estimated DC</text>
        </g>
        <g transform="translate(685,35)">
          <rect width="120" height="110" rx="18" fill="var(--card-2)" stroke="var(--border)"/>
          <text x="60" y="34" text-anchor="middle" font-size="13" font-weight="700" fill="var(--text)">ESP8266</text>
          <text id="svgHealth" x="60" y="64" text-anchor="middle" font-size="12" fill="var(--green)">healthy</text>
          <text x="60" y="84" text-anchor="middle" font-size="11" fill="var(--muted)">Monitor &amp; AP</text>
        </g>
        <g transform="translate(842,58)">
          <circle cx="16" cy="32" r="14" fill="var(--accent-soft)" stroke="var(--accent)"/>
          <text x="16" y="36" text-anchor="middle" font-size="10" font-weight="700" fill="var(--accent)">UI</text>
        </g>
      </svg>
    </div>
  </section>

  <!-- HOW WE BUILT IT TAB -->
  <section class="panel" id="tab-built" role="tabpanel">
    <h2 class="page-title">How we built it</h2>
    <p class="page-lead">From concept to live WiFi sensing, with an honest view of what is working today and what comes next.</p>
    <div class="timeline">
      <div class="card tl-item"><span class="tl-dot" aria-hidden="true"></span><h3>1. Idea</h3><p>We wanted to make invisible 2.4 GHz radio energy around classrooms and homes measurable, showing how much power WiFi networks emit and how a passive harvester captures it.</p></div>
      <div class="card tl-item"><span class="tl-dot" aria-hidden="true"></span><h3>2. Parts</h3><p>We selected a NodeMCU 1.0 (ESP-12E / ESP8266) powered over USB for WiFi scanning, hosting, and optional analog A0 voltage sensing, alongside plans for Schottky diodes and a storage capacitor.</p></div>
      <div class="card tl-item"><span class="tl-dot" aria-hidden="true"></span><h3>3. Firmware</h3><p>Written in Arduino IDE for ESP8266, the board runs an open WiFi hotspot with a captive portal, non-blocking async RF scans every 8 seconds, priority client slots, rate limiting, and LED health states.</p></div>
      <div class="card tl-item"><span class="tl-dot" aria-hidden="true"></span><h3>4. Dashboard</h3><p>Designed with Claude and coded with Google AI Studio as a single self-contained page stored directly in flash memory so it works offline without any external libraries.</p></div>
      <div class="card tl-item"><span class="tl-dot" aria-hidden="true"></span><h3>5. Testing</h3><p>Verified live 2.4 GHz scans across channels 1 to 13, linear RF power summation, priority/guest slot promotion, and real-time updates over the board's hotspot.</p></div>
      <div class="card tl-item"><span class="tl-dot" aria-hidden="true"></span><h3>6. Next</h3><p>The RF rectifier stage is not built yet. Once low-forward-voltage Schottky diodes arrive, we will assemble the voltage doubler, connect the storage capacitor, and measure real harvested voltage on A0.</p></div>
    </div>
  </section>

  <!-- SCHEMATICS TAB -->
  <section class="panel" id="tab-schematics" role="tabpanel">
    <h2 class="page-title">Schematics</h2>
    <p class="page-lead">Hardware architecture separating what is currently wired from the planned RF rectifier circuit.</p>
    <div class="schem-grid">
      <div class="card">
        <div class="schem-head"><h3>1. System block diagram</h3><span class="badge ok">Built (Monitor) / Planned (RF Stage)</span></div>
        <svg class="schem-svg" viewBox="0 0 760 120" role="img" aria-label="Block diagram">
          <g fill="none" stroke="currentColor" stroke-width="1.5">
            <rect x="10" y="30" width="110" height="54" rx="10"/><text x="65" y="61" text-anchor="middle" fill="currentColor" stroke="none" font-size="13">Antenna</text>
            <line x1="120" y1="57" x2="155" y2="57"/>
            <rect x="155" y="30" width="120" height="54" rx="10" stroke-dasharray="4 3"/><text x="215" y="55" text-anchor="middle" fill="currentColor" stroke="none" font-size="13">Rectifier</text><text x="215" y="72" text-anchor="middle" fill="var(--amber)" stroke="none" font-size="11">Planned</text>
            <line x1="275" y1="57" x2="310" y2="57"/>
            <rect x="310" y="30" width="130" height="54" rx="10" stroke-dasharray="4 3"/><text x="375" y="55" text-anchor="middle" fill="currentColor" stroke="none" font-size="13">Storage Cap</text><text x="375" y="72" text-anchor="middle" fill="var(--amber)" stroke="none" font-size="11">Planned</text>
            <line x1="440" y1="57" x2="475" y2="57"/>
            <rect x="475" y="30" width="120" height="54" rx="10"/><text x="535" y="55" text-anchor="middle" fill="currentColor" stroke="none" font-size="13">ESP8266</text><text x="535" y="72" text-anchor="middle" fill="var(--green)" stroke="none" font-size="11">Built</text>
            <line x1="595" y1="57" x2="630" y2="57"/>
            <rect x="630" y="30" width="120" height="54" rx="10"/><text x="690" y="55" text-anchor="middle" fill="currentColor" stroke="none" font-size="13">Dashboard</text><text x="690" y="72" text-anchor="middle" fill="var(--green)" stroke="none" font-size="11">Built</text>
          </g>
        </svg>
      </div>

      <div class="card">
        <div class="schem-head"><h3>2. Full RF harvester circuit schematic</h3><span class="badge warn">Planned</span></div>
        <svg class="schem-svg" viewBox="0 0 760 230" role="img" aria-label="Full circuit schematic planned">
          <g fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round">
            <!-- Antenna -->
            <line x1="50" y1="50" x2="50" y2="120"/><line x1="35" y1="50" x2="65" y2="50"/><line x1="35" y1="50" x2="50" y2="75"/><line x1="65" y1="50" x2="50" y2="75"/>
            <text x="50" y="36" text-anchor="middle" fill="currentColor" stroke="none" font-size="12">2.4 GHz Antenna</text>
            <!-- Coupling cap -->
            <line x1="50" y1="120" x2="105" y2="120"/><line x1="105" y1="105" x2="105" y2="135"/><line x1="115" y1="105" x2="115" y2="135"/><line x1="115" y1="120" x2="170" y2="120"/>
            <text x="110" y="96" text-anchor="middle" fill="var(--muted)" stroke="none" font-size="11">C1</text>
            <!-- D1 shunt to GND -->
            <circle cx="170" cy="120" r="2.5" fill="currentColor"/>
            <line x1="170" y1="120" x2="170" y2="145"/><polygon points="160,165 180,165 170,145"/><line x1="160" y1="145" x2="180" y2="145"/><line x1="170" y1="165" x2="170" y2="195"/>
            <text x="198" y="160" fill="var(--muted)" stroke="none" font-size="11">D1 Schottky</text>
            <!-- D2 series -->
            <line x1="170" y1="120" x2="225" y2="120"/><polygon points="225,110 225,130 245,120"/><line x1="245" y1="110" x2="245" y2="130"/><line x1="245" y1="120" x2="310" y2="120"/>
            <text x="235" y="100" text-anchor="middle" fill="var(--muted)" stroke="none" font-size="11">D2 Schottky</text>
            <!-- Storage cap -->
            <circle cx="310" cy="120" r="2.5" fill="currentColor"/>
            <line x1="310" y1="120" x2="310" y2="150"/><line x1="296" y1="150" x2="324" y2="150"/><line x1="296" y1="160" x2="324" y2="160"/><line x1="310" y1="160" x2="310" y2="195"/>
            <text x="330" y="158" fill="var(--muted)" stroke="none" font-size="11">C_store</text>
            <!-- Bleed resistor -->
            <line x1="310" y1="120" x2="405" y2="120"/><circle cx="405" cy="120" r="2.5" fill="currentColor"/>
            <line x1="405" y1="120" x2="405" y2="138"/><rect x="398" y="138" width="14" height="36" rx="2"/><line x1="405" y1="174" x2="405" y2="195"/>
            <text x="424" y="160" fill="var(--muted)" stroke="none" font-size="11">R_bleed</text>
            <!-- Divider to A0 -->
            <line x1="405" y1="120" x2="500" y2="120"/><rect x="493" y="128" width="14" height="26" rx="2"/><line x1="500" y1="120" x2="500" y2="128"/><line x1="500" y1="154" x2="500" y2="165"/>
            <circle cx="500" cy="160" r="2.5" fill="currentColor"/>
            <rect x="493" y="165" width="14" height="22" rx="2"/><line x1="500" y1="187" x2="500" y2="195"/>
            <text x="518" y="146" fill="var(--muted)" stroke="none" font-size="11">R1/R2</text>
            <line x1="500" y1="160" x2="585" y2="160"/>
            <!-- Ground rail -->
            <line x1="150" y1="195" x2="585" y2="195"/>
            <!-- NodeMCU -->
            <rect x="585" y="75" width="145" height="135" rx="12"/>
            <text x="657" y="105" text-anchor="middle" fill="currentColor" stroke="none" font-size="13" font-weight="700">NodeMCU</text>
            <text x="600" y="164" fill="currentColor" stroke="none" font-size="12">A0</text>
            <text x="600" y="198" fill="currentColor" stroke="none" font-size="12">GND</text>
            <line x1="657" y1="45" x2="657" y2="75"/>
            <text x="657" y="36" text-anchor="middle" fill="var(--accent)" stroke="none" font-size="12">USB 5V Power</text>
          </g>
        </svg>
      </div>

      <div class="card">
        <div class="schem-head"><h3>3. Current NodeMCU wiring diagram</h3><span class="badge ok">Built</span></div>
        <svg class="schem-svg" viewBox="0 0 760 175" role="img" aria-label="Current wiring diagram built">
          <g fill="none" stroke="currentColor" stroke-width="1.5">
            <rect x="40" y="55" width="120" height="50" rx="10"/><text x="100" y="84" text-anchor="middle" fill="currentColor" stroke="none" font-size="13">USB Power</text>
            <line x1="160" y1="80" x2="260" y2="80" stroke="var(--accent)" stroke-width="2"/>
            <text x="210" y="70" text-anchor="middle" fill="var(--accent)" stroke="none" font-size="11">5V / Data</text>

            <rect x="260" y="28" width="220" height="118" rx="14"/>
            <text x="370" y="60" text-anchor="middle" fill="currentColor" stroke="none" font-size="14" font-weight="700">NodeMCU 1.0 (ESP-12E)</text>
            <text x="370" y="84" text-anchor="middle" fill="var(--muted)" stroke="none" font-size="12">Onboard WiFi Antenna + LED</text>
            <text x="466" y="114" text-anchor="end" fill="currentColor" stroke="none" font-size="12">A0</text>
            <text x="274" y="132" fill="var(--muted)" stroke="none" font-size="11">3V3 / VIN (NC)</text>

            <line x1="480" y1="110" x2="575" y2="110" stroke-dasharray="4 3"/>
            <rect x="575" y="84" width="150" height="52" rx="10" stroke-dasharray="4 3"/>
            <text x="650" y="108" text-anchor="middle" fill="currentColor" stroke="none" font-size="12">Harvester Sense</text>
            <text x="650" y="124" text-anchor="middle" fill="var(--muted)" stroke="none" font-size="11">Optional A0 input</text>
          </g>
        </svg>
        <div class="note-box">Note: 3V3 and VIN are not connected to the harvester. The board is powered exclusively via USB, and A0 is an optional sense input only.</div>
      </div>
    </div>
  </section>

  <!-- TEAM TAB -->
  <section class="panel" id="tab-team" role="tabpanel">
    <h2 class="page-title">Team</h2>
    <p class="page-lead">Built collaboratively by the project team.</p>
    <div class="team-grid">
      <div class="card team-card">Hanish Choubisa</div>
      <div class="card team-card">Dhruv Thiyya</div>
      <div class="card team-card">Harshil Pandya</div>
      <div class="card team-card">Harshit Borad</div>
      <div class="card team-card">Darshil Bhalala</div>
    </div>
  </section>

  <!-- ADMIN TAB -->
  <section class="panel" id="tab-admin" role="tabpanel">
    <div id="adminLockView" class="card keypad-wrap">
      <h2 style="margin:0 0 4px">Admin Access</h2>
      <p style="margin:0;color:var(--muted);font-size:13.5px" id="pinPromptText">Enter 4-digit PIN to manage devices and LED templates</p>
      <div class="pin-dots" aria-hidden="true">
        <span class="pin-dot"></span><span class="pin-dot"></span><span class="pin-dot"></span><span class="pin-dot"></span>
      </div>
      <div id="pinErr" style="min-height:20px;font-size:13px;color:var(--red);margin-bottom:10px"></div>
      <div class="keypad" id="keypad">
        <button type="button" class="key-btn" data-k="1">1</button>
        <button type="button" class="key-btn" data-k="2">2</button>
        <button type="button" class="key-btn" data-k="3">3</button>
        <button type="button" class="key-btn" data-k="4">4</button>
        <button type="button" class="key-btn" data-k="5">5</button>
        <button type="button" class="key-btn" data-k="6">6</button>
        <button type="button" class="key-btn" data-k="7">7</button>
        <button type="button" class="key-btn" data-k="8">8</button>
        <button type="button" class="key-btn" data-k="9">9</button>
        <button type="button" class="key-btn sm" data-k="clr">Clear</button>
        <button type="button" class="key-btn" data-k="0">0</button>
        <button type="button" class="key-btn sm" data-k="del">Back</button>
      </div>
    </div>

    <div id="adminDashView" style="display:none">
      <div class="status-strip">
        <div>
          <div class="status-main">Board Administration</div>
          <div style="font-size:13px;color:var(--muted)" id="adminMsg">Authenticated session active</div>
        </div>
        <button type="button" class="btn danger" id="btnLogout">Log out</button>
      </div>

      <div class="admin-grid">
        <div class="card">
          <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:10px">
            <h3 style="margin:0">Connected devices</h3>
            <button type="button" class="btn" id="btnRefreshDevs">Refresh</button>
          </div>
          <div id="devList"></div>
          <h4 style="margin:18px 0 8px;font-size:14px;color:var(--muted)">Blocked MACs</h4>
          <div id="blockList"></div>
        </div>

        <div class="card">
          <div style="display:flex;align-items:center;justify-content:space-between;gap:8px;margin-bottom:12px">
            <h3 style="margin:0">LED Control <span class="led-preview-lamp" id="ledLamp" title="Dashboard LED preview"></span></h3>
            <div class="seg" role="group" aria-label="LED mode">
              <button type="button" id="btnLedSys" aria-pressed="true">System mode</button>
              <button type="button" id="btnLedAdm" aria-pressed="false">Admin mode</button>
            </div>
          </div>

          <div style="margin-bottom:14px">
            <div style="display:flex;justify-content:space-between;font-size:13px;color:var(--muted)">
              <span>Master brightness</span><span class="num" id="masterVal">100%</span>
            </div>
            <input type="range" class="range-input" id="masterRange" min="0" max="100" value="100" aria-label="Master brightness">
          </div>

          <div id="tplWrap"></div>
        </div>
      </div>
    </div>
  </section>
</main>

<!-- SETTINGS MODAL -->
<div class="modal-back" id="settingsModal" role="dialog" aria-modal="true" aria-labelledby="settingsTitle">
  <div class="modal">
    <div class="modal-head">
      <h2 id="settingsTitle">Settings</h2>
      <button type="button" class="icon-btn" id="btnCloseSettings" aria-label="Close settings">&times;</button>
    </div>
    <div class="set-row">
      <span>Appearance</span>
      <div class="seg" role="group" aria-label="Theme override">
        <button type="button" class="theme-btn" data-th="auto" aria-pressed="true">Auto</button>
        <button type="button" class="theme-btn" data-th="light" aria-pressed="false">Light</button>
        <button type="button" class="theme-btn" data-th="dark" aria-pressed="false">Dark</button>
      </div>
    </div>
    <div class="set-row">
      <div>
        <div>Chart history (<span id="histModeLbl">Real</span>)</div>
        <div style="font-size:12px;color:var(--muted)">Clears only the active mode</div>
      </div>
      <button type="button" class="btn danger" id="btnClearHist">Clear data</button>
    </div>
    <div class="set-row" id="resetBoardRow">
      <div>
        <div>Device energy counter</div>
        <div style="font-size:12px;color:var(--muted)">Resets collected energy on ESP8266</div>
      </div>
      <button type="button" class="btn" id="btnResetBoard">Reset</button>
    </div>
  </div>
</div>

<script>
(() => {
  "use strict";

  // Single application state object
  const state = {
    mode: "real",
    tab: "live",
    rangeSec: 300,
    theme: "auto",
    connected: false,
    reconnecting: false,
    netsOpen: false,
    data: null,
    history: { real: [], sim: [] },
    simEnergy: 0.42,
    simTick: 0,
    admin: {
      pin: "",
      token: "",
      wrongCount: 0,
      lockUntil: 0,
      ledMode: "system",
      ledSlot: 1,
      master: 100,
      previewSlot: null,
      devices: [
        { mac: "A4:83:E7:21:9B:10", first_seen_seconds: 142, request_count: 68, window_count: 3, is_priority: true, is_heavy: false },
        { mac: "DC:A6:32:7F:44:02", first_seen_seconds: 95, request_count: 41, window_count: 2, is_priority: true, is_heavy: false },
        { mac: "5C:CF:7F:10:8E:55", first_seen_seconds: 28, request_count: 19, window_count: 1, is_priority: false, is_heavy: false }
      ],
      blocklist: ["00:1A:2B:3C:4D:5E"],
      templates: [
        { name: "Heartbeat", blinks: 2, cycle: 2.0, brightness: 100 },
        { name: "Sleepy", blinks: 1, cycle: 4.0, brightness: 30 },
        { name: "Morse SOS", blinks: 3, cycle: 3.0, brightness: 100 },
        { name: "Breathing", blinks: 1, cycle: 3.0, brightness: 60 },
        { name: "Alarm", blinks: 6, cycle: 1.5, brightness: 100 }
      ]
    },
    hoverIdx: -1
  };

  const LS_MODE = "harvester_mode_v1";
  const LS_THEME = "harvester_theme_v1";
  const LS_HIST_REAL = "harvester_hist_real_v1";
  const LS_HIST_SIM = "harvester_hist_sim_v1";
  const SIM_SSIDS = ["CampusNet-24", "LabRouter_5", "HomeWiFi", "GuestAccess", "SensorMesh", "OfficeLink", "WorkshopAP", "IoT_Gateway"];

  const $ = (id) => document.getElementById(id);

  // Formatting helpers
  function fmtPowerUw(uw) {
    if (!isFinite(uw) || uw < 0) return "--";
    if (uw < 1) return (uw * 1000).toFixed(1) + " nW";
    if (uw < 1000) return uw.toFixed(3) + " \u00B5W";
    return (uw / 1000).toFixed(3) + " mW";
  }
  function fmtEnergyUj(uj) {
    if (!isFinite(uj) || uj < 0) return "--";
    if (uj < 1) return (uj * 1000).toFixed(1) + " nJ";
    if (uj < 1000) return uj.toFixed(2) + " \u00B5J";
    return (uj / 1000).toFixed(3) + " mJ";
  }
  function fmtDuration(sec) {
    sec = Math.max(0, Math.floor(sec || 0));
    const h = Math.floor(sec / 3600);
    const m = Math.floor((sec % 3600) / 60);
    const s = sec % 60;
    if (h > 0) return h + "h " + m + "m";
    if (m > 0) return m + "m " + s + "s";
    return s + "s";
  }
  function escHtml(s) {
    return String(s).replace(/[&<>"']/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", "\"": "&quot;", "'": "&#39;" }[c]));
  }

  // Storage helpers
  function loadStorage() {
    try {
      const savedMode = localStorage.getItem(LS_MODE);
      if (savedMode === "real" || savedMode === "sim") {
        state.mode = savedMode;
      } else {
        state.mode = location.protocol === "file:" ? "sim" : "real";
      }
      const savedTheme = localStorage.getItem(LS_THEME);
      if (savedTheme === "auto" || savedTheme === "light" || savedTheme === "dark") state.theme = savedTheme;

      const hr = JSON.parse(localStorage.getItem(LS_HIST_REAL) || "[]");
      const hs = JSON.parse(localStorage.getItem(LS_HIST_SIM) || "[]");
      if (Array.isArray(hr)) state.history.real = hr;
      if (Array.isArray(hs)) state.history.sim = hs;
    } catch (_) {}
  }
  function saveHistory(mode) {
    try {
      const key = mode === "real" ? LS_HIST_REAL : LS_HIST_SIM;
      const cutoff = Date.now() - 86400 * 1000;
      let arr = state.history[mode].filter((p) => p && p.t >= cutoff);
      if (arr.length > 1200) arr = arr.slice(arr.length - 1200);
      state.history[mode] = arr;
      localStorage.setItem(key, JSON.stringify(arr));
    } catch (_) {}
  }

  // Theme
  function applyTheme() {
    const root = document.documentElement;
    if (state.theme === "auto") root.removeAttribute("data-theme");
    else root.setAttribute("data-theme", state.theme);
    document.querySelectorAll(".theme-btn").forEach((b) => {
      b.setAttribute("aria-pressed", String(b.dataset.th === state.theme));
    });
  }

  // Simulated generator matching exact ESP8266 JSON shape
  function generateSimData() {
    state.simTick += 1;
    const netCount = 3 + Math.floor(Math.random() * 6);
    const ch = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    const used = new Set();
    const nets = [];
    let totalUw = 0;

    for (let i = 0; i < netCount; i++) {
      let name = SIM_SSIDS[(i + state.simTick) % SIM_SSIDS.length];
      if (used.has(name)) name += "_" + i;
      used.add(name);
      const rssi = -35 - Math.floor(Math.random() * 46); // -35..-80 dBm
      const c = [1, 6, 11, 3, 9, 4, 8][i % 7];
      ch[c - 1] += 1;
      totalUw += Math.pow(10, rssi / 10) * 1000;
      nets.push({ s: name, r: rssi, c, e: i % 4 === 0 ? 0 : 1 });
    }
    nets.sort((a, b) => b.r - a.r);
    const strongest = nets.length ? nets[0].r : -100;
    const harvestUw = totalUw * 0.20;
    state.simEnergy += harvestUw * 2.0;

    return {
      strongest,
      total_uw: Number(totalUw.toFixed(6)),
      harvest_uw: Number(harvestUw.toFixed(6)),
      energy_uj: Number(state.simEnergy.toFixed(3)),
      count: netCount,
      channels: ch,
      adc_v: Number((0.18 + Math.sin(state.simTick / 5) * 0.04).toFixed(2)),
      adc_ok: state.simTick % 7 !== 0,
      up: state.simTick * 2 + 120,
      scan_age: (state.simTick * 2) % 8,
      priority: true,
      heavy: false,
      led_mode: state.admin.ledMode,
      health: "healthy",
      nets
    };
  }

  // Polling loop
  let pollTimer = null;
  async function pollOnce() {
    if (document.hidden) return;
    if (state.mode === "sim") {
      const d = generateSimData();
      state.data = d;
      state.connected = true;
      state.reconnecting = false;
      state.history.sim.push({ t: Date.now(), v: d.harvest_uw });
      saveHistory("sim");
      renderAll();
      return;
    }

    try {
      const res = await fetch("/api/data", { cache: "no-store" });
      if (!res.ok) throw new Error("HTTP " + res.status);
      const d = await res.json();
      state.data = d;
      state.connected = true;
      state.reconnecting = false;
      state.history.real.push({ t: Date.now(), v: d.harvest_uw });
      saveHistory("real");
    } catch (_) {
      state.connected = false;
      state.reconnecting = true;
    }
    renderAll();
  }

  function startPolling() {
    if (pollTimer) clearInterval(pollTimer);
    pollOnce();
    pollTimer = setInterval(pollOnce, 2000);
  }

  // Render Header
  function renderHeader() {
    const isSim = state.mode === "sim";
    $("btnModeReal").setAttribute("aria-pressed", String(!isSim));
    $("btnModeSim").setAttribute("aria-pressed", String(isSim));
    $("histModeLbl").textContent = isSim ? "Simulated" : "Real";
    $("resetBoardRow").style.display = isSim ? "none" : "flex";

    const badge = $("modeBadge");
    const dot = $("connDot");
    const txt = $("connText");

    if (isSim) {
      badge.textContent = "Simulated data";
      badge.className = "badge sim";
      dot.className = "dot warn";
      txt.textContent = "Local simulation";
    } else {
      badge.textContent = "Live data from device";
      badge.className = "badge real";
      if (state.connected) {
        dot.className = "dot ok";
        txt.textContent = "Connected";
      } else {
        dot.className = "dot bad";
        txt.textContent = "Reconnecting";
      }
    }
  }

  // Render Live Tab
  function renderLive() {
    const d = state.data;
    if (!d) {
      $("liveStatusLine").textContent = state.mode === "real" && state.reconnecting ? "Reconnecting" : "Waiting for first scan";
      return;
    }

    let statusText = "Weak signal";
    if (state.mode === "real" && state.reconnecting) {
      statusText = "Reconnecting";
    } else if (d.scan_age > 30) {
      statusText = "Waiting for first scan";
    } else if (d.strongest > -55) {
      statusText = "Strong signal around you";
    } else if (d.strongest >= -70) {
      statusText = "Moderate signal around you";
    }
    $("liveStatusLine").textContent = statusText;

    const adcEl = $("adcLine");
    if (d.adc_ok) {
      adcEl.style.display = "flex";
      adcEl.textContent = "Scanning \u2022 Capacitor voltage: " + Number(d.adc_v).toFixed(2) + " V";
    } else {
      adcEl.style.display = "none";
    }

    $("scanningTag").textContent = d.scan_age <= 2 ? "Scanning" : "Idle (" + d.scan_age + "s)";
    $("tierTag").textContent = d.priority ? "Priority" : "Guest";
    $("tierTag").className = d.priority ? "badge real" : "badge";
    $("heavyTag").style.display = d.heavy ? "inline-flex" : "none";

    const hTag = $("healthTag");
    hTag.textContent = d.health || "healthy";
    hTag.className = "badge " + (d.health === "healthy" ? "ok" : (d.health === "trouble" || d.health === "low_memory" ? "bad" : "warn"));

    // 5-bar signal meter
    const r = Number(d.strongest);
    let bars = 1, q = "Very weak";
    if (r > -50) { bars = 5; q = "Excellent"; }
    else if (r >= -60) { bars = 4; q = "Good"; }
    else if (r >= -70) { bars = 3; q = "Fair"; }
    else if (r >= -80) { bars = 2; q = "Weak"; }

    $("kpiStrongest").textContent = r + " dBm";
    $("kpiSigQuality").textContent = q;
    const barEls = $("sigBars").children;
    for (let i = 0; i < 5; i++) barEls[i].classList.toggle("on", i < bars);

    $("kpiPower").textContent = fmtPowerUw(d.harvest_uw);
    $("kpiTotalPower").textContent = "Total RF: " + fmtPowerUw(d.total_uw);
    $("kpiEnergy").textContent = fmtEnergyUj(d.energy_uj);
    $("kpiUptime").textContent = "Uptime: " + fmtDuration(d.up);
    $("kpiCount").textContent = String(d.count);
    $("kpiScanAge").textContent = "Scan age: " + d.scan_age + "s";

    // Nearby networks & channel bars
    if (state.netsOpen) {
      const tbody = $("netsTbody");
      if (Array.isArray(d.nets) && d.nets.length) {
        tbody.innerHTML = d.nets.map((n) =>
          "<tr><td>" + escHtml(n.s || "(hidden)") + "</td><td class=\"num\">" + n.r + " dBm</td><td class=\"num\">" + n.c + "</td><td>" + (n.e ? "Protected" : "Open") + "</td></tr>"
        ).join("");
      } else {
        tbody.innerHTML = "<tr><td colspan=\"4\" style=\"color:var(--muted)\">No networks detected yet</td></tr>";
      }

      const ch = Array.isArray(d.channels) && d.channels.length === 13 ? d.channels : new Array(13).fill(0);
      const maxCh = Math.max(1, ...ch);
      $("chBars").innerHTML = ch.map((cnt, idx) => {
        const pct = Math.round((cnt / maxCh) * 100);
        return "<div class=\"ch-col\"><div class=\"ch-fill\" style=\"height:" + Math.max(4, pct) + "%\" title=\"Ch " + (idx + 1) + ": " + cnt + "\"></div><span class=\"ch-lbl num\">" + (idx + 1) + "</span></div>";
      }).join("");
    }

    drawChart();
  }

  // Render How It Works live values
  function renderHow() {
    const d = state.data;
    if (!d) return;
    $("svgNets").textContent = d.count + " nets";
    $("svgDbm").textContent = d.strongest + " dBm";
    $("svgRf").textContent = fmtPowerUw(d.total_uw);
    $("svgEst").textContent = fmtPowerUw(d.harvest_uw);
    $("svgCap").textContent = d.adc_ok ? (Number(d.adc_v).toFixed(2) + " V") : fmtEnergyUj(d.energy_uj);
    $("svgHealth").textContent = d.health || "healthy";
  }

  // Custom Canvas Chart
  let chartPoints = [];
  function drawChart() {
    const canvas = $("powerCanvas");
    if (!canvas || !canvas.parentElement) return;
    const rect = canvas.parentElement.getBoundingClientRect();
    if (rect.width < 10 || rect.height < 10) return;

    const dpr = window.devicePixelRatio || 1;
    canvas.width = Math.round(rect.width * dpr);
    canvas.height = Math.round(rect.height * dpr);
    const ctx = canvas.getContext("2d");
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

    const w = rect.width, h = rect.height;
    const padL = 12, padR = 12, padT = 16, padB = 26;
    const pw = w - padL - padR, ph = h - padT - padB;
    ctx.clearRect(0, 0, w, h);

    const styles = getComputedStyle(document.documentElement);
    const accent = styles.getPropertyValue("--accent").trim() || "#007AFF";
    const muted = styles.getPropertyValue("--muted").trim() || "#8E8E93";
    const border = styles.getPropertyValue("--border").trim() || "rgba(0,0,0,0.08)";

    const now = Date.now();
    const start = now - state.rangeSec * 1000;
    const raw = (state.history[state.mode] || []).filter((p) => p.t >= start);

    // Grid lines
    ctx.strokeStyle = border;
    ctx.lineWidth = 1;
    for (let i = 0; i <= 3; i++) {
      const y = padT + (ph * i) / 3;
      ctx.beginPath();
      ctx.moveTo(padL, y);
      ctx.lineTo(w - padR, y);
      ctx.stroke();
    }

    if (raw.length < 2) {
      chartPoints = [];
      ctx.fillStyle = muted;
      ctx.font = "13px -apple-system, sans-serif";
      ctx.textAlign = "center";
      ctx.fillText(raw.length ? "Collecting samples\u2026" : "No history in this range yet", w / 2, h / 2);
      return;
    }

    const vals = raw.map((p) => p.v);
    let minV = Math.min(...vals);
    let maxV = Math.max(...vals);
    if (maxV - minV < 1e-9) { maxV = minV + 0.001; minV = Math.max(0, minV - 0.0005); }
    else { const pad = (maxV - minV) * 0.15; minV = Math.max(0, minV - pad); maxV += pad; }

    chartPoints = raw.map((p) => {
      const x = padL + ((p.t - start) / (state.rangeSec * 1000)) * pw;
      const y = padT + ph - ((p.v - minV) / (maxV - minV)) * ph;
      return { x, y, t: p.t, v: p.v };
    });

    // Smooth path
    ctx.beginPath();
    ctx.moveTo(chartPoints[0].x, chartPoints[0].y);
    for (let i = 0; i < chartPoints.length - 1; i++) {
      const xc = (chartPoints[i].x + chartPoints[i + 1].x) / 2;
      const yc = (chartPoints[i].y + chartPoints[i + 1].y) / 2;
      ctx.quadraticCurveTo(chartPoints[i].x, chartPoints[i].y, xc, yc);
    }
    const last = chartPoints[chartPoints.length - 1];
    ctx.lineTo(last.x, last.y);

    // Fill gradient
    const grad = ctx.createLinearGradient(0, padT, 0, padT + ph);
    grad.addColorStop(0, accent + "44");
    grad.addColorStop(1, accent + "02");
    ctx.save();
    ctx.lineTo(last.x, padT + ph);
    ctx.lineTo(chartPoints[0].x, padT + ph);
    ctx.closePath();
    ctx.fillStyle = grad;
    ctx.fill();
    ctx.restore();

    // Stroke line
    ctx.strokeStyle = accent;
    ctx.lineWidth = 2.2;
    ctx.stroke();

    // Axis labels
    ctx.fillStyle = muted;
    ctx.font = "11px -apple-system, sans-serif";
    ctx.textAlign = "left";
    ctx.fillText(fmtPowerUw(maxV), padL, padT + 10);
    ctx.fillText(fmtPowerUw(minV), padL, padT + ph - 4);
    ctx.textAlign = "right";
    ctx.fillText("Now", w - padR, h - 6);

    // Hover point
    if (state.hoverIdx >= 0 && state.hoverIdx < chartPoints.length) {
      const pt = chartPoints[state.hoverIdx];
      ctx.beginPath();
      ctx.arc(pt.x, pt.y, 5, 0, Math.PI * 2);
      ctx.fillStyle = accent;
      ctx.fill();
      ctx.lineWidth = 2;
      ctx.strokeStyle = "#fff";
      ctx.stroke();
    }
  }

  function handleChartPointer(clientX) {
    if (!chartPoints.length) return;
    const rect = $("powerCanvas").getBoundingClientRect();
    const x = clientX - rect.left;
    let best = 0, bestDist = Infinity;
    for (let i = 0; i < chartPoints.length; i++) {
      const d = Math.abs(chartPoints[i].x - x);
      if (d < bestDist) { bestDist = d; best = i; }
    }
    state.hoverIdx = best;
    const pt = chartPoints[best];
    const tip = $("chartTip");
    const tm = new Date(pt.t).toLocaleTimeString([], { hour: "2-digit", minute: "2-digit", second: "2-digit" });
    tip.textContent = fmtPowerUw(pt.v) + " \u2022 " + tm;
    tip.style.left = Math.min(rect.width - 60, Math.max(60, pt.x)) + "px";
    tip.style.top = Math.max(28, pt.y) + "px";
    tip.style.opacity = "1";
    drawChart();
  }

  // Admin API & UI
  async function adminFetch(url, opts = {}) {
    if (state.mode === "sim") return null;
    const headers = Object.assign({}, opts.headers || {}, { "X-Token": state.admin.token });
    const res = await fetch(url, Object.assign({}, opts, { headers }));
    if (res.status === 401) {
      state.admin.token = "";
      $("pinErr").textContent = "Session expired or unauthorized (401).";
      renderAdmin();
      throw new Error("401");
    }
    if (res.status === 423) {
      state.admin.lockUntil = Date.now() + 60000;
      state.admin.token = "";
      renderAdmin();
      throw new Error("423");
    }
    return res;
  }

  async function submitPin() {
    const now = Date.now();
    if (state.admin.lockUntil > now) return;
    const pin = state.admin.pin;
    state.admin.pin = "";
    renderPinDots();

    if (state.mode === "sim") {
      if (pin === "9610") {
        state.admin.token = "simtoken";
        state.admin.wrongCount = 0;
        $("pinErr").textContent = "";
      } else {
        state.admin.wrongCount += 1;
        if (state.admin.wrongCount >= 5) {
          state.admin.wrongCount = 0;
          state.admin.lockUntil = now + 60000;
        } else {
          $("pinErr").textContent = "Wrong PIN (" + state.admin.wrongCount + "/5)";
        }
      }
      renderAdmin();
      return;
    }

    try {
      const res = await fetch("/api/admin/login?pin=" + encodeURIComponent(pin), { method: "POST" });
      if (res.status === 423) {
        state.admin.lockUntil = Date.now() + 60000;
        $("pinErr").textContent = "Locked for 60 seconds (423).";
      } else if (res.status === 401) {
        $("pinErr").textContent = "Invalid PIN (401).";
      } else if (!res.ok) {
        $("pinErr").textContent = "Error (" + res.status + ").";
      } else {
        const j = await res.json();
        if (j.ok && j.token) {
          state.admin.token = j.token;
          $("pinErr").textContent = "";
          await refreshDevices();
        }
      }
    } catch (_) {
      $("pinErr").textContent = "Board unreachable.";
    }
    renderAdmin();
  }

  async function refreshDevices() {
    if (!state.admin.token || state.mode === "sim") return;
    try {
      const res = await adminFetch("/api/admin/devices");
      if (!res || !res.ok) return;
      const j = await res.json();
      if (Array.isArray(j.devices)) state.admin.devices = j.devices;
      if (Array.isArray(j.blocklist)) state.admin.blocklist = j.blocklist;
      renderAdminDevices();
    } catch (_) {}
  }

  function renderPinDots() {
    const dots = document.querySelectorAll(".pin-dot");
    dots.forEach((d, i) => d.classList.toggle("filled", i < state.admin.pin.length));
  }

  function renderAdminDevices() {
    const devEl = $("devList");
    if (!state.admin.devices.length) {
      devEl.innerHTML = "<div style=\"color:var(--muted);font-size:14px\">No stations connected</div>";
    } else {
      devEl.innerHTML = state.admin.devices.map((d) =>
        "<div class=\"dev-item\"><div>" +
        "<div class=\"num\" style=\"font-weight:600\">" + (d.is_priority ? "\u2605 " : "") + escHtml(d.mac) +
        (d.is_heavy ? " <span class=\"badge bad\">Heavy</span>" : "") + "</div>" +
        "<div class=\"num\" style=\"font-size:12.5px;color:var(--muted)\">Connected " + fmtDuration(d.first_seen_seconds) + " \u2022 " + d.request_count + " reqs</div>" +
        "</div><div style=\"display:flex;gap:6px\">" +
        "<button type=\"button\" class=\"btn\" data-act=\"rem\" data-mac=\"" + escHtml(d.mac) + "\">Remove</button>" +
        "<button type=\"button\" class=\"btn danger\" data-act=\"blk\" data-mac=\"" + escHtml(d.mac) + "\">Block</button>" +
        "</div></div>"
      ).join("");
    }

    const blkEl = $("blockList");
    if (!state.admin.blocklist.length) {
      blkEl.innerHTML = "<div style=\"color:var(--muted);font-size:13px\">Blocklist is empty</div>";
    } else {
      blkEl.innerHTML = state.admin.blocklist.map((m) =>
        "<div class=\"dev-item\"><span class=\"num\" style=\"font-size:13.5px\">" + escHtml(m) + "</span>" +
        "<button type=\"button\" class=\"btn\" data-act=\"unblk\" data-mac=\"" + escHtml(m) + "\">Unblock</button></div>"
      ).join("");
    }
  }

  function renderAdminTemplates() {
    $("btnLedSys").setAttribute("aria-pressed", String(state.admin.ledMode === "system"));
    $("btnLedAdm").setAttribute("aria-pressed", String(state.admin.ledMode === "admin"));
    $("masterRange").value = String(state.admin.master);
    $("masterVal").textContent = state.admin.master + "%";

    const wrap = $("tplWrap");
    wrap.style.display = state.admin.ledMode === "admin" ? "block" : "none";
    wrap.innerHTML = state.admin.templates.map((t, idx) => {
      const slot = idx + 1;
      const active = state.admin.ledSlot === slot;
      return "<div class=\"tpl-card\">" +
        "<div style=\"display:flex;justify-content:space-between;align-items:center\">" +
        "<strong>Slot " + slot + (active ? " (Active)" : "") + "</strong>" +
        "<button type=\"button\" class=\"btn\" data-sel-slot=\"" + slot + "\">" + (active ? "Selected" : "Use slot") + "</button>" +
        "</div>" +
        "<div class=\"tpl-row\">" +
        "<label class=\"field\">Name<input type=\"text\" maxlength=\"16\" data-f=\"name\" data-slot=\"" + slot + "\" value=\"" + escHtml(t.name) + "\"></label>" +
        "<label class=\"field\">Blinks (1-10)<input type=\"number\" min=\"1\" max=\"10\" data-f=\"blinks\" data-slot=\"" + slot + "\" value=\"" + t.blinks + "\"></label>" +
        "<label class=\"field\">Cycle s (0.5-10)<input type=\"number\" min=\"0.5\" max=\"10\" step=\"0.1\" data-f=\"cycle\" data-slot=\"" + slot + "\" value=\"" + t.cycle + "\"></label>" +
        "<label class=\"field\">Brightness %<input type=\"number\" min=\"0\" max=\"100\" data-f=\"brightness\" data-slot=\"" + slot + "\" value=\"" + t.brightness + "\"></label>" +
        "</div>" +
        "<div style=\"display:flex;gap:8px;margin-top:10px\">" +
        "<button type=\"button\" class=\"btn primary\" data-save-slot=\"" + slot + "\">Save</button>" +
        "<button type=\"button\" class=\"btn\" data-prev-slot=\"" + slot + "\">" + (state.admin.previewSlot === slot ? "Stop preview" : "Preview") + "</button>" +
        "</div></div>";
    }).join("");
  }

  function renderAdmin() {
    const locked = state.admin.lockUntil > Date.now();
    const authed = Boolean(state.admin.token) && !locked;
    $("adminLockView").style.display = authed ? "none" : "block";
    $("adminDashView").style.display = authed ? "block" : "none";
    if (!authed) {
      if (locked) {
        const rem = Math.ceil((state.admin.lockUntil - Date.now()) / 1000);
        $("pinErr").textContent = "Too many attempts. Locked for " + rem + "s";
      }
      renderPinDots();
      return;
    }
    $("adminMsg").textContent = state.mode === "sim" ? "Simulated admin controls (board not called)" : "Authenticated with board";
    renderAdminDevices();
    renderAdminTemplates();
  }

  // Dashboard-only LED preview animator
  function tickLedLamp() {
    const lamp = $("ledLamp");
    if (!lamp) return;
    const now = Date.now();
    if (state.admin.lockUntil > now && state.tab === "admin" && !state.admin.token) {
      const rem = Math.ceil((state.admin.lockUntil - now) / 1000);
      $("pinErr").textContent = "Too many attempts. Locked for " + rem + "s";
    } else if (state.admin.lockUntil && state.admin.lockUntil <= now) {
      state.admin.lockUntil = 0;
      $("pinErr").textContent = "";
    }

    let on = false, alpha = 0;
    if (state.admin.previewSlot) {
      const t = state.admin.templates[state.admin.previewSlot - 1];
      const b = Math.max(1, Math.min(10, Number(t.blinks) || 1));
      const cycMs = Math.max(b * 100, Math.round((Number(t.cycle) || 1) * 1000));
      const slotMs = Math.max(50, Math.min(150, Math.floor(cycMs / (b * 2))));
      const period = slotMs * 2;
      const pos = now % cycMs;
      on = pos < b * period && (pos % period) < slotMs;
      alpha = on ? ((Number(t.brightness) * state.admin.master) / 10000) : 0;
    }
    if (alpha > 0) {
      lamp.style.background = "rgba(10,132,255," + Math.max(0.2, alpha).toFixed(2) + ")";
      lamp.style.boxShadow = "0 0 10px rgba(10,132,255," + Math.max(0.25, alpha).toFixed(2) + ")";
    } else {
      lamp.style.background = "var(--card-2)";
      lamp.style.boxShadow = "none";
    }
  }

  function renderAll() {
    renderHeader();
    renderLive();
    renderHow();
    if (state.tab === "admin") renderAdmin();
  }

  // Tab switching
  function setTab(tab) {
    state.tab = tab;
    document.querySelectorAll(".nav-btn").forEach((b) => {
      b.setAttribute("aria-selected", String(b.dataset.tab === tab));
    });
    document.querySelectorAll(".panel").forEach((p) => {
      p.classList.toggle("active", p.id === "tab-" + tab);
    });
    if (tab === "live") drawChart();
    if (tab === "admin") { renderAdmin(); refreshDevices(); }
  }

  // Event wiring
  function bindEvents() {
    document.querySelectorAll(".nav-btn").forEach((b) => {
      b.addEventListener("click", () => setTab(b.dataset.tab));
    });

    $("btnModeReal").addEventListener("click", () => {
      state.mode = "real";
      localStorage.setItem(LS_MODE, "real");
      state.data = null;
      startPolling();
      renderAll();
    });
    $("btnModeSim").addEventListener("click", () => {
      state.mode = "sim";
      localStorage.setItem(LS_MODE, "sim");
      startPolling();
      renderAll();
    });

    document.querySelectorAll(".range-btn").forEach((b) => {
      b.addEventListener("click", () => {
        state.rangeSec = Number(b.dataset.range);
        document.querySelectorAll(".range-btn").forEach((x) => x.setAttribute("aria-pressed", String(x === b)));
        state.hoverIdx = -1;
        $("chartTip").style.opacity = "0";
        drawChart();
      });
    });

    $("btnToggleNets").addEventListener("click", () => {
      state.netsOpen = !state.netsOpen;
      $("btnToggleNets").setAttribute("aria-expanded", String(state.netsOpen));
      $("netsBody").classList.toggle("open", state.netsOpen);
      $("netsChevron").textContent = state.netsOpen ? "Hide" : "Show";
      if (state.netsOpen) renderLive();
    });

    // Chart tooltip
    const cv = $("powerCanvas");
    cv.addEventListener("mousemove", (e) => handleChartPointer(e.clientX));
    cv.addEventListener("touchstart", (e) => { if (e.touches[0]) handleChartPointer(e.touches[0].clientX); }, { passive: true });
    cv.addEventListener("touchmove", (e) => { if (e.touches[0]) handleChartPointer(e.touches[0].clientX); }, { passive: true });
    cv.addEventListener("mouseleave", () => { state.hoverIdx = -1; $("chartTip").style.opacity = "0"; drawChart(); });

    // Settings modal
    $("btnSettings").addEventListener("click", () => $("settingsModal").classList.add("open"));
    $("btnCloseSettings").addEventListener("click", () => $("settingsModal").classList.remove("open"));
    $("settingsModal").addEventListener("click", (e) => { if (e.target === $("settingsModal")) $("settingsModal").classList.remove("open"); });

    document.querySelectorAll(".theme-btn").forEach((b) => {
      b.addEventListener("click", () => {
        state.theme = b.dataset.th;
        localStorage.setItem(LS_THEME, state.theme);
        applyTheme();
        drawChart();
      });
    });

    $("btnClearHist").addEventListener("click", () => {
      state.history[state.mode] = [];
      localStorage.removeItem(state.mode === "real" ? LS_HIST_REAL : LS_HIST_SIM);
      drawChart();
    });

    $("btnResetBoard").addEventListener("click", async () => {
      if (state.mode !== "real") return;
      try { await fetch("/api/reset", { cache: "no-store" }); pollOnce(); } catch (_) {}
    });

    // Admin keypad
    $("keypad").addEventListener("click", (e) => {
      const btn = e.target.closest("button[data-k]");
      if (!btn || state.admin.lockUntil > Date.now()) return;
      const k = btn.dataset.k;
      if (k === "clr") state.admin.pin = "";
      else if (k === "del") state.admin.pin = state.admin.pin.slice(0, -1);
      else if (state.admin.pin.length < 4) state.admin.pin += k;
      renderPinDots();
      if (state.admin.pin.length === 4) submitPin();
    });

    $("btnLogout").addEventListener("click", () => {
      state.admin.token = "";
      state.admin.previewSlot = null;
      renderAdmin();
    });
    $("btnRefreshDevs").addEventListener("click", refreshDevices);

    // Device actions
    $("tab-admin").addEventListener("click", async (e) => {
      const actBtn = e.target.closest("button[data-act]");
      if (actBtn) {
        const act = actBtn.dataset.act, mac = actBtn.dataset.mac;
        if (state.mode === "sim") {
          if (act === "rem") state.admin.devices = state.admin.devices.filter((d) => d.mac !== mac);
          if (act === "blk") {
            state.admin.devices = state.admin.devices.filter((d) => d.mac !== mac);
            if (!state.admin.blocklist.includes(mac)) state.admin.blocklist.push(mac);
          }
          if (act === "unblk") state.admin.blocklist = state.admin.blocklist.filter((m) => m !== mac);
          renderAdminDevices();
          return;
        }
        const path = act === "rem" ? "/api/admin/remove" : (act === "blk" ? "/api/admin/block" : "/api/admin/unblock");
        try { await adminFetch(path + "?mac=" + encodeURIComponent(mac), { method: "POST" }); await refreshDevices(); } catch (_) {}
        return;
      }

      const sel = e.target.closest("button[data-sel-slot]");
      if (sel) {
        const slot = Number(sel.dataset.selSlot);
        state.admin.ledSlot = slot;
        state.admin.ledMode = "admin";
        if (state.mode === "real") {
          try { await adminFetch("/api/admin/led?mode=admin&slot=" + slot, { method: "POST" }); } catch (_) {}
        }
        renderAdminTemplates();
        return;
      }

      const prev = e.target.closest("button[data-prev-slot]");
      if (prev) {
        const slot = Number(prev.dataset.prevSlot);
        readTemplateInputs(slot);
        state.admin.previewSlot = (state.admin.previewSlot === slot) ? null : slot;
        renderAdminTemplates();
        return;
      }

      const save = e.target.closest("button[data-save-slot]");
      if (save) {
        const slot = Number(save.dataset.saveSlot);
        const tpl = readTemplateInputs(slot);
        if (state.mode === "real") {
          const body = "name=" + encodeURIComponent(tpl.name) + "&blinks=" + tpl.blinks + "&cycle=" + tpl.cycle + "&brightness=" + tpl.brightness;
          try {
            const res = await adminFetch("/api/admin/template?slot=" + slot, {
              method: "POST",
              headers: { "Content-Type": "application/x-www-form-urlencoded" },
              body
            });
            if (res && res.ok) {
              const j = await res.json();
              if (j.real_cycle) tpl.cycle = Number(j.real_cycle);
              $("adminMsg").textContent = "Saved slot " + slot + " (cycle " + tpl.cycle + "s)";
            }
          } catch (_) {}
        } else {
          $("adminMsg").textContent = "Saved slot " + slot + " (simulated)";
        }
        renderAdminTemplates();
      }
    });

    $("btnLedSys").addEventListener("click", async () => {
      state.admin.ledMode = "system";
      state.admin.previewSlot = null;
      if (state.mode === "real") {
        try { await adminFetch("/api/admin/led?mode=system", { method: "POST" }); } catch (_) {}
      }
      renderAdminTemplates();
    });
    $("btnLedAdm").addEventListener("click", async () => {
      state.admin.ledMode = "admin";
      if (state.mode === "real") {
        try { await adminFetch("/api/admin/led?mode=admin&slot=" + state.admin.ledSlot, { method: "POST" }); } catch (_) {}
      }
      renderAdminTemplates();
    });

    $("masterRange").addEventListener("input", () => {
      state.admin.master = Number($("masterRange").value);
      $("masterVal").textContent = state.admin.master + "%";
    });
    $("masterRange").addEventListener("change", async () => {
      if (state.mode === "real") {
        try { await adminFetch("/api/admin/master?value=" + state.admin.master, { method: "POST" }); } catch (_) {}
      }
    });

    document.addEventListener("visibilitychange", () => {
      if (!document.hidden) pollOnce();
    });
    window.addEventListener("resize", () => { if (state.tab === "live") drawChart(); });

    setInterval(() => {
      if (state.tab === "admin" && state.admin.token && !document.hidden) refreshDevices();
    }, 5000);
    setInterval(tickLedLamp, 40);
  }

  function readTemplateInputs(slot) {
    const tpl = state.admin.templates[slot - 1];
    const q = (f) => document.querySelector("input[data-slot='" + slot + "'][data-f='" + f + "']");
    const n = q("name"), b = q("blinks"), c = q("cycle"), br = q("brightness");
    if (n) tpl.name = n.value.slice(0, 16) || ("Slot " + slot);
    if (b) tpl.blinks = Math.max(1, Math.min(10, Math.round(Number(b.value) || 1)));
    if (c) tpl.cycle = Math.max(0.5, Math.min(10, Number(c.value) || 1));
    if (br) tpl.brightness = Math.max(0, Math.min(100, Math.round(Number(br.value) || 0)));
    return tpl;
  }

  // Boot
  loadStorage();
  applyTheme();
  bindEvents();
  renderAll();
  startPolling();
})();
</script>
</body>
</html>
)WEEH";
