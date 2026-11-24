
#pragma once
#include <Arduino.h>

const char kPanel[] PROGMEM = R"HTML(<!doctype html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#d7f4ff">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="default">
<meta name="apple-mobile-web-app-title" content="AIHome">
<link rel="manifest" href="/manifest.webmanifest">
<link rel="apple-touch-icon" href="/apple-touch-icon.png">
<title>AIHome · 客厅空调</title>
<script>
document.documentElement.dataset.room = location.pathname === '/bedroom' ? 'bedroom' : 'living';
</script>
<style>
:root{
  color-scheme:light;
  --bg:#eefaff;
  --panel:#f7fcff;
  --panel2:#eaf7fb;
  --line:#c5e1ec;
  --text:#163d51;
  --muted:#456779;
  --accent:#087eae;
  --accent-soft:#d5f2fa;
  --glow:#64c9e044;
  --warm:#b24f22;
  --fan-power:.58;
  --wind-speed:2.8s;
  font-family:-apple-system,BlinkMacSystemFont,"SF Pro Display","Segoe UI",sans-serif;
}
*{box-sizing:border-box}
html{min-height:100%;background:var(--bg)}
body{
  margin:0;min-height:100vh;color:var(--text);
  background:
    radial-gradient(ellipse at 90% 6%,#fff3db 0%,transparent 28%),
    radial-gradient(ellipse at 12% 34%,#c5eced 0%,transparent 47%),
    linear-gradient(160deg,#bee3f4 0%,#edf8fa 55%,#fafdfc 100%);
  font-size:14px;line-height:1.5;
}
button,a{-webkit-tap-highlight-color:transparent}
button{
  color:inherit;font:inherit;cursor:pointer;touch-action:manipulation;
  border:1px solid var(--line);background:linear-gradient(180deg,#ffffff,#f7fcff);
  border-radius:15px;min-height:48px;
  transition:transform .16s,border-color .2s,background .2s,box-shadow .2s,opacity .2s;
}
button:active{transform:scale(.97)}
button:focus-visible,a:focus-visible{outline:2px solid var(--accent);outline-offset:3px}
button:disabled{opacity:.38;cursor:default;transform:none}
button[aria-pressed="true"]{
  border-color:var(--accent);color:var(--accent);background:var(--accent-soft);
  box-shadow:0 0 20px var(--glow),inset 0 0 14px rgba(255,255,255,.025);
}
a{color:#176784;text-decoration:none}
main{
  width:min(100%,520px);margin:0 auto;
  padding:calc(20px + env(safe-area-inset-top)) 18px calc(30px + env(safe-area-inset-bottom));
}
.topbar{display:flex;align-items:center;justify-content:space-between;gap:12px}
.brand{display:flex;align-items:center;gap:11px;min-width:0}
.brand-mark{
  width:46px;height:46px;flex:0 0 46px;display:grid;place-items:center;
  border:1px solid #99ccde;border-radius:14px;background:#f4fdff;
  box-shadow:0 0 18px #2598e12b;
}
.brand-mark svg{display:block;width:39px;height:39px}
.brand-orbit{transform-box:view-box;transform-origin:50% 50%;animation:fanSpin 14s linear infinite}
h1{font-size:clamp(16px,4.6vw,22px);letter-spacing:-.35px;line-height:1.22;margin:0;font-weight:700;white-space:nowrap}
h1 span{color:var(--accent)}
.network{font-size:11px;color:var(--muted);margin-top:5px}
.network:before{content:"";display:inline-block;width:6px;height:6px;border-radius:50%;background:#71818d;margin:0 7px 1px 1px}
.online .network:before{background:#54e3a5;box-shadow:0 0 10px #54e3a5}
.wifi-link{font-size:11px;color:#176784;white-space:nowrap;padding:9px 11px;border:1px solid var(--line);border-radius:12px;background:#f7fcff}
.room-strip{
  display:flex;align-items:center;gap:10px;margin:20px 0 12px;padding:11px 13px;
  border:1px solid var(--line);border-radius:15px;background:#ffffffb3;
}
.room-dot{width:8px;height:8px;border-radius:50%;background:var(--accent);box-shadow:0 0 12px var(--glow);flex:0 0 auto}
.room-meta{display:flex;flex-direction:column;line-height:1.25}
.room-meta strong{font-size:13px;letter-spacing:.3px}
.room-meta small{font-size:10px;color:var(--muted);margin-top:4px}
.room-chip{margin-left:auto;font-size:8px;letter-spacing:1.3px;color:#476d80;border:1px solid var(--line);border-radius:7px;padding:5px 7px;white-space:nowrap}
.card{
  border:1px solid var(--line);border-radius:22px;
  background:linear-gradient(145deg,#fff,#f0faff);
  box-shadow:0 8px 25px #28739316;
}
.scene{
  --sky-top:#7eb9dc;
  --sky-low:#f7f6e8;
  --sea-top:#8ecad5;
  --sea-low:#aedbd9;
  --wave-color:#70b8c9;
  --sun:#fffdf1;
  --ambient:#ffffff00;
  --cloud-time:20s;
  --wave-time:10s;
  --palm-time:6s;
  position:relative;isolation:isolate;overflow:hidden;border-radius:22px;
  background:
    radial-gradient(circle at 78% 21%,var(--sun) 0 5%,transparent 25%),
    linear-gradient(169deg,var(--sky-top) 0%,var(--sky-low) 65%,var(--sea-top) 66%,var(--sea-low) 100%);
}
.running[data-mode="制冷"] .scene{
  --sky-top:#72b9e2;--sky-low:#e1f8ff;--sea-top:#68b8d2;--sea-low:#9cdeed;
  --wave-color:#66b2ce;--sun:#f1fcff;--ambient:#e2faffb3;
}
.running[data-mode="制热"] .scene{
  --sky-top:#ec9f73;--sky-low:#ffebd2;--sea-top:#c0a2a6;--sea-low:#f4c3a6;
  --wave-color:#b69ba2;--sun:#ffd69780;
}
.running[data-mode="除湿"] .scene{
  --sky-top:#969fba;--sky-low:#f0edf7;--sea-top:#95adc5;--sea-low:#b9d0d9;
  --wave-color:#87a9c0;--sun:#f5eff9;--ambient:#f4eef3ce;--cloud-time:30s;--wave-time:16s;
}
.running[data-mode="送风"] .scene{
  --sky-top:#69b5c5;--sky-low:#e5f7ed;--sea-top:#78c8bf;--sea-low:#b3e0cd;
  --wave-color:#6cb6aa;--sun:#effff4;--cloud-time:11s;--wave-time:7s;--palm-time:4s;
}
.running[data-mode="自动"] .scene{
  --sky-top:#859ed2;--sky-low:#f5f5f1;--sea-top:#85baca;--sea-low:#b3dad7;
  --wave-color:#79b0c4;--sun:#fff1d1;--ambient:#eee0f0ad;
}
.coast{position:absolute;inset:0;overflow:hidden;pointer-events:none}
.coast::after{
  content:"";position:absolute;inset:-8%;
  background:radial-gradient(ellipse at 68% 56%,var(--ambient),transparent 65%);
  opacity:0;pointer-events:none;
}
.running[data-mode="除湿"] .coast::after{animation:coastalMist 18s ease-in-out infinite alternate-reverse}
.running[data-mode="自动"] .coast::after{animation:coastalGlow 16s ease-in-out infinite alternate}
@keyframes coastalMist{from{opacity:.18;transform:translateX(-7%)}to{opacity:.64;transform:translateX(7%)}}
@keyframes coastalGlow{from{opacity:.15}to{opacity:.72}}
.coast-weather,.coast-sun{display:none;position:absolute}
.running[data-mode="制冷"] .coast-snow,
.running[data-mode="除湿"] .coast-drops,
.running[data-mode="制热"] .coast-sun{display:block}
.coast-weather{inset:0}
.coast-weather i{position:absolute;left:var(--x);animation-delay:var(--delay)}
.coast-snow i{
  top:4%;color:#fff;font-style:normal;font-size:var(--size,13px);line-height:1;
  text-shadow:0 1px 3px #446f9a70;opacity:0;
  animation:snowFall 9s linear infinite;animation-delay:var(--delay);
}
.coast-weather i:nth-child(1){--x:12%;--delay:-1s}
.coast-weather i:nth-child(2){--x:28%;--delay:-6s;--size:10px}
.coast-weather i:nth-child(3){--x:46%;--delay:-3s}
.coast-weather i:nth-child(4){--x:66%;--delay:-7.5s;--size:10px}
.coast-weather i:nth-child(5){--x:83%;--delay:-4.5s}
.coast-weather i:nth-child(6){--x:93%;--delay:-2s;--size:9px}
.coast-drops i{
  top:25%;width:8px;height:8px;border-radius:65% 0 65% 65%;
  background:linear-gradient(135deg,#f8fdff,#4d9ebf);
  border:1px solid #5787aa60;opacity:0;
  animation:dryDrop 6.5s ease-in infinite;animation-delay:var(--delay);
}
.coast-drops i:nth-child(2){--x:76%;--delay:-3.5s}
.coast-drops i:nth-child(3){--x:89%;--delay:-5.5s}
.coast-sun{
  top:15%;right:11%;width:31px;aspect-ratio:1;border-radius:50%;
  background:radial-gradient(circle at 38% 35%,#fffde3,#ffd36d 82%);
  box-shadow:0 0 0 5px #fff3b85c,0 0 24px 9px #ffd77988;
  animation:sunGlow 7s ease-in-out infinite alternate;
}
@keyframes snowFall{
  0%{opacity:0;transform:translate(-5px,-12px) rotate(-10deg)}
  18%,78%{opacity:.95}
  100%{opacity:0;transform:translate(9px,225px) rotate(30deg)}
}
@keyframes dryDrop{
  0%{opacity:0;transform:translateY(0) rotate(-45deg) scale(.7)}
  24%{opacity:.86;transform:translateY(22px) rotate(-45deg) scale(1)}
  75%,100%{opacity:0;transform:translateY(100px) rotate(-45deg) scale(.15)}
}
@keyframes sunGlow{from{opacity:.74;transform:scale(.94)}to{opacity:1;transform:scale(1.06)}}
.cloud{
  position:absolute;width:66%;height:13%;border-radius:50%;
  background:
    radial-gradient(ellipse at 35% 65%,#ffffffd9 0%,transparent 58%),
    radial-gradient(ellipse at 63% 51%,#fcffffba 0%,transparent 61%);
  filter:blur(1px);animation:cloudDrift var(--cloud-time) ease-in-out infinite alternate;
}
.cloud.one{top:17%;left:-4%}
.cloud.two{top:31%;right:-9%;transform:scale(.84,.62);opacity:.75;animation-delay:-7s}
.wave{
  position:absolute;left:-25%;width:150%;height:32%;border-radius:49% 51% 0 0 / 17% 23% 0 0;
  animation:tide var(--wave-time) ease-in-out infinite alternate;
}
.wave.back{bottom:-5%;background:linear-gradient(170deg,var(--sea-top),var(--wave-color) 52%,var(--wave-color))}
.wave.front{
  bottom:-14%;background:linear-gradient(176deg,#eef9e9 0%,#d7efde 14%,#f5ebd0 38%,#efe2c3);
  border-top:2px solid #f9fffad9;box-shadow:0 -3px 8px #eefdf77a;animation-delay:-4s;
}
.palm{position:absolute;z-index:1;right:5%;bottom:20%;width:42px;height:89px;transform-origin:bottom;animation:palmSway var(--palm-time) ease-in-out infinite alternate}
.palm::before{
  content:"";position:absolute;bottom:0;right:10px;width:25px;height:67px;
  border-right:5px solid #756558;border-radius:0 80% 0 0;transform:rotate(-11deg);
}
.palm i{
  position:absolute;right:12px;top:10px;width:31px;height:13px;background:linear-gradient(135deg,#4b987a,#226953);
  border-radius:100% 0 100% 0;transform-origin:right center;transform:rotate(var(--leaf-angle));
}
.palm i:nth-child(1){--leaf-angle:6deg}
.palm i:nth-child(2){--leaf-angle:44deg;background:linear-gradient(130deg,#73a689,#367459)}
.palm i:nth-child(3){--leaf-angle:94deg}
.palm i:nth-child(4){--leaf-angle:144deg;background:linear-gradient(130deg,#73a689,#367459)}
.palm i:nth-child(5){--leaf-angle:184deg}
@keyframes cloudDrift{from{translate:-5px}to{translate:9px}}
@keyframes tide{from{transform:translate(-5%,0)}to{transform:translate(5%,-5%)}}
@keyframes palmSway{from{transform:rotate(-3deg)}to{transform:rotate(3deg)}}

.cabinet{position:absolute;z-index:2;top:calc(50% + 8px);left:50%;width:min(98.9%,195.5px);transform:translate(-50%,-50%)}
.cabinet svg{display:block;width:100%;height:auto;overflow:visible}
.scene-top{position:absolute;top:14px;left:10px;right:10px;display:flex;justify-content:space-between;align-items:center;z-index:5}
.scene-power{padding:2px 7px;border-radius:999px;background:#d7e9ef;color:#365b6a;font-size:10px;white-space:nowrap}
.scene-power.on{background:#d6f5e4;color:#126243}
.scene-status{font-size:8px;color:#185a48;display:flex;align-items:center;gap:5px;white-space:nowrap}
.scene-status:before{content:"";width:5px;height:5px;border-radius:50%;background:#279775;box-shadow:0 0 9px #279775}
.scene-status.offline{color:#965117}
.scene-status.offline:before{background:#965117;box-shadow:0 0 9px #965117}
.room-glow{position:absolute;width:230px;height:230px;left:50%;top:42%;transform:translate(-50%,-50%);border-radius:50%;background:radial-gradient(circle,var(--glow),transparent 68%);opacity:.45;transition:background .4s,opacity .4s}
.running .room-glow{opacity:.95}
.swing #cabinet-grille{transform-box:fill-box;transform-origin:center;animation:grilleSwing 2.6s ease-in-out infinite alternate}
.airflow{visibility:hidden;pointer-events:none}
.running .airflow{visibility:visible;opacity:var(--fan-power)}
.swing-x .airflow{transform-origin:110px 112px;animation:windSway 3s ease-in-out infinite alternate}
.air-guide,.air-stream{fill:none;stroke:var(--accent);stroke-linecap:round}
.air-guide{stroke-width:2.5;opacity:.32}
.air-stream{stroke-width:4;stroke-dasharray:18 82;filter:drop-shadow(0 0 5px var(--accent))}
.running .air-stream{animation:airStream var(--wind-speed) linear infinite}
.air-stream:nth-child(2){animation-delay:-.7s}
.air-stream:nth-child(3){animation-delay:-1.4s}
body[data-mode="除湿"] .air-stream{stroke-dasharray:3 24}
body[data-mode="制热"] .air-stream{stroke-width:6}
body[data-mode="送风"] .air-stream{stroke-dasharray:28 72}
.cabinet-lcd text{font-variant-numeric:tabular-nums;letter-spacing:.25px}
.cabinet-lcd .lcd-detail{font-size:13px;font-weight:600;fill:#adf4e8}
.section-head{display:flex;align-items:center;justify-content:space-between;gap:10px;margin:16px 2px 8px}
.section-head strong{font-size:14px;letter-spacing:.4px}
.section-head span{font-size:10px;color:var(--muted)}
.hero-layout{
  display:grid;grid-template-columns:repeat(2,minmax(0,1fr));
  gap:10px;align-items:stretch;margin-top:16px;
}
.hero-layout > *{min-width:0;min-height:366px}
.temperature-card{
  padding:12px 8px 11px;text-align:center;
  display:flex;flex-direction:column;justify-content:center;
}
.temp-head{display:flex;align-items:center;justify-content:space-between;gap:4px;margin:0 3px 4px;text-align:left}
.temp-head strong{font-size:12px;letter-spacing:.2px;white-space:nowrap}
.dial-wrap{width:min(100%,230px);aspect-ratio:1;margin:0 auto;position:relative;touch-action:none;user-select:none;-webkit-user-select:none}
#tempDial{width:100%;height:100%;display:block;overflow:visible;touch-action:none;cursor:grab}
#tempDial.dragging{cursor:grabbing}
#tempDial[aria-disabled="true"]{pointer-events:none;filter:grayscale(1);opacity:.55}
main button:disabled{filter:grayscale(1);box-shadow:none}
.dial-track{fill:none;stroke:#d7e9ef;stroke-width:36;stroke-linecap:round}
.dial-progress{fill:none;stroke:var(--accent);stroke-width:36;stroke-linecap:round;filter:drop-shadow(0 0 4px var(--glow));stroke-dasharray:0 1000}
.dial-ticks line{stroke:#759baa;stroke-width:1.5;opacity:.65}
.dial-ticks line.major{stroke:#527f95;stroke-width:2.2;opacity:.9}
.dial-thumb{fill:#f4fcff;stroke:var(--accent);stroke-width:5;filter:drop-shadow(0 0 8px var(--accent))}
.dial-thumb-core{fill:var(--accent)}
.dial-label-svg{fill:#456779;font-size:10px;letter-spacing:1px}
.dial-center{
  position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;justify-content:center;
  pointer-events:none;padding-top:3px;
}
.dial-caption{font-size:8px;color:#466b7a;letter-spacing:1.8px}
#temp{font-size:clamp(29px,8vw,46px);line-height:1.08;font-weight:250;letter-spacing:-2px;font-variant-numeric:tabular-nums;color:#163d51;text-shadow:0 0 25px var(--glow);margin:5px 0 0}
#temp small{font-size:12px;letter-spacing:0;color:var(--accent);margin-left:3px}
#power{font-size:10px;letter-spacing:1.8px;color:var(--accent);margin-top:7px}
.dial-hint{font-size:8px;color:#4c6e7d;margin-top:3px}
.dial-steps{display:flex;justify-content:center;gap:10px;margin-top:-1px;position:relative;z-index:2}
.dial-steps button{width:30px;min-height:29px;height:29px;border-radius:10px;font-size:18px;line-height:1;color:var(--accent);padding:0}
.power-row{display:flex;justify-content:center;align-items:center;gap:8px;margin-top:7px}
.power-button{
  width:46px;height:42px;min-height:42px;border-radius:14px;font-size:20px;color:var(--accent);
  border-color:rgba(84,207,255,.48);box-shadow:0 0 20px var(--glow);
}
.running .power-button{background:var(--accent-soft);box-shadow:0 0 25px var(--glow),inset 0 0 15px var(--accent-soft)}
.power-caption{font-size:9px;color:#456779}
#message{
  position:fixed;z-index:10;left:50%;bottom:calc(12px + env(safe-area-inset-bottom));
  transform:translateX(-50%);width:max-content;max-width:calc(100% - 32px);
  margin:0;padding:8px 12px;border:1px solid var(--line);border-radius:10px;
  background:#f7fcff;color:#163d51;font-size:12px;text-align:center;pointer-events:none;
}
#message:empty{display:none}
.mode-grid{
  display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:8px;padding:12px 6px 10px;
  border-radius:24px;
  background:
    radial-gradient(ellipse at 21% 91%,#72c9dcb8,transparent 55%),
    radial-gradient(ellipse at 76% 9%,#fff1d6,transparent 56%),
    linear-gradient(140deg,#cae9f5,#dbece8 78%,#c0e4ee);
}
.mode-grid button{
  position:relative;isolation:isolate;overflow:hidden;min-height:83px;padding:8px 0;
  border-radius:999px;border:1px solid #ffffffd9;font-size:11px;
  color:#204b60;background:linear-gradient(153deg,#ffffffbd,#ffffff45 52%,#d5f2fa85);
  -webkit-backdrop-filter:blur(13px) saturate(150%);
  backdrop-filter:blur(13px) saturate(150%);
  box-shadow:inset 1px 1px 1px #fff,inset -1px -2px 4px #4c93ab32,0 4px 12px #2c667a24;
  transition:transform .25s,box-shadow .25s,border-color .25s;
}
.mode-grid button::before{
  content:"";position:absolute;z-index:-1;top:3px;left:8%;width:84%;height:35%;
  border-radius:60% 60% 50% 50%;background:linear-gradient(175deg,#fffe,transparent);
  pointer-events:none;
}
.mode-grid .icon{display:block;font-size:23px;line-height:1.35;margin-bottom:4px;color:var(--mode-color)}
.mode-grid button[aria-pressed="true"]{
  transform:translateY(-4px);border-color:var(--mode-color);
  background:linear-gradient(153deg,#fffd,#fff4 53%,var(--mode-tint));
  box-shadow:inset 1px 2px 2px #fff,inset -1px -3px 6px var(--mode-tint),0 6px 17px var(--mode-tint);
}
.mode-grid button[aria-pressed="true"]::before{animation:glassLight 4s ease-in-out infinite alternate}
.mode-grid button:disabled{opacity:.46;box-shadow:inset 0 1px 1px #fff;transform:none}
.mode-grid button[data-key="auto"]{--mode-color:#8b6519;--mode-tint:#e6bd687d}
.mode-grid button[data-key="cool"]{--mode-color:#087eae;--mode-tint:#39b2db80}
.mode-grid button[data-key="dry"]{--mode-color:#7754a8;--mode-tint:#a982d380}
.mode-grid button[data-key="fanMode"]{--mode-color:#137f71;--mode-tint:#54bea880}
.mode-grid button[data-key="heat"]{--mode-color:#ad4b27;--mode-tint:#ef986680}
@keyframes glassLight{from{transform:translate(-10%,0)}to{transform:translate(9%,5px)}}

.air-card{padding:14px 15px}
.air-top{display:flex;justify-content:space-between;align-items:center}
.air-top strong{font-size:12px}
.air-top span{font-size:10px;color:var(--accent)}
.air-visual{display:flex;align-items:center;gap:13px;margin-top:13px}
.fan-orb{
  flex:0 0 58px;width:58px;height:58px;border-radius:50%;display:grid;place-items:center;
  border:1px solid rgba(111,192,219,.28);background:radial-gradient(circle at 35% 30%,#f9feff,#d6f3fc 72%);
  box-shadow:inset 0 0 18px #4b99ad24,0 0 18px rgba(73,190,225,.08);
}
.fan-symbol{font-size:27px;color:var(--accent);text-shadow:0 0 14px var(--glow);animation:fanSpin 4s linear infinite;animation-play-state:paused}
.running .fan-symbol{animation-play-state:running}
.fan-info{flex:1;min-width:0}
.fan-info-head{display:flex;justify-content:space-between;align-items:center;gap:8px;font-size:10px;color:#456779}
.fan-bars{display:flex;gap:5px;margin:9px 0 5px}
.fan-bars i{height:7px;flex:1;border-radius:5px;background:#d7e9ef;border:1px solid #bbd8e4;transition:background .2s,box-shadow .2s}
.fan-bars i.on{background:var(--accent);border-color:var(--accent);box-shadow:0 0 9px var(--glow)}
.fan-scale{display:flex;justify-content:space-between;font-size:8px;color:#456779}
.fan-change{padding:0 12px;min-height:38px;border-radius:12px;font-size:10px;white-space:nowrap}
.feature-grid{
  display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:9px;padding:9px;
  border:1px solid #badbe5;border-radius:21px;
  background:radial-gradient(ellipse at 100% 100%,#f2e6cbad,transparent 66%),
             linear-gradient(145deg,#cbe7f0,#dfefec);
}
.feature-grid button{
  --feature-tint:#d2eaf5;
  --feature-ink:#215d79;
  min-height:72px;padding:8px 3px;font-size:10px;font-weight:600;
  border-color:#ffffffad;color:#204659;
  background:linear-gradient(155deg,#f8fcff,var(--feature-tint));
  box-shadow:inset 0 1px 1px #fff,0 2px 5px #36596616;
}
.feature-grid button:is([data-key="quiet"],[data-key="economy"],[data-key="sleep"]){
  --feature-tint:#d6eee2;--feature-ink:#2e6956;
}
.feature-grid button:is([data-key="timer"],[data-key="light"],[data-key="dryAux"]){
  --feature-tint:#f3e6c9;--feature-ink:#775521;
}
.feature-grid .icon{display:block;font-size:20px;line-height:1.4;margin-bottom:3px;color:var(--feature-ink)}
.feature-grid button[aria-pressed="true"]{
  border-color:var(--feature-ink);color:var(--feature-ink);
  background:var(--feature-tint);box-shadow:inset 0 0 0 1px #ffffffb3;
}
.feature-grid button:disabled{
  opacity:1;filter:none;box-shadow:none;color:#5f6c72;border-color:#c7d5da;
  background:linear-gradient(150deg,#f0f4f4,#e1e8e9);
}
.feature-grid button:disabled .icon{color:#73848b}
@keyframes airStream{from{stroke-dashoffset:18}to{stroke-dashoffset:-100}}
@keyframes fanSpin{to{transform:rotate(360deg)}}
@keyframes grilleSwing{from{transform:skewY(-2deg) scaleY(.98)}to{transform:skewY(2deg) scaleY(.88)}}
@keyframes windSway{from{transform:skewY(-3deg)}to{transform:skewY(3deg)}}
@media(prefers-reduced-motion:reduce){*,*:before,*:after{animation:none!important;transition:none!important}}
@media(min-width:700px){
  body{padding:20px 0}
  main{border:1px solid rgba(84,207,255,.2);border-radius:28px;background:#f3fbffab;box-shadow:0 20px 60px #4b99ad24;padding:24px 22px 30px}
}
@media(max-width:360px){
  main{padding-left:13px;padding-right:13px}
  .hero-layout{gap:7px}
  .hero-layout > *{min-height:320px}
  .temperature-card{padding:9px 5px}
  .temp-head{margin-left:1px;margin-right:1px}
  .temp-head strong{font-size:11px}
  .dial-wrap{width:100%}
  #temp{font-size:clamp(27px,8vw,34px)}
  .feature-grid{gap:7px}
}

html[data-room="bedroom"] .hero-layout{grid-template-columns:minmax(0,1fr);gap:10px;margin-top:12px}
html[data-room="bedroom"] .hero-layout > .scene{min-height:0;height:clamp(185px,54vw,237px)}
html[data-room="bedroom"] .scene .palm{right:5%;bottom:9%;transform:scale(.9)}
html[data-room="bedroom"] .scene .cloud.one{top:17%;left:4%;width:40%}
html[data-room="bedroom"] .scene .cloud.two{top:30%;right:3%;width:36%}
html[data-room="bedroom"] .wall-unit{position:absolute;z-index:2;width:min(90%,395px);top:9%;left:50%;transform:translateX(-50%) scaleY(1.12);transform-origin:center top}
html[data-room="bedroom"] .wall-unit svg{width:100%;height:auto;display:block;filter:drop-shadow(0 8px 10px #23455636)}
html[data-room="bedroom"] .wall-led{fill:#8fa5ac}
html[data-room="bedroom"] .running .wall-led{fill:#36a6d7;filter:drop-shadow(0 0 4px #53beff)}
html[data-room="bedroom"] .wall-unit .airflow{transform-origin:190px 104px}
html[data-room="bedroom"] .hero-layout > .temperature-card{
  min-height:0;display:grid;grid-template-columns:minmax(0,1.15fr) minmax(0,.85fr);
  grid-template-areas:"dial label" "dial steps" "dial power";align-content:center;
  grid-template-rows:1fr auto 1fr;align-items:center;column-gap:12px;padding:12px 15px;
}
html[data-room="bedroom"] .temperature-card .dial-wrap{grid-area:dial;width:min(100%,186px)}
html[data-room="bedroom"] .temp-head{grid-area:label;align-self:end;justify-content:center;margin:0 0 9px}
html[data-room="bedroom"] .dial-steps{grid-area:steps;margin:0}
html[data-room="bedroom"] .dial-steps button{width:42px;height:42px;min-height:42px}
html[data-room="bedroom"] .power-row{grid-area:power;align-self:start;flex-direction:column;gap:4px;margin-top:10px}
html[data-room="bedroom"] .power-caption{font-size:10px}
html[data-room="bedroom"] .feature-grid button{min-height:66px}

html[data-room="living"] .bedroom-only,
html[data-room="bedroom"] .living-only{display:none}
.wall-lcd{font-variant-numeric:tabular-nums;font-weight:600}
.wall-flap{
  transform-box:fill-box;transform-origin:center top;transform:rotateX(0deg);
  transition:transform .6s ease;
}
.running .wall-flap{transform:rotateX(-55deg)}
.running.swing .wall-flap{animation:wallFlap 3s ease-in-out infinite alternate}
@keyframes wallFlap{from{transform:rotateX(-30deg)}to{transform:rotateX(-75deg)}}
html[data-room="bedroom"] .swing-x .wall-unit .airflow{animation:none}
@media(max-width:360px){
  html[data-room="bedroom"] .hero-layout > .temperature-card{padding:10px 12px;column-gap:8px}
  html[data-room="bedroom"] .dial-steps button{width:38px}
}
</style>
</head>
<body data-mode="制冷">
<main>
  <header class="topbar">
    <div class="brand">
      <div class="brand-mark" aria-hidden="true">
        <svg viewBox="0 0 64 64" fill="none" stroke="url(#brand-gradient)" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
          <defs>
            <linearGradient id="brand-gradient" x2="1" y2="1">
              <stop stop-color="#069d9a"/><stop offset="1" stop-color="#1763bb"/>
            </linearGradient>
          </defs>
          <g class="brand-orbit">
            <path d="M18 8a28 28 0 0 1 34 44M45 57A28 28 0 0 1 11 13"/>
            <path d="M11 43A24 24 0 0 1 49 15M54 22a24 24 0 0 1-39 27"/>
            <circle cx="51" cy="53" r="2" fill="#f4fdff"/><circle cx="12" cy="12" r="2" fill="#f4fdff"/>
          </g>
          <path d="M13 28 32 17l19 11M17 27v18h8m14 0h8V27"/>
          <rect x="25" y="30" width="14" height="14" rx="2"/>
          <path d="M28 30v-4m4 4v-4m4 4v-4m-8 18v4m4-4v4m4-4v4m-11-14h-4m4 3h-4m4 3h-4m18-6h4m-4 3h4m-4 3h4"/>
        </svg>
      </div>
      <div>
        <h1>李工‘s&nbsp;&nbsp;<span>AIHome</span></h1>
        <div id="network" class="network">正在连接控制版…</div>
      </div>
    </div>
    <a id="room-link" class="wifi-link" href="/bedroom">卧室</a>
  </header>

  <div class="room-strip">
    <span class="room-dot"></span>
    <div class="room-meta"><strong id="room-name">客厅</strong><small id="room-device">格力立式柜机</small></div>
  </div>

  <div class="hero-layout">
  <section class="scene" aria-label="空调和送风动画">
    <div class="scene-top">
      <span class="scene-power" id="scene-power" title="控制器电源设定">—</span>
      <span class="scene-status" id="scene-status">等待连接</span>
    </div>
    <div class="coast" aria-hidden="true">
      <i class="cloud one"></i><i class="cloud two"></i>
      <i class="wave back"></i><i class="wave front"></i>
      <span class="palm"><i></i><i></i><i></i><i></i><i></i></span>
      <span class="coast-weather coast-snow"><i>❄</i><i>❄</i><i>❄</i><i>❄</i><i>❄</i><i>❄</i></span>
      <span class="coast-weather coast-drops"><i></i><i></i><i></i></span>
      <i class="coast-sun"></i>
    </div>
    <div class="room-glow"></div>
    <div class="cabinet living-only">
      <svg viewBox="0 0 250 430" role="img" aria-label="白色格力立式空调柜机示意图">
        <defs>
          <linearGradient id="frontShell" x1="0" x2="1" y1="0" y2="0">
            <stop offset="0" stop-color="#b9c2c7"/>
            <stop offset=".12" stop-color="#f7f7f2"/>
            <stop offset=".7" stop-color="#fffefa"/>
            <stop offset="1" stop-color="#d8dedf"/>
          </linearGradient>
          <linearGradient id="sideShell" x1="0" x2="1">
            <stop offset="0" stop-color="#aeb8bd"/>
            <stop offset=".35" stop-color="#d7dddf"/>
            <stop offset="1" stop-color="#7d898f"/>
          </linearGradient>
          <linearGradient id="topShine" x1="0" x2="0" y1="0" y2="1">
            <stop offset="0" stop-color="#fff"/>
            <stop offset="1" stop-color="#e4e8e6"/>
          </linearGradient>
          <linearGradient id="blackPanel" x1="0" x2="0" y1="0" y2="1">
            <stop offset="0" stop-color="#20252b"/>
            <stop offset=".5" stop-color="#080b10"/>
            <stop offset="1" stop-color="#171c22"/>
          </linearGradient>
          <linearGradient id="displayGlow" x1="0" x2="1">
            <stop offset="0" stop-color="#172a35"/>
            <stop offset="1" stop-color="#050a10"/>
          </linearGradient>
        </defs>
        <ellipse cx="126" cy="414" rx="96" ry="12" fill="#000" opacity=".36"/>
        <path d="M184 24 Q205 20 213 39 L225 385 Q225 401 208 407 L185 401 Z" fill="url(#sideShell)" stroke="#8d999e" stroke-width="1"/>
        <path d="M48 32 Q48 12 68 10 L178 10 Q193 12 195 31 L195 389 Q194 405 178 409 L64 405 Q48 402 47 386 Z" fill="url(#frontShell)" stroke="#c6cece" stroke-width="1.4"/>
        <path d="M57 28 Q58 17 71 17 L174 17 Q184 18 186 31 L186 383 Q185 393 174 396 L67 393 Q58 391 57 381 Z" fill="url(#topShine)" opacity=".5"/>
        <path d="M62 36 Q62 25 75 24 L170 24 Q181 25 182 38 L182 158 L62 158 Z" fill="#f4f4ef" opacity=".42"/>
        <path d="M65 193 L183 193 L183 379 Q182 391 170 391 L72 388 Q65 386 65 376 Z" fill="#f6f6f0" opacity=".55"/>
        <g class="cabinet-lcd" text-anchor="middle">
          <rect x="63" y="189" width="122" height="75" rx="10" fill="url(#blackPanel)" stroke="#30363b"/>
          <rect x="68" y="194" width="112" height="65" rx="7" fill="url(#displayGlow)" stroke="#457a84" stroke-opacity=".45"/>
          <text id="unit-temp-svg" x="124" y="220" font-size="24" font-weight="700" fill="#a9fff3">—</text>
          <path d="M77 227H171" stroke="#46727d" stroke-opacity=".65"/>
          <text id="lcd-mode" class="lcd-detail" x="124" y="241"></text>
          <text id="lcd-fan" class="lcd-detail" x="124" y="255"></text>
        </g>
        <path d="M72 30 L72 150" stroke="#fff" stroke-width="2" opacity=".5"/>
        <path d="M51 385 Q123 397 195 389 L215 391 Q215 407 200 412 L63 411 Q48 407 48 393 Z" fill="#c6ccca" stroke="#929d9f" stroke-width="1"/>
        <path d="M62 397 Q124 405 196 397" stroke="#fff" stroke-width="2" opacity=".5" fill="none"/>
        <rect x="72" y="78" width="104" height="72" rx="7" fill="url(#blackPanel)"/>
        <g id="cabinet-grille" stroke="#77858b" stroke-width="2" opacity=".8">
          <path d="M79 88 H169 M79 98 H169 M79 108 H169 M79 118 H169 M79 128 H169 M79 138 H169"/>
        </g>
        <g class="airflow" aria-hidden="true">
          <g class="air-guide">
            <path d="M107 92 Q187 88 280 53"/>
            <path d="M113 111 Q201 107 293 102"/>
            <path d="M115 130 Q203 135 278 154"/>
          </g>
          <g>
            <path class="air-stream" pathLength="100" d="M107 92 Q187 88 280 53"/>
            <path class="air-stream" pathLength="100" d="M113 111 Q201 107 293 102"/>
            <path class="air-stream" pathLength="100" d="M115 130 Q203 135 278 154"/>
          </g>
        </g>
        <text x="78" y="58" font-size="7" letter-spacing="2" fill="#d2d6d2">GREE</text>
      </svg>
    </div>
    <div class="wall-unit bedroom-only">
      <svg viewBox="0 0 390 200" role="img" aria-label="参照卧室照片绘制的白色格力壁挂空调">
        <defs>
          <linearGradient id="wallShell" x2=".18" y2="1">
            <stop stop-color="#fff"/><stop offset=".63" stop-color="#f4f6f5"/><stop offset="1" stop-color="#d7e1e4"/>
          </linearGradient>
          <linearGradient id="wallSide" x2="1" y2="1">
            <stop stop-color="#e5eeef"/><stop offset="1" stop-color="#a1b3ba"/>
          </linearGradient>
          <linearGradient id="wallVent" x2="0" y2="1">
            <stop stop-color="#344b56"/><stop offset="1" stop-color="#152831"/>
          </linearGradient>
        </defs>
        <path d="M330 86h35q10 0 10 12v12" fill="none" stroke="#afc1c5" stroke-width="7" stroke-linecap="round"/>
        <path d="M320 31h21q14 0 16 14v45q0 11-14 16l-19 4Z" fill="url(#wallSide)"/>
        <path d="M42 28h287q17 0 19 16l-3 47q-1 17-20 19H55q-18-1-21-16l-7-49q-1-17 15-17Z"
              fill="url(#wallShell)" stroke="#c4d2d8"/>
        <path d="M35 44q1-9 12-9h278q9 0 13 5" fill="none" stroke="#fff" stroke-width="2" opacity=".9"/>
        <path d="M39 85q145 5 300 0" fill="none" stroke="#becbd1" stroke-width="1"/>
        <text x="183" y="78" text-anchor="middle" font-size="7" letter-spacing="1.6" fill="#6b7d85">GREE</text>
        <circle class="wall-led" cx="308" cy="49" r="3"/>
        <g class="wall-lcd" text-anchor="middle">
          <rect x="220" y="44" width="79" height="39" rx="6" fill="#19343e"/>
          <text id="wall-temp" x="259.5" y="62" font-size="17" fill="#acfff0">—</text>
          <text id="wall-mode" x="242" y="76" font-size="9" fill="#acfff0"></text>
          <text id="wall-fan" x="276" y="76" font-size="9" fill="#acfff0"></text>
        </g>
        <path d="M46 91h287l-6 12H55Z" fill="url(#wallVent)"/>
        <path class="wall-flap" d="M50 94h280l-3 7H55Z" fill="#dce5e7" stroke="#b1c0c5"/>
        <g class="airflow" aria-hidden="true">
          <g class="air-guide">
            <path d="M106 104Q93 142 70 179"/>
            <path d="M190 105Q187 146 188 186"/>
            <path d="M276 104Q289 142 313 179"/>
          </g>
          <g>
            <path class="air-stream" pathLength="100" d="M106 104Q93 142 70 179"/>
            <path class="air-stream" pathLength="100" d="M190 105Q187 146 188 186"/>
            <path class="air-stream" pathLength="100" d="M276 104Q289 142 313 179"/>
          </g>
        </g>
      </svg>
    </div>
  </section>

    <section class="card temperature-card" aria-label="触控温度控制">
      <div class="temp-head"><strong>温度设定</strong></div>
      <div class="dial-wrap">
        <svg id="tempDial" viewBox="0 0 260 260" role="slider" aria-label="目标温度" aria-valuemin="16" aria-valuemax="30" aria-valuenow="24" aria-disabled="true" tabindex="-1">
          <path class="dial-track" d="M55.046681 204.953319 A106 106 0 1 1 204.953319 204.953319" pathLength="1000"/>
          <path id="dialProgress" class="dial-progress" d="M55.046681 204.953319 A106 106 0 1 1 204.953319 204.953319" pathLength="1000"/>
          <g id="dialTicks" class="dial-ticks"></g>
          <circle id="dialThumb" class="dial-thumb" cx="130" cy="24" r="13"/>
          <circle id="dialThumbCore" class="dial-thumb-core" cx="130" cy="24" r="4"/>
          <text class="dial-label-svg" x="26" y="239">16°</text>
          <text class="dial-label-svg" x="204" y="239">30°</text>
        </svg>
        <div class="dial-center">
          <span class="dial-caption">TARGET TEMP</span>
          <strong id="temp">24<small>°C</small></strong>
          <span id="power">等待连接</span>
          <span class="dial-hint" id="dial-hint"></span>
        </div>
      </div>
      <div class="dial-steps">
        <button type="button" data-temp-step="-1" aria-label="降低一度" disabled>−</button>
        <button type="button" data-temp-step="1" aria-label="升高一度" disabled>＋</button>
      </div>
      <div class="power-row">
        <button class="power-button" data-key="power" aria-label="开关空调" aria-pressed="false" disabled>⏻</button>
        <span class="power-caption" id="power-caption">空调电源</span>
      </div>
    </section>
  </div>

  <div class="section-head"><strong>运行模式</strong><span>切换空调工作状态</span></div>
  <section class="mode-grid" aria-label="运行模式">
    <button data-key="auto" data-mode-label="自动" aria-pressed="false" disabled><span class="icon" aria-hidden="true">✧</span>自动</button>
    <button data-key="cool" data-mode-label="制冷" aria-pressed="false" disabled><span class="icon" aria-hidden="true">❄</span>制冷</button>
    <button data-key="dry" data-mode-label="除湿" aria-pressed="false" disabled><span class="icon" aria-hidden="true">♢</span>除湿</button>
    <button data-key="fanMode" data-mode-label="送风" aria-pressed="false" disabled><span class="icon" aria-hidden="true">≋</span>送风</button>
    <button data-key="heat" data-mode-label="制热" aria-pressed="false" disabled><span class="icon" aria-hidden="true">☀</span>制热</button>
  </section>

  <div class="section-head"><strong>风量与送风</strong></div>
  <section class="card air-card">
    <div class="air-top"><strong>当前风量</strong><span id="fan-value">自动风</span></div>
    <div class="air-visual">
      <div class="fan-orb"><span class="fan-symbol">✣</span></div>
      <div class="fan-info">
        <div class="fan-info-head"><span>风速档位</span><span id="fan-desc">自动调节</span></div>
        <div class="fan-bars" id="fan-bars"><i></i><i></i><i></i><i></i><i></i></div>
        <div class="fan-scale"><span>AUTO</span><span>LOW</span><span>MED</span><span>HIGH</span><span>TURBO</span></div>
      </div>
      <button class="fan-change" data-key="fan" disabled>切换风速</button>
    </div>
  </section>

  <div class="section-head"><strong>舒适设置</strong><span>轻触开启 / 关闭</span></div>
  <section class="feature-grid" aria-label="舒适设置">
    <button data-key="vertical" aria-pressed="false" disabled><span class="icon">↕</span>上下扫风</button>
    <button data-key="horizontal" aria-pressed="false" disabled><span class="icon">↔</span>左右扫风</button>
    <button data-key="quiet" aria-pressed="false" disabled><span class="icon">☾</span>静音</button>
    <button data-key="turbo" aria-pressed="false" disabled><span class="icon">»</span>强劲</button>
    <button data-key="economy" aria-pressed="false" disabled><span class="icon">♧</span>节能</button>
    <button data-key="sleep" aria-pressed="false" disabled><span class="icon">☽</span>睡眠</button>
    <button data-key="timer" aria-pressed="false" disabled><span class="icon">◷</span>定时 30 分</button>
    <button data-key="light" aria-pressed="false" disabled><span class="icon">☼</span>灯光</button>
    <button data-key="dryAux" aria-pressed="false" disabled><span class="icon">◈</span>干燥／辅热</button>
  </section>

  <p id="message" role="status" aria-live="polite"></p>
</main>

<script>
'use strict';
const el = id => document.getElementById(id);
const isBedroom = document.documentElement.dataset.room === 'bedroom';
const acEndpoint = isBedroom ? '/api/bedroom/ac' : '/api/ac';
if (isBedroom) {
  document.title = 'AIHome · 卧室 1';
  el('room-name').textContent = '卧室 1';
  el('room-device').textContent = '格力壁挂空调';
  el('room-link').textContent = '客厅';
  el('room-link').href = '/';
  const features = document.querySelector('.feature-grid');
  for (const key of ['timer', 'quiet', 'sleep']) {
    features.prepend(features.querySelector('[data-key="' + key + '"]'));
  }
}
const buttons = Array.from(document.querySelectorAll('[data-key]'));
const stepButtons = Array.from(document.querySelectorAll('[data-temp-step]'));
const errors = {
  power_off: '请先开机，或直接点制冷／制热。',
  temperature_locked: '当前模式或节能锁定了温度。退出后再调。',
  economy_cooling_only: '本固件仅在制冷模式下启用节能。',
  fan_locked: '节能开启时风速和风量选项受限。',
  dry_aux_mode: '制冷／除湿使用干燥，制热使用辅热。',
  transmit_failed: '开发板没有完成红外发送，请重试。',
  transmit_busy: '已有两条指令正在发送或排队，请稍候。'
};
let ac = null;
let sending = false;
let requestVersion = 0;
let pollTimer;
let refreshing = false;
let pollController = null;
let previewTemperature = null;
let dragging = false;

async function request(url, value, controller = new AbortController()) {
  const timeout = setTimeout(() => controller.abort(), 3000);
  const options = value === undefined ? {cache:'no-store'} : {
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body:JSON.stringify(value)
  };
  try {
    const response = await fetch(url, {...options, signal:controller.signal});
    const data = await response.json();
    if (!response.ok) throw new Error(errors[data.error] || '请求失败，请重试。');
    return data;
  } finally {
    clearTimeout(timeout);
  }
}

function wait(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

function buildDialTicks() {
  const group = el('dialTicks');
  const ns = 'http://www.w3.org/2000/svg';
  for (let i = 0; i <= 28; i++) {
    const angle = (135 + i / 28 * 270) * Math.PI / 180;
    const major = i % 2 === 0;
    const r1 = major ? 126 : 128;
    const r2 = 130;
    const line = document.createElementNS(ns, 'line');
    line.setAttribute('x1', 130 + Math.cos(angle) * r1);
    line.setAttribute('y1', 130 + Math.sin(angle) * r1);
    line.setAttribute('x2', 130 + Math.cos(angle) * r2);
    line.setAttribute('y2', 130 + Math.sin(angle) * r2);
    if (major) line.setAttribute('class', 'major');
    group.appendChild(line);
  }
}

function updateDial(value) {
  const temp = Math.max(16, Math.min(30, Number(value) || 16));
  const ratio = (temp - 16) / 14;
  const progress = ratio * 1000;
  const angle = (135 + ratio * 270) * Math.PI / 180;
  const x = 130 + Math.cos(angle) * 106;
  const y = 130 + Math.sin(angle) * 106;
  el('dialProgress').style.strokeDasharray = progress + ' 1000';
  el('dialProgress').style.visibility = ratio === 0 ? 'hidden' : 'visible';
  el('dialThumb').setAttribute('cx', x);
  el('dialThumb').setAttribute('cy', y);
  el('dialThumbCore').setAttribute('cx', x);
  el('dialThumbCore').setAttribute('cy', y);
  el('temp').innerHTML = temp + '<small>°C</small>';
  el('tempDial').setAttribute('aria-valuenow', String(temp));
}

function getDialTemperature(event) {
  const rect = el('tempDial').getBoundingClientRect();
  const x = (event.clientX - rect.left) * 260 / rect.width;
  const y = (event.clientY - rect.top) * 260 / rect.height;
  const dx = x - 130;
  const dy = y - 130;
  if (Math.hypot(dx, dy) < 35) return null;
  const degree = Math.atan2(dy, dx) * 180 / Math.PI;
  let progress = (degree - 135 + 360) % 360;
  if (progress > 270) {
    progress = progress - 270 < 360 - progress ? 270 : 0;
  }
  return Math.max(16, Math.min(30, 16 + Math.round(progress / 270 * 14)));
}

function showProgress() {
  if (!ac) return;
  el('message').textContent = !ac.ready ? '红外发射接口初始化失败。'
    : ac.failed ? errors.transmit_failed
    : '';
}

function schedulePoll() {
  clearTimeout(pollTimer);
  if (document.hidden) return;
  const interval = !ac ? 2000 : ac.pending ? 180 : 5000;
  pollTimer = setTimeout(refreshAc, interval);
}

function canSendCommand() {
  return Boolean(ac && ac.ready && !sending && Number(ac.pending) < 2);
}

function render() {
  el('network').textContent = ac ? '控制版已连接' : '正在连接控制版…';
  const active = Boolean(ac && ac.power && ac.sent && !ac.failed);
  document.body.classList.toggle('online', Boolean(ac));
  document.body.classList.toggle('running', active);
  document.body.classList.toggle('swing', active && ac.vertical);
  document.body.classList.toggle('swing-x', active && ac.horizontal);
  document.body.dataset.mode = ac ? ac.mode : '';

  const modeColors = {
    '制冷':'#087eae',
    '制热':'#b24f22',
    '除湿':'#7754a8',
    '送风':'#127b69',
    '自动':'#4163a5'
  };
  const accent = ac ? (modeColors[ac.mode] || '#087eae') : '#087eae';
  document.documentElement.style.setProperty('--accent', accent);
  document.documentElement.style.setProperty('--accent-soft', accent + '24');
  document.documentElement.style.setProperty('--glow', accent + '55');

  const fan = ac ? Math.max(0, Math.min(5, Number(ac.fan) || 0)) : 0;
  const fanVisual = fan === 0 ? 2 : fan;
  const fanPower = ac && ac.turbo ? 1 : ac && ac.quiet ? .28 : .28 + fanVisual * .14;
  const windSpeed = ac && ac.turbo ? .85 : ac && ac.quiet ? 4.2 : 3.8 - fanVisual * .48;
  document.documentElement.style.setProperty('--fan-power', String(Math.min(1, fanPower)));
  document.documentElement.style.setProperty('--wind-speed', windSpeed + 's');

  const shownTemp = ac ? (previewTemperature === null ? Number(ac.temperature) : previewTemperature) : null;
  if (shownTemp === null || !Number.isFinite(shownTemp)) {
    updateDial(16);
    el('temp').innerHTML = '—<small>°C</small>';
    el('dial-hint').textContent = '等待设备连接';
  } else {
    updateDial(shownTemp);
    el('dial-hint').textContent = dragging ? '松开以应用温度' : previewTemperature !== null ? '正在应用设定…' : '';
  }

  el('scene-power').textContent = ac ? (ac.power ? '开机' : '关机') : '—';
  el('scene-power').classList.toggle('on', Boolean(ac && ac.power));
  el('scene-status').textContent = ac ? (ac.ready ? '红外发射就绪' : '红外接口异常') : '等待连接';
  el('scene-status').classList.toggle('offline', !ac || !ac.ready);
  el(isBedroom ? 'wall-temp' : 'unit-temp-svg').textContent = active && Number.isFinite(shownTemp) ? shownTemp + '°' : '—';
  el(isBedroom ? 'wall-mode' : 'lcd-mode').textContent = active ? ac.mode : '';
  const fanLabel = fan ? fan + ' 档' : '自动';
  el(isBedroom ? 'wall-fan' : 'lcd-fan').textContent = active ? (isBedroom ? fanLabel : '风量 ' + fanLabel) : '';

  const enabled = canSendCommand();
  const controlsEnabled = enabled && ac.power;
  for (const button of buttons) {
    const {key, modeLabel} = button.dataset;
    button.disabled = key === 'power' ? !enabled : !controlsEnabled;
    if (button.hasAttribute('aria-pressed')) {
      const pressed = Boolean(ac && ac.power && (modeLabel ? ac.mode === modeLabel : ac[key] === true));
      button.setAttribute('aria-pressed', String(pressed));
    }
  }
  for (const button of stepButtons) button.disabled = !controlsEnabled;
  const dial = el('tempDial');
  dial.setAttribute('aria-disabled', String(!controlsEnabled));
  dial.tabIndex = controlsEnabled ? 0 : -1;

  const bars = Array.from(el('fan-bars').querySelectorAll('i'));
  bars.forEach((bar, index) => bar.classList.toggle('on', active && index < fanVisual));
  el('fan-value').textContent = active ? (fan ? fan + ' 档' : '自动风') : '—';
  el('fan-desc').textContent = !active ? '送风停止' : ac.turbo ? '强劲送风' : ac && ac.quiet ? '静音柔风' : fan ? ['','低速柔风','中低风量','中速送风','高速送风','最大风量'][fan] : '自动调节';
  el('power-caption').textContent = active ? '空调已发送开机设定' : ac && ac.sent ? '空调已发送关机设定' : '空调电源';

  if (!ac) {
    el('power').textContent = '离线';
    return;
  }

  el('power').textContent = ac.pending ? '待发送设定' : ac.sent ? (ac.power ? '开机设定' : '关机设定') : '尚未发送';
}

async function sendKey(key) {
  if (!canSendCommand() || (key !== 'power' && !ac.power)) return;
  sending = true;
  previewTemperature = null;
  ++requestVersion;
  if (pollController) pollController.abort();
  clearTimeout(pollTimer);
  render();
  el('message').textContent = '';
  try {
    ac = await request(acEndpoint, {key});
    showProgress();
  } catch (error) {
    if (error.name === 'AbortError' || error instanceof TypeError) {
      ac = null;
      el('message').textContent = '指令结果未知，正在重新读取状态。';
    } else {
      el('message').textContent = error.message;
    }
  } finally {
    sending = false;
    render();
    schedulePoll();
  }
}

async function sendTemperatureTo(target) {
  if (!canSendCommand() || !ac.power) return;
  target = Math.max(16, Math.min(30, Math.round(target)));
  let current = Number(ac.temperature);
  if (!Number.isFinite(current) || current === target) {
    previewTemperature = null;
    render();
    return;
  }
  previewTemperature = target;
  sending = true;
  ++requestVersion;
  if (pollController) pollController.abort();
  clearTimeout(pollTimer);
  render();
  el('message').textContent = '';
  try {
    let waitCount = 0;
    while (current !== target) {
      while (ac && Number(ac.pending) >= 2 && waitCount < 25) {
        await wait(180);
        ac = await request(acEndpoint);
        render();
        waitCount++;
      }
      if (ac && Number(ac.pending) >= 2) throw new Error('红外发送队列繁忙，请稍后再调温。');
      const key = target < current ? 'cooler' : 'warmer';
      ac = await request(acEndpoint, {key});
      const nextTemp = Number(ac.temperature);
      if (!Number.isFinite(nextTemp) || nextTemp === current) break;
      current = nextTemp;
      render();
      if (ac.failed) throw new Error(errors.transmit_failed);
    }
    if (current !== target) {
      el('message').textContent = '温度指令未完全应用，请检查空调模式或稍后重试。';
    } else {
      showProgress();
    }
  } catch (error) {
    if (error.name === 'AbortError' || error instanceof TypeError) {
      el('message').textContent = '温度指令结果未知，正在重新读取状态。';
    } else {
      el('message').textContent = error.message;
    }
  } finally {
    sending = false;
    previewTemperature = null;
    render();
    schedulePoll();
  }
}

async function refreshAc() {
  if (sending || refreshing || document.hidden) return;
  refreshing = true;
  const version = requestVersion;
  pollController = new AbortController();
  try {
    const latest = await request(acEndpoint, undefined, pollController);
    if (version !== requestVersion) return;
    ac = latest;
    showProgress();
    render();
  } catch {
    if (version !== requestVersion) return;
    ac = null;
    render();
    el('message').textContent = '等待 AIHome，连上后会自动恢复。';
  } finally {
    refreshing = false;
    pollController = null;
    if (version === requestVersion) schedulePoll();
  }
}

for (const button of buttons) {
  button.addEventListener('click', () => sendKey(button.dataset.key));
}

for (const button of stepButtons) {
  button.addEventListener('click', () => {
    if (!ac) return;
    const current = previewTemperature === null ? Number(ac.temperature) : previewTemperature;
    sendTemperatureTo(current + Number(button.dataset.tempStep));
  });
}

const dial = el('tempDial');
dial.addEventListener('pointerdown', event => {
  if (!canSendCommand() || !ac.power) return;
  const value = getDialTemperature(event);
  if (value === null) return;
  dragging = true;
  previewTemperature = value;
  dial.classList.add('dragging');
  if (dial.setPointerCapture) dial.setPointerCapture(event.pointerId);
  updateDial(value);
  el('dial-hint').textContent = '松开以应用温度';
  event.preventDefault();
});
dial.addEventListener('pointermove', event => {
  if (!dragging) return;
  const value = getDialTemperature(event);
  if (value === null) return;
  previewTemperature = value;
  updateDial(value);
  el('dial-hint').textContent = '目标温度 ' + value + '°C';
  event.preventDefault();
});
function finishDialDrag() {
  if (!dragging) return;
  dragging = false;
  dial.classList.remove('dragging');
  const target = previewTemperature;
  if (target !== null) sendTemperatureTo(target);
}
dial.addEventListener('pointerup', finishDialDrag);
dial.addEventListener('pointercancel', finishDialDrag);
dial.addEventListener('lostpointercapture', finishDialDrag);
dial.addEventListener('keydown', event => {
  if (event.key === 'ArrowUp' || event.key === 'ArrowRight') {
    event.preventDefault();
    if (ac) sendTemperatureTo(Number(ac.temperature) + 1);
  } else if (event.key === 'ArrowDown' || event.key === 'ArrowLeft') {
    event.preventDefault();
    if (ac) sendTemperatureTo(Number(ac.temperature) - 1);
  }
});

window.addEventListener('pageshow', refreshAc);
window.addEventListener('online', refreshAc);
document.addEventListener('visibilitychange', () => {
  if (document.hidden) clearTimeout(pollTimer);
  else refreshAc();
});

buildDialTicks();
updateDial(24);
refreshAc();
</script>
</body>
</html>)HTML";
