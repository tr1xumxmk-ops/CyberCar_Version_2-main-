#ifndef SPARK_CAR_CAPTIVE_PORTAL_H
#define SPARK_CAR_CAPTIVE_PORTAL_H

#include <Arduino.h>
#include <WebServer.h>
#include "secure_backend.h"

// Captive-portal navigation and the embedded HTML/CSS/JS UI; face photos are served from PROGMEM by face_routes.h.
static void redirectToPortal() {
  addSecurityHeaders();
  webServer.sendHeader("Location", "http://192.168.4.1/");
  webServer.send(302, "text/plain; charset=utf-8", "");
}

static const char PAGE_HTML[] PROGMEM = R"SPARKHTML(
<!doctype html>
<html lang="en" dir="ltr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<meta name="theme-color" content="#060713">
<title>SPARK CAR // SPARK TEAM NEON CONTROL</title>
<style>
:root{--bg:#060713;--panel:rgba(11,14,34,.90);--line:rgba(0,225,255,.28);--cyan:#00e5ff;--pink:#ff32cb;--violet:#8a5cff;--lime:#b9ff3b;--red:#ff315a;--text:#eff7ff;--muted:#91a3c8;--radius:18px;--shadow:0 15px 46px rgba(0,0,0,.44)}
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent}html{scroll-behavior:smooth}body{min-height:100vh;overflow-x:hidden;color:var(--text);background:var(--bg);font-family:Inter,ui-sans-serif,system-ui,-apple-system,"Segoe UI",sans-serif}
body:before{content:"";position:fixed;inset:0;z-index:-2;pointer-events:none;background:radial-gradient(ellipse at 10% 6%,rgba(0,229,255,.14),transparent 32%),radial-gradient(ellipse at 90% 12%,rgba(255,50,203,.13),transparent 34%),linear-gradient(rgba(0,229,255,.035) 1px,transparent 1px),linear-gradient(90deg,rgba(0,229,255,.035) 1px,transparent 1px);background-size:auto,auto,31px 31px,31px 31px;animation:gridDrift 22s linear infinite}
body:after{content:"";position:fixed;inset:0;z-index:5;pointer-events:none;background:repeating-linear-gradient(0deg,transparent 0 3px,rgba(255,255,255,.012) 4px);opacity:.34}
@keyframes gridDrift{to{background-position:0 0,0 0,31px 62px,62px 31px}}@keyframes glow{0%,100%{filter:drop-shadow(0 0 3px var(--cyan))}50%{filter:drop-shadow(0 0 11px var(--pink))}}@keyframes float{0%,100%{transform:translateY(0)}50%{transform:translateY(-5px)}}@keyframes scan{from{transform:translateY(-130%)}to{transform:translateY(360%)}}@keyframes spin{to{transform:rotate(360deg)}}@keyframes breathe{0%,100%{opacity:.64}50%{opacity:1}}@keyframes dancing{0%,100%{transform:rotate(-2deg) translateY(0)}50%{transform:rotate(2deg) translateY(-4px)}}
button,input{font:inherit}button{cursor:pointer;color:inherit}button:focus-visible,input:focus-visible{outline:2px solid var(--lime);outline-offset:3px}button:disabled{opacity:.38;cursor:not-allowed}.wrap{width:min(1220px,calc(100% - 28px));margin:0 auto}.topbar{display:flex;align-items:center;justify-content:space-between;gap:12px;margin:14px auto 0;padding:12px 15px;border:1px solid var(--line);border-radius:16px;background:rgba(7,9,24,.82);backdrop-filter:blur(16px);position:relative;z-index:2}.brand{display:flex;align-items:center;gap:11px}.brandmark{width:38px;height:38px;display:grid;place-items:center;border-radius:11px;border:1px solid var(--cyan);color:var(--cyan);font-weight:1000;box-shadow:0 0 18px rgba(0,229,255,.17);font-size:1.1rem}.brand-name{font-size:.83rem;font-weight:1000;letter-spacing:.17em}.brand-name small{display:block;margin-top:3px;font-size:.54rem;letter-spacing:.21em;color:var(--muted)}.top-right{display:flex;align-items:center;gap:8px;flex-wrap:wrap;justify-content:flex-end}.status{font-size:.65rem;letter-spacing:.08em;color:#a3ffec;display:flex;align-items:center;gap:8px}.connection-dot{width:8px;height:8px;border-radius:50%;background:#41ffb3;box-shadow:0 0 10px #41ffb3;animation:breathe 1.5s infinite}.connection-dot.off{background:var(--red);box-shadow:0 0 8px var(--red);animation:none}.top-pill{border:1px solid rgba(255,50,203,.32);background:rgba(255,50,203,.06);border-radius:10px;padding:8px 10px;color:#ffb7ef;font-size:.62rem;font-weight:850;letter-spacing:.08em}
main{padding:18px 0 28px}.hero{position:relative;overflow:hidden;display:grid;grid-template-columns:1.2fr .8fr;gap:16px;align-items:center;padding:clamp(22px,4vw,42px);border:1px solid rgba(0,229,255,.32);border-radius:24px;background:linear-gradient(112deg,rgba(9,18,42,.96),rgba(24,9,40,.91));box-shadow:var(--shadow);animation:appear .6s both}@keyframes appear{from{opacity:0;transform:translateY(10px)}to{opacity:1;transform:translateY(0)}}.hero:before{content:"";position:absolute;left:8%;right:8%;top:0;height:2px;background:linear-gradient(90deg,transparent,var(--cyan),var(--pink),transparent);animation:glow 3s infinite}.hero:after{content:"";position:absolute;left:-25%;top:0;bottom:0;width:18%;transform:skewX(-24deg);background:linear-gradient(90deg,transparent,rgba(0,229,255,.07),transparent);animation:sweep 8s linear infinite}@keyframes sweep{to{left:130%}}.eyebrow{font-size:.64rem;font-weight:950;letter-spacing:.22em;color:var(--cyan);text-transform:uppercase;display:flex;align-items:center;gap:9px}.eyebrow:before{content:"";width:20px;height:2px;background:var(--cyan);box-shadow:0 0 9px var(--cyan)}h1{font-size:clamp(2.35rem,6.2vw,4.8rem);line-height:.91;letter-spacing:-.065em;margin:15px 0 14px;font-weight:1000;text-shadow:3px 0 rgba(255,50,203,.48),-2px 0 rgba(0,229,255,.4)}h1 span{color:var(--cyan)}.hero p{max-width:580px;color:#b4c3e1;font-size:.88rem;line-height:1.7}.hero-metrics{display:flex;flex-wrap:wrap;gap:9px;margin-top:20px}.chip{padding:9px 11px;border:1px solid rgba(0,229,255,.2);border-radius:10px;background:rgba(0,229,255,.035);color:var(--muted);font-size:.62rem;letter-spacing:.07em}.chip b{display:block;color:var(--cyan);font-size:.75rem;margin-bottom:3px}.hero-visual{justify-self:center;width:min(100%,350px);animation:float 4.8s ease-in-out infinite;position:relative}.hero-visual svg{width:100%;height:auto;filter:drop-shadow(0 0 18px rgba(0,229,255,.19))}.orbit{position:absolute;inset:4% 0 0 5%;width:90%;aspect-ratio:1;border:1px dashed rgba(255,50,203,.35);border-radius:50%;animation:spin 28s linear infinite;pointer-events:none}.orbit:after{content:"";position:absolute;left:8%;top:13%;width:7px;height:7px;border-radius:50%;background:var(--pink);box-shadow:0 0 13px var(--pink)}
.grid{display:grid;grid-template-columns:minmax(0,.92fr) minmax(0,1.3fr);gap:15px;margin-top:15px}.panel{position:relative;min-width:0;padding:18px;border:1px solid var(--line);border-radius:18px;background:var(--panel);box-shadow:var(--shadow);overflow:hidden}.panel:before{content:"";position:absolute;right:0;top:0;width:84px;height:1px;background:var(--pink);box-shadow:0 0 13px var(--pink)}.panel-head{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:15px}.panel-title{font-size:.76rem;font-weight:950;letter-spacing:.15em;text-transform:uppercase}.panel-index{color:var(--cyan);font-size:.58rem;letter-spacing:.12em}.telemetry{display:grid;grid-template-columns:1fr 1fr;gap:9px}.tile{min-width:0;border:1px solid rgba(255,255,255,.07);background:rgba(255,255,255,.022);padding:13px;border-radius:13px}.tile-label{font-size:.58rem;color:var(--muted);letter-spacing:.09em;text-transform:uppercase;margin-bottom:7px}.tile-value{font-size:1.18rem;font-weight:1000;overflow-wrap:anywhere}.cyan{color:var(--cyan)}.pink{color:var(--pink)}.lime{color:var(--lime)}.violet{color:var(--violet)}.tiny{font-size:.59rem;color:var(--muted);line-height:1.55;margin-top:6px}.meter{height:4px;border-radius:10px;background:#202345;margin-top:9px;overflow:hidden}.meter span{display:block;height:100%;width:40%;background:linear-gradient(90deg,var(--cyan),var(--pink));transition:width .4s}
.speed-row{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;margin-bottom:16px}.speed-btn{border:1px solid rgba(0,229,255,.23);background:rgba(0,229,255,.035);border-radius:11px;padding:11px 5px;font-size:.68rem;font-weight:950;letter-spacing:.08em;transition:.16s}.speed-btn small{display:block;margin-top:4px;color:var(--muted);font-size:.56rem;font-weight:600;letter-spacing:0}.speed-btn.active{border-color:var(--cyan);color:var(--cyan);background:rgba(0,229,255,.10);box-shadow:inset 0 0 16px rgba(0,229,255,.05),0 0 12px rgba(0,229,255,.07)}.speed-btn:active{transform:scale(.97)}.drive-layout{display:grid;grid-template-columns:minmax(200px,1fr) minmax(125px,.75fr);gap:15px;align-items:center}.dpad-area{display:grid;place-items:center}.dpad{width:min(100%,310px);display:grid;grid-template-columns:repeat(3,minmax(0,1fr));grid-template-rows:repeat(3,76px);gap:8px;touch-action:none;user-select:none}.drive-btn{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:4px;min-width:0;padding:7px 3px;border:1px solid rgba(0,229,255,.32);border-radius:14px;background:linear-gradient(145deg,rgba(0,229,255,.13),rgba(13,14,38,.92));box-shadow:inset 0 0 17px rgba(0,229,255,.045),0 5px 16px rgba(0,0,0,.22);color:var(--cyan);font-size:1.45rem;font-weight:1000;line-height:1;touch-action:none;user-select:none;transition:transform .08s,border-color .08s,background .08s}.drive-btn small{font-size:.49rem;line-height:1.2;letter-spacing:.06em;color:var(--muted);font-weight:900}.drive-btn:active,.drive-btn.pressed{transform:scale(.97);border-color:var(--lime);background:rgba(185,255,59,.13);color:var(--lime)}.drive-forward{grid-column:2;grid-row:1}.drive-left{grid-column:1;grid-row:2}.drive-stop{grid-column:2;grid-row:2;color:var(--lime);border-color:rgba(185,255,59,.38)}.drive-right{grid-column:3;grid-row:2}.drive-backward{grid-column:2;grid-row:3}.drive-hints{display:grid;gap:8px}.hint{display:flex;gap:8px;align-items:flex-start;padding:10px;border-radius:10px;background:rgba(255,255,255,.025);color:var(--muted);font-size:.63rem;line-height:1.45}.hint b{display:block;color:var(--text);margin-bottom:2px;font-size:.68rem}.hint-icon{flex:0 0 24px;width:24px;height:24px;display:grid;place-items:center;border-radius:7px;color:var(--cyan);background:rgba(0,229,255,.09);font-weight:950}.btn-row{display:grid;grid-template-columns:1fr 1fr;gap:9px;margin-top:15px}.action-btn{padding:11px;border:1px solid rgba(0,229,255,.24);background:rgba(0,229,255,.045);border-radius:11px;font-size:.66rem;font-weight:900;letter-spacing:.04em}.action-btn:hover{border-color:var(--cyan)}.brake-btn{grid-column:1/-1;border-color:rgba(255,49,90,.7);background:rgba(255,49,90,.12);color:#ff98ac;box-shadow:inset 0 0 14px rgba(255,49,90,.04)}.resume-btn{grid-column:1/-1;border-color:rgba(185,255,59,.3);color:var(--lime)}.servo-row{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-bottom:10px}.servo-angle{font-size:1.25rem;color:var(--pink);font-weight:1000}.slider{width:100%;accent-color:var(--pink);cursor:pointer}.slider-labels{display:flex;justify-content:space-between;color:var(--muted);font-size:.58rem;margin-top:5px}.servo-ticks{display:flex;gap:6px;flex-wrap:wrap;margin-top:12px}.servo-tick{padding:7px 9px;border:1px solid rgba(255,50,203,.24);background:rgba(255,50,203,.035);border-radius:8px;font-size:.59rem}.servo-tick:hover{border-color:var(--pink);color:#ffb7ef}.servo-note{margin-top:10px;padding:10px;border-left:2px solid var(--pink);background:rgba(255,50,203,.045);color:var(--muted);font-size:.62rem;line-height:1.55}
.face-panel{display:grid;grid-template-columns:150px 1fr;align-items:center;gap:14px}.face-shell{width:100%;max-width:175px;aspect-ratio:3/2;justify-self:center;filter:drop-shadow(0 0 13px rgba(0,229,255,.17));animation:float 3.8s ease-in-out infinite;border:1px solid rgba(0,229,255,.28);border-radius:12px;overflow:hidden;background:#030613;display:grid;place-items:center}.face-image{width:100%;height:100%;object-fit:contain;display:block;transition:filter .22s,transform .22s}.face-image[data-mood="fast"]{filter:drop-shadow(0 0 11px var(--lime));transform:scale(1.025)}.face-image[data-mood="left"]{transform:rotate(-2deg) scale(1.02)}.face-image[data-mood="right"]{transform:rotate(2deg) scale(1.02)}.face-image[data-mood="brake"]{filter:drop-shadow(0 0 10px var(--red))}.face-image[data-mood="dance"]{animation:dancing .55s ease-in-out infinite}.face-meta{min-width:0}.face-kicker{color:var(--cyan);font-size:.59rem;font-weight:950;letter-spacing:.15em}.face-title{font-size:1.2rem;font-weight:1000;margin:8px 0}.face-desc{font-size:.67rem;color:var(--muted);line-height:1.55}.face-badges{display:flex;gap:6px;flex-wrap:wrap;margin-top:11px}.face-badge{border:1px solid rgba(0,229,255,.22);padding:6px 8px;border-radius:7px;color:#bdd5f4;font-size:.55rem}
.team-grid{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:10px}.member{--team:#00e5ff;min-width:0;border:1px solid color-mix(in srgb,var(--team) 55%,transparent);border-radius:13px;background:linear-gradient(155deg,color-mix(in srgb,var(--team) 8%,transparent),rgba(255,255,255,.018));padding:13px 10px;text-align:center;position:relative;overflow:hidden}.member:before{content:"";position:absolute;left:10%;right:10%;top:0;height:2px;background:var(--team);box-shadow:0 0 12px var(--team)}.member-avatar{width:42px;height:42px;margin:4px auto 10px;display:grid;place-items:center;border-radius:13px;border:1px solid var(--team);background:color-mix(in srgb,var(--team) 9%,transparent);color:var(--team);font-size:.83rem;font-weight:1000;box-shadow:0 0 15px color-mix(in srgb,var(--team) 12%,transparent)}.member-name{color:var(--team);font-size:.72rem;font-weight:1000;line-height:1.35;overflow-wrap:anywhere}.member-role{font-size:.61rem;color:var(--text);margin-top:6px;font-weight:800;line-height:1.35}.member-dept{font-size:.54rem;color:var(--muted);margin-top:6px;line-height:1.35}.team-foot{display:flex;align-items:center;justify-content:space-between;gap:12px;flex-wrap:wrap;margin-top:15px;padding-top:12px;border-top:1px solid rgba(255,255,255,.07);font-size:.61rem;color:var(--muted)}.school-meta{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:9px;margin-top:14px}.school-meta div{padding:11px;border-radius:10px;border:1px solid rgba(138,92,255,.2);background:rgba(138,92,255,.045)}.school-meta small{display:block;color:var(--muted);font-size:.56rem;margin-bottom:5px}.school-meta b{font-size:.65rem;color:#d9d0ff;overflow-wrap:anywhere}.footer{display:flex;justify-content:space-between;align-items:center;gap:12px;flex-wrap:wrap;padding:18px 2px 8px;color:var(--muted);font-size:.61rem}.footer strong{color:var(--pink);letter-spacing:.12em}.toast{position:fixed;left:50%;bottom:20px;transform:translate(-50%,20px);opacity:0;pointer-events:none;z-index:30;max-width:calc(100% - 28px);padding:12px 16px;border:1px solid rgba(0,229,255,.3);border-radius:12px;background:#0d1128;color:#f3fbff;font-size:.72rem;box-shadow:0 10px 35px rgba(0,0,0,.5);transition:.22s}.toast.show{opacity:1;transform:translate(-50%,0)}.gate{position:fixed;inset:0;z-index:50;display:grid;place-items:center;padding:20px;background:rgba(3,4,15,.82);backdrop-filter:blur(12px)}.gate.hidden{display:none}.gate-card{width:min(410px,100%);border:1px solid rgba(0,229,255,.38);border-radius:19px;background:linear-gradient(145deg,#101631,#1a0e2c);padding:24px;box-shadow:0 0 50px rgba(0,229,255,.1)}.gate-card h2{font-size:1.3rem;letter-spacing:.05em;margin:12px 0 7px}.gate-card p{font-size:.72rem;color:var(--muted);line-height:1.6;margin-bottom:16px}.gate-card label{display:block;color:var(--cyan);font-size:.64rem;margin-bottom:6px}.gate-card input{width:100%;padding:13px;border-radius:10px;border:1px solid rgba(0,229,255,.3);background:#070b1c;color:#fff;outline:none}.gate-card button{width:100%;margin-top:12px;padding:12px;border:1px solid var(--pink);border-radius:10px;background:rgba(255,50,203,.1);font-size:.72rem;font-weight:950}.gate-error{min-height:17px;color:#ff96b0;font-size:.65rem;margin-top:8px}.demo-note{display:none;margin-top:12px;font-size:.61rem;color:var(--lime)}
@media(max-width:850px){.grid{grid-template-columns:1fr}.hero{grid-template-columns:1fr .68fr}.hero-visual{max-width:260px}.team-grid{grid-template-columns:repeat(2,minmax(0,1fr))}}@media(max-width:580px){.wrap{width:min(100% - 18px,520px)}.topbar{margin-top:9px;padding:10px}.brand-name{font-size:.72rem}.top-pill{display:none}.hero{grid-template-columns:1fr;padding:21px}.hero-visual{max-width:210px;margin-top:5px}.grid{gap:10px;margin-top:10px}.panel{padding:14px}.drive-layout{grid-template-columns:minmax(0,1fr)}.dpad{width:min(100%,330px);grid-template-rows:repeat(3,70px)}.drive-hints{grid-template-columns:1fr 1fr}.face-panel{grid-template-columns:120px 1fr}.team-grid{gap:8px}.school-meta{grid-template-columns:1fr}.footer{font-size:.58rem}.panel-title{font-size:.7rem}}
@media(prefers-reduced-motion:reduce){*,*:before,*:after{animation-duration:.01ms!important;animation-iteration-count:1!important;scroll-behavior:auto!important;transition-duration:.01ms!important}}

/* Vehicle motion reacts to the command state returned by the controller. */
.hero-visual{isolation:isolate;transition:filter .2s ease}.hero-visual .vehicle-svg{position:relative;z-index:1;transform-origin:50% 70%;transition:transform .18s ease;will-change:transform}.drift-lines{position:absolute;z-index:0;left:-3%;right:-3%;top:46%;height:32%;opacity:0;pointer-events:none;background:repeating-linear-gradient(170deg,transparent 0 9px,rgba(255,50,203,.7) 10px 12px,transparent 13px 23px);filter:drop-shadow(0 0 6px rgba(255,50,203,.6));transform:skewX(-18deg)}
@keyframes sparkForward{0%,100%{transform:translateY(0) scale(1)}50%{transform:translateY(-7px) scale(1.012)}}@keyframes sparkReverse{0%,100%{transform:translateY(0)}50%{transform:translateY(5px)}}@keyframes sparkDriftLeft{0%,100%{transform:translateX(0) rotate(0)}35%{transform:translateX(-10px) rotate(-5deg)}70%{transform:translateX(4px) rotate(2deg)}}@keyframes sparkDriftRight{0%,100%{transform:translateX(0) rotate(0)}35%{transform:translateX(10px) rotate(5deg)}70%{transform:translateX(-4px) rotate(-2deg)}}@keyframes driftStreak{0%{transform:translateX(18px) skewX(-18deg);opacity:0}35%{opacity:.9}100%{transform:translateX(-22px) skewX(-18deg);opacity:0}}@keyframes brakeJolt{0%,100%{transform:translateX(0)}30%{transform:translateX(-4px)}60%{transform:translateX(4px)}}
.hero-visual[data-motion="forward"] .vehicle-svg{animation:sparkForward .42s ease-in-out infinite}.hero-visual[data-motion="backward"] .vehicle-svg{animation:sparkReverse .45s ease-in-out infinite}.hero-visual[data-motion="left"] .vehicle-svg{transform:rotate(-4deg) translateX(-4px)}.hero-visual[data-motion="right"] .vehicle-svg{transform:rotate(4deg) translateX(4px)}.hero-visual[data-motion="drift-left"] .vehicle-svg{animation:sparkDriftLeft .34s ease-in-out infinite}.hero-visual[data-motion="drift-right"] .vehicle-svg{animation:sparkDriftRight .34s ease-in-out infinite}.hero-visual[data-motion^="drift"] .drift-lines{opacity:1;animation:driftStreak .38s linear infinite}.hero-visual[data-motion="brake"] .vehicle-svg{animation:brakeJolt .2s linear 1;filter:drop-shadow(0 0 15px rgba(255,49,90,.7))}.hero-visual[data-motion="stop"] .vehicle-svg{animation:none;transform:none}.hero-visual[data-mode="auto"] .orbit{border-color:rgba(185,255,59,.7)}.hero-visual[data-mode="qlearning"] .orbit{border-color:rgba(138,92,255,.75)}
.mode-console{margin-top:15px}.mode-choice-row{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:9px}.mode-choice{min-width:0;min-height:65px;text-align:left;padding:12px 14px;border:1px solid rgba(0,229,255,.25);border-radius:12px;background:rgba(0,229,255,.035);transition:transform .15s,border-color .15s,background .15s}.mode-choice b{display:block;font-size:.76rem;letter-spacing:.07em;color:var(--text)}.mode-choice small{display:block;font-size:.62rem;color:var(--muted);margin-top:5px}.mode-choice:hover{transform:translateY(-2px)}.mode-choice.active{border-color:var(--pink);background:rgba(255,50,203,.10);box-shadow:inset 0 0 0 1px rgba(255,50,203,.08),0 0 16px rgba(255,50,203,.08)}.mode-choice.active b{color:var(--pink)}.mode-details{display:flex;gap:14px;align-items:center;margin-top:12px;padding:12px;border:1px solid rgba(138,92,255,.25);background:rgba(138,92,255,.045);border-radius:12px}.mode-details strong{display:block;color:var(--cyan);font-size:.8rem;margin-top:5px}.mode-details p{font-size:.69rem;color:var(--muted);line-height:1.5}.rl-sim{margin-top:12px;padding:13px;border:1px dashed rgba(185,255,59,.28);border-radius:13px;background:rgba(185,255,59,.025)}.rl-sim-head{display:flex;justify-content:space-between;gap:8px;align-items:center;color:var(--lime);font-size:.73rem;letter-spacing:.1em}.sim-tag{font-size:.55rem;padding:5px 7px;border:1px solid rgba(185,255,59,.3);border-radius:6px}.rl-sim>p{font-size:.65rem;color:var(--muted);line-height:1.55;margin:9px 0}.rl-stats{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:8px}.rl-stats>div{padding:8px;border:1px solid rgba(255,255,255,.07);border-radius:9px;background:rgba(0,0,0,.16);min-width:0}.rl-stats small{display:block;color:var(--muted);font-size:.52rem}.rl-stats b{display:block;margin-top:4px;font-size:.75rem;color:var(--text);overflow-wrap:anywhere}.rl-status{font-size:.59rem;letter-spacing:.08em;color:var(--lime);margin:10px 0}.rl-button{border-color:rgba(185,255,59,.4);color:var(--lime)}.mode-warning{font-size:.63rem;color:#ffcf88;line-height:1.5;margin-top:11px}
/* Extra low-memory vector face variants are driven by data-expression. */
@media(max-width:640px){.mode-choice-row{grid-template-columns:1fr}.mode-choice{min-height:54px}.mode-details{align-items:flex-start;flex-direction:column}.rl-stats{grid-template-columns:repeat(3,minmax(0,1fr))}}

</style>
</head>
<body>
<header class="topbar wrap">
  <div class="brand"><div class="brandmark">S/</div><div class="brand-name">SPARK CAR <span style="color:var(--pink)">SPARK TEAM</span><small>NEON CONTROL // ESP32-S2</small></div></div>
  <div class="top-right"><div class="status"><i class="connection-dot" id="connectionIndicator"></i><span id="connectionText">LOCAL SYSTEM</span></div><span class="top-pill">NO CLOUD // DIRECT WIFI</span><span class="top-pill">AP PASSWORD ONLY</span></div>
</header>
<main class="wrap">
<section class="hero">
 <div><div class="eyebrow">SPARK TEAM // ROBOTICS LAB</div><h1>SPARK <span>CAR</span><br>NEON DRIVE</h1><p>A hand-built robot control console by SPARK TEAM. Direct ESP32-S2 Wi-Fi, manual driving, a ball-release servo, AUTO demo, Q-learning simulation, and a live vehicle animation.</p><div class="hero-metrics"><div class="chip"><b>ESP32-S2</b>CONTROL CORE</div><div class="chip"><b>192.168.4.1</b>LOCAL PORTAL</div><div class="chip"><b id="heapHero">— KB</b>FREE HEAP</div></div></div>
 <div class="hero-visual" id="vehicleStage" data-motion="stop" data-mode="manual" data-speed="fast"><div class="orbit"></div><svg id="carIllustration" class="vehicle-svg" viewBox="0 0 340 245" role="img" aria-label="Spark Car animated vehicle illustration"><defs><linearGradient id="carbody" x1="0" y1="0" x2="1" y2="1"><stop stop-color="#082a49"/><stop offset=".55" stop-color="#20164c"/><stop offset="1" stop-color="#071426"/></linearGradient><linearGradient id="glass" x1="0" y1="0" x2="1" y2="1"><stop stop-color="#00e5ff" stop-opacity=".75"/><stop offset="1" stop-color="#ff32cb" stop-opacity=".24"/></linearGradient></defs><ellipse cx="170" cy="214" rx="122" ry="17" fill="#00e5ff" opacity=".09"/><g stroke="#00e5ff" stroke-width="2.5" stroke-linejoin="round"><path d="M44 145 L62 105 Q71 91 96 86 L132 47 Q144 35 168 35 L203 39 Q225 43 237 65 L264 99 L291 116 L302 151 L284 176 L59 176 L39 160 Z" fill="url(#carbody)"/><path d="M94 91 L137 53 Q148 46 166 47 L198 49 Q218 53 227 69 L244 96 Z" fill="url(#glass)"/><path d="M69 111 L103 104 L98 139 L52 143 Z" fill="#091629"/><path d="M242 106 L273 120 L283 145 L245 140 Z" fill="#091629"/><path d="M109 107 L231 107 L245 147 L96 147 Z" fill="#121432" stroke="#ff32cb"/><path d="M115 117 L149 117 M164 117 L196 117 M209 117 L230 117" stroke="#00e5ff" stroke-width="5" stroke-linecap="round"/><path d="M111 163 L94 188 L121 194 L141 175 M226 163 L245 188 L219 194 L200 175" fill="#111831" stroke="#ff32cb"/><path d="M77 154 L62 178 M263 154 L279 178" stroke="#00e5ff" stroke-width="4"/><circle cx="80" cy="181" r="19" fill="#080b19" stroke="#ff32cb" stroke-width="4"/><circle cx="80" cy="181" r="7" fill="#00e5ff" stroke="none"/><circle cx="260" cy="181" r="19" fill="#080b19" stroke="#ff32cb" stroke-width="4"/><circle cx="260" cy="181" r="7" fill="#00e5ff" stroke="none"/><path d="M126 163 L215 163" stroke="#b9ff3b" stroke-width="4" stroke-linecap="round"/><circle cx="98" cy="151" r="5" fill="#ff32cb" stroke="none"/><circle cx="242" cy="151" r="5" fill="#ff32cb" stroke="none"/></g><g fill="#00e5ff" font-family="monospace" font-size="8" letter-spacing="2"><text x="133" y="20">SPARK / ROBO UNIT</text><text x="136" y="228">SYSTEM ONLINE</text></g></svg><div class="drift-lines" id="driftLines" aria-hidden="true"></div></div>
</section>
<section class="panel mode-console" aria-label="Control modes">
 <div class="panel-head"><h2 class="panel-title">Control brain / الوضع</h2><span class="panel-index">MODE / 00</span></div>
 <div class="mode-choice-row">
  <button class="mode-choice active" type="button" data-mode-choice="manual"><b>MANUAL</b><small>Hold-to-drive buttons</small></button>
  <button class="mode-choice" type="button" data-mode-choice="auto"><b>AUTO</b><small>Short timed demo</small></button>
  <button class="mode-choice" type="button" data-mode-choice="qlearning"><b>Q-LEARNING</b><small>Policy simulation</small></button>
 </div>
 <div class="mode-details"><div><span class="tile-label">CURRENT MODE</span><strong id="modeValue">MANUAL</strong></div><p id="modeSummary">Hold a direction button to drive; release to stop.</p></div>
 <div class="rl-sim"><div class="rl-sim-head"><b>Q-LEARNING LAB</b><span class="sim-tag">SIMULATION ONLY</span></div><p>Because the distance sensor has been removed, this demo does not send Q-learning actions to the real motors. Connect a feedback sensor and define a reward signal before enabling real RL control.</p><div class="rl-stats"><div><small>EPISODE</small><b id="rlEpisode">0</b></div><div><small>DEMO REWARD</small><b id="rlReward">0</b></div><div><small>POLICY ACTION</small><b id="rlAction">WAITING</b></div></div><div id="rlStatus" class="rl-status">READY · SELECT Q-LEARNING</div><button type="button" id="rlTrainBtn" class="action-btn rl-button">▶ RUN POLICY PREVIEW</button></div>
 <p class="mode-warning">⚠ AUTO runs one brief, slow, timed path only. It cannot detect obstacles. Test with wheels raised and a clear area. STOP or Emergency Brake cancels it.</p>
</section>
<div class="grid">
 <section class="panel"><div class="panel-head"><h2 class="panel-title">System telemetry</h2><span class="panel-index">SYS / 01</span></div><div class="telemetry">
  <div class="tile"><div class="tile-label">Drive command</div><div class="tile-value cyan" id="commandValue">STOP</div><div class="tiny">current motor direction</div></div>
  <div class="tile"><div class="tile-label">Speed profile</div><div class="tile-value pink" id="levelValue">FAST</div><div class="tiny">FAST = full continuous drive</div></div>
  <div class="tile"><div class="tile-label">Free heap</div><div class="tile-value lime" id="heapValue">— KB</div><div class="meter"><span id="heapFill"></span></div></div>
  <div class="tile"><div class="tile-label">Servo angle</div><div class="tile-value violet"><span id="telemetryServo">0</span>°</div><div class="tiny" id="servoStatus">starting at zero</div></div>
 </div><div class="panel-head" style="margin:18px 0 10px"><h2 class="panel-title">Speed profiles</h2><span class="panel-index">DIGITAL / 02</span></div>
 <div class="speed-row"><button class="speed-btn" data-level="slow">SLOW<small>85/200 ms</small></button><button class="speed-btn" data-level="medium">MEDIUM<small>145/200 ms</small></button><button class="speed-btn active" data-level="fast">FAST<small>FULL ON</small></button></div><p class="tiny">No LEDC/analogWrite motor PWM. FAST enables both motor channels continuously. MEDIUM and SLOW use software timed digital bursts and may feel pulsed.</p>
 </section>
 <section class="panel"><div class="panel-head"><h2 class="panel-title">Neon drive console</h2><span class="panel-index">MANUAL / 03</span></div>
  <div class="drive-layout"><div class="dpad-area"><div class="dpad" id="drivePad" aria-label="Four-direction car controls">
   <button class="drive-btn drive-forward" type="button" data-drive="forward" aria-label="Drive forward">▲<small>FORWARD</small></button>
   <button class="drive-btn drive-left" type="button" data-drive="left" aria-label="Turn left">◀<small>LEFT</small></button>
   <button class="drive-btn drive-stop" type="button" data-drive="stop" aria-label="Stop car">■<small>STOP</small></button>
   <button class="drive-btn drive-right" type="button" data-drive="right" aria-label="Turn right">▶<small>RIGHT</small></button>
   <button class="drive-btn drive-backward" type="button" data-drive="backward" aria-label="Drive backward">▼<small>BACKWARD</small></button>
  </div></div>
  <div class="drive-hints"><div class="hint"><span class="hint-icon">↕</span><span><b>Press and hold</b>Hold a direction to move; release to stop.</span></div><div class="hint"><span class="hint-icon">⌁</span><span><b>Fail-safe</b>If updates stop, movement times out.</span></div><div class="hint"><span class="hint-icon">⌂</span><span><b>Local Wi-Fi</b>No router or cloud required.</span></div><div class="hint"><span class="hint-icon">⌨</span><span><b>Keyboard</b>Arrow keys or WASD; Space stops.</span></div></div></div>
  <div class="btn-row"><button class="action-btn" id="hornBtn">♪ HORN</button><button class="action-btn" id="stopBtn">■ STOP</button><button class="action-btn brake-btn" id="brakeBtn">⚠ EMERGENCY BRAKE — LATCHED</button><button class="action-btn resume-btn" id="resumeBtn" disabled>↻ RESUME (STAYS STOPPED)</button></div>
 </section>
</div>
<div class="grid">
 <section class="panel"><div class="panel-head"><h2 class="panel-title">Ball servo / manual angle</h2><span class="panel-index">SERVO / 04</span></div><div class="servo-row"><span class="tile-label">Target angle</span><strong class="servo-angle"><span id="servoValue">0</span>°</strong></div>
  <input class="slider" id="servoSlider" type="range" min="0" max="180" step="1" value="0" aria-label="Ball servo angle from zero to 180 degrees"><div class="slider-labels"><span>0° / START</span><span>90° / MID</span><span>180° / MAX</span></div>
  <div class="servo-ticks"><button class="servo-tick" data-angle="0">0° HOME</button><button class="servo-tick" data-angle="45">45°</button><button class="servo-tick" data-angle="90">90°</button><button class="servo-tick" data-angle="135">135°</button><button class="servo-tick" data-angle="180">180°</button></div>
  <div class="servo-note">The servo starts at <b>0°</b> on boot. Move the slider to set the ball-release mechanism manually; the firmware does not automatically release it. Check that the linkage can move through the selected angle without jamming.</div>
 </section>
 <section class="panel"><div class="panel-head"><h2 class="panel-title">SPARK face core</h2><span class="panel-index">MOOD / 05</span></div><div class="face-panel"><div class="face-shell"><img id="faceImage" class="face-image" data-mood="idle" src="/assets/faces/idle.jpg?v=2" width="192" height="128" loading="eager" decoding="async" alt="SPARK CAR robot face: idle expression"></div><div class="face-meta"><div class="face-kicker">CURRENT EXPRESSION · <span id="faceVariantLabel">IDLE</span></div><div class="face-title" id="faceTitle">IDLE / READY</div><div class="face-desc" id="faceDescription">The supplied SPARK face artwork changes with driving state, speed profile, AUTO demo, Q-Learning preview and emergency stop.</div><div class="face-badges"><span class="face-badge">ORIGINAL TEAM ART</span><span class="face-badge">JPEG / FLASH</span><span class="face-badge">LIVE STATE</span></div></div></div>
 </section>
</div>
<section class="panel" style="margin-top:15px"><div class="panel-head"><h2 class="panel-title">The SPARK TEAM</h2><span class="panel-index">PEOPLE / 06</span></div><p class="tiny" style="margin:-5px 0 15px;font-size:.69rem">Four roles. One mission: build a smarter, better-documented robotics platform.</p>
 <div class="team-grid">
  <article class="member" style="--team:#00e5ff"><div class="member-avatar">HN</div><div class="member-name">HAIDER NATHER</div><div class="member-role">Team Lead</div><div class="member-dept">AI &amp; Robotics Engineering</div></article>
  <article class="member" style="--team:#ff32cb"><div class="member-avatar">AA</div><div class="member-name">AHMED AMJED</div><div class="member-role">AI Developer</div><div class="member-dept">AI &amp; Robotics Engineering</div></article>
  <article class="member" style="--team:#9d78ff"><div class="member-avatar">AS</div><div class="member-name">AYAT SAYED</div><div class="member-role">System Designer</div><div class="member-dept">AI &amp; Robotics Engineering</div></article>
  <article class="member" style="--team:#b9ff3b"><div class="member-avatar">BF</div><div class="member-name">BARAA FADHEL</div><div class="member-role">Robotics Designer</div><div class="member-dept">AI &amp; Robotics Engineering</div></article>
 </div>
 <div class="school-meta"><div><small>UNIVERSITY / الجامعة</small><b id="universityLabel">الجامعة العراقية</b></div><div><small>ACADEMIC YEAR / السنة</small><b id="academicYearLabel">2026–2027</b></div><div><small>PROGRAM / التخصص</small><b>AI &amp; Robotics Engineering</b></div></div>
</section>
<footer class="footer"><div><strong>SPARK TEAM</strong> · SPARK CAR · © <span id="yearLabel">2026</span> Spark Team.</div><div>For academic &amp; robotics learning · Software: Apache-2.0 where applicable · Respect included asset notices.</div></footer>
</main>
<div class="toast" id="toast" role="status" aria-live="polite"></div>
<script>
'use strict';
const DEMO_MODE = location.protocol === 'file:' || location.hostname !== '192.168.4.1';
const UNIVERSITY_NAME = 'الجامعة العراقية';
const ACADEMIC_YEAR = '2026–2027'; // Change to your institution's academic year.
const $ = id => document.getElementById(id);
let activeLevel='fast', activeMode='manual', localEstop=false, servoAngle=0, stateBusy=false, toastTimer=0, rlPreviewTimer=0, rlPreviewActive=false, faceCycleIndex=0;
let heldDirection=null, driveTimer=0, driveBusy=false, pendingDriveDirection=null, lastDriveRequest='stop', activeDrivePointerId=null, activeDriveButton=null;
let servoDragging=false, servoBusy=false, pendingServoAngle=null, lastServoSent=null, servoSendTimer=0, lastServoSentAt=0;
const SERVO_MIN_SEND_INTERVAL_MS=125;
let activeFaceMood='', activeCommand='stop', autoVisualTimer=0, autoVisualToken=0;
let demoState={command:'stop',level:'fast',mode:'manual',estop:false,servoAngle:0,servoAttached:true,heap:184000,uptime:0};
$('universityLabel').textContent=UNIVERSITY_NAME; $('academicYearLabel').textContent=ACADEMIC_YEAR; $('yearLabel').textContent=ACADEMIC_YEAR.slice(0,4);
function toast(message){clearTimeout(toastTimer);$('toast').textContent=message;$('toast').classList.add('show');toastTimer=setTimeout(()=>$('toast').classList.remove('show'),2200)}
function commandFromXY(x,y){if(Math.abs(x)>Math.abs(y))return x>22?'right':(x< -22?'left':'stop');return y>22?'forward':(y< -22?'backward':'stop')}
function setVehicleMotion(command, estop=false){
 const hero=$('vehicleStage'); if(!hero)return;
 let motion=command||'stop';
 if(estop||motion==='brake')motion='brake';
 else if((motion==='left'||motion==='right')&&activeLevel==='fast')motion='drift-'+motion;
 hero.dataset.motion=motion; hero.dataset.mode=activeMode; hero.dataset.speed=activeLevel;
 hero.classList.toggle('is-moving',!['stop','brake'].includes(motion));
}
const faceImages=Object.freeze({
 idle:'/assets/faces/idle.jpg?v=2', happy:'/assets/faces/happy.jpg?v=2', fast:'/assets/faces/dance2.jpg?v=2',
 backward:'/assets/faces/auto.jpg?v=2', left:'/assets/faces/drift.jpg?v=2', right:'/assets/faces/dance2.jpg?v=2',
 brake:'/assets/faces/brake.jpg?v=2', error:'/assets/faces/angry.jpg?v=2', dance:'/assets/faces/dance.jpg?v=2',
 dance2:'/assets/faces/dance2.jpg?v=2', drift:'/assets/faces/drift.jpg?v=2', auto:'/assets/faces/auto.jpg?v=2',
 qlearning:'/assets/faces/qlearning.jpg?v=2', boot:'/assets/faces/boot.jpg?v=2', manual:'/assets/faces/manual.jpg?v=2'
});
const faceVariants={
 idle:['idle','boot','manual'], happy:['happy','dance','dance2'], fast:['dance2','drift','happy'],
 backward:['auto','manual','idle'], left:['drift','dance','dance2'], right:['dance2','drift','happy'],
 brake:['brake'], error:['angry','boot'], dance:['dance','dance2','happy'], auto:['auto','happy','manual'], qlearning:['qlearning','auto','manual']
};
const faceCopy={
 idle:['IDLE / READY','Spark Car is waiting for your next command.'],
 happy:['FORWARD / HAPPY','The original team face artwork reacts to forward movement.'],
 fast:['BOOST / MAX DRIVE','FAST keeps both L298N enable inputs continuously HIGH.'],
 backward:['REVERSE / FOCUSED','The face changes while reverse is commanded.'],
 left:['LEFT / DRIFT','Turn-left motion selects drift and celebration artwork.'],
 right:['RIGHT / DRIFT','Turn-right motion selects another energetic frame.'],
 brake:['EMERGENCY / LOCKED','Emergency brake is latched; press Resume before driving again.'],
 error:['LINK LOST / CHECK','The dashboard cannot read the car state.'],
 dance:['SPARK / CELEBRATE','A short expressive cycle for horn/celebration feedback.'],
 auto:['AUTO / DEMO','The controller is running the bounded, sensorless demonstration.'],
 qlearning:['Q-LEARNING / LAB','This mode is a browser-side policy demonstration; motors stay stopped.']
};
function applyFaceImage(){
 const image=$('faceImage'); if(!image)return;
 const options=faceVariants[activeFaceMood]||faceVariants.idle;
 const key=options[faceCycleIndex%options.length]||'idle';
 const src=faceImages[key]||faceImages.idle;
 image.dataset.mood=activeFaceMood;
 if(image.getAttribute('src')!==src) image.setAttribute('src',src);
 image.alt='SPARK CAR face: '+key;
 const label=$('faceVariantLabel'); if(label)label.textContent=key.toUpperCase().replace('QLEARNING','Q-LEARNING');
}
function setFace(command,estop=false){
 let mood='idle';
 if(estop||command==='brake') mood='brake';
 else if(command==='error') mood='error';
 else if(command==='dance') mood='dance';
 else if(activeMode==='qlearning'&&command==='stop') mood='qlearning';
 else if(activeMode==='auto'&&command==='stop') mood='auto';
 else if(command==='forward') mood=activeLevel==='fast'?'fast':'happy';
 else if(command==='backward') mood='backward';
 else if(command==='left'||command==='drift-left') mood='left';
 else if(command==='right'||command==='drift-right') mood='right';
 const changed=activeFaceMood!==mood;
 activeFaceMood=mood; activeCommand=command;
 if(changed)faceCycleIndex=0;
 applyFaceImage();
 const item=faceCopy[mood]||faceCopy.idle;
 $('faceTitle').textContent=item[0];$('faceDescription').textContent=item[1];
 setVehicleMotion(command,estop);
}
setInterval(()=>{
 const seq=faceVariants[activeFaceMood]||faceVariants.idle;
 // Do not animate emergency/error frames; the warning image should remain stable and readable.
 if(seq.length>1&&!['brake','error'].includes(activeFaceMood)){
  faceCycleIndex=(faceCycleIndex+1)%seq.length;applyFaceImage();
 }
},1750);
function setModeVisual(mode){activeMode=mode;document.querySelectorAll('[data-mode-choice]').forEach(b=>b.classList.toggle('active',b.dataset.modeChoice===mode));$('modeValue').textContent=mode==='qlearning'?'Q-LEARNING':mode.toUpperCase();$('modeSummary').textContent=mode==='manual'?'Hold a direction button to drive; release to stop.':mode==='auto'?'Short timed auto demo; no obstacle sensor is installed.':'Q-learning policy simulator; real motors stay stopped until sensor feedback is added.';if(mode!=='auto'){autoVisualToken++;if(autoVisualTimer)clearTimeout(autoVisualTimer);autoVisualTimer=0;}setVehicleMotion(activeCommand,localEstop);}
function runAutoVisual(){autoVisualToken++;const token=autoVisualToken;if(autoVisualTimer)clearTimeout(autoVisualTimer);const steps=[['forward',420],['stop',220],['right',240],['stop',220],['forward',360],['stop',60]];let i=0;function tick(){if(token!==autoVisualToken||activeMode!=='auto'||localEstop)return;const [cmd,wait]=steps[i];if(DEMO_MODE)demoState.command=cmd;setFace(cmd,false);i++;if(i<steps.length)autoVisualTimer=setTimeout(tick,wait);else{autoVisualTimer=0;if(DEMO_MODE)demoState.command='stop';setFace('stop',false);}}tick();}
function runQLearningPreview(){
 if(rlPreviewTimer)clearInterval(rlPreviewTimer);rlPreviewActive=true;let step=0, episode=Number($('rlEpisode').textContent)||0, reward=0;
 const actions=[['forward','EXPLORE / FORWARD',2],['left','TURN / TEST PATH',1],['forward','EXPLOIT / BEST ACTION',3],['stop','RESET / SAFE STOP',1]];
 $('rlStatus').textContent='SIMULATING POLICY · MOTORS NOT COMMANDED';
 rlPreviewTimer=setInterval(()=>{const a=actions[step%actions.length];$('rlAction').textContent=a[1];reward+=a[2];$('rlReward').textContent=String(reward);setVehicleMotion(a[0],false);setFace(a[0]);step++;if(step>=actions.length){clearInterval(rlPreviewTimer);rlPreviewTimer=0;rlPreviewActive=false;$('rlEpisode').textContent=String(episode+1);$('rlStatus').textContent='SIM COMPLETE · SIMULATED REWARD '+reward;setVehicleMotion('stop',false);setFace('stop');}},620);
}

async function request(path, method='POST', data={}){
 if(DEMO_MODE){await new Promise(r=>setTimeout(r,35));
  if(path==='/api/state')return{...demoState};
  if(path==='/api/drive'){if(demoState.estop)throw new Error('emergency_stop_latched');const direction=String(data.direction||'stop');if(!['forward','backward','left','right','stop'].includes(direction))throw new Error('invalid_direction');if(demoState.mode!=='manual'&&direction!=='stop')throw new Error('mode_locked');demoState.command=direction;if(direction==='stop')demoState.mode='manual';return{ok:true}}
  if(path==='/api/speed'){demoState.level=data.level;return{ok:true}}
  if(path==='/api/mode'){if(demoState.estop)throw new Error('emergency_stop_latched');demoState.mode=data.mode;if(data.mode==='auto'){demoState.level='slow';demoState.command='forward'}else demoState.command='stop';return{ok:true}}
  if(path==='/api/servo'){demoState.servoAngle=Number(data.angle)||0;return{ok:true}}
  if(path==='/api/brake'){demoState.estop=true;demoState.command='brake';demoState.mode='manual';return{ok:true}}
  if(path==='/api/resume'){demoState.estop=false;demoState.command='stop';demoState.mode='manual';return{ok:true}}
  return{ok:true}}
 const controller = new AbortController();
 const timeoutId = setTimeout(() => controller.abort(), 1800);
 const opts={method,cache:'no-store',signal:controller.signal,headers:{'Accept':'application/json'}};
 if(method==='POST'){opts.headers['Content-Type']='application/x-www-form-urlencoded;charset=UTF-8';opts.body=new URLSearchParams(data).toString()}
 try {
  const response=await fetch(path,opts);let json={};try{json=await response.json()}catch(_e){}
  if(!response.ok){const error=new Error(json.error||'request_failed');error.status=response.status;throw error}return json
 } catch (error) {
  if(error.name==='AbortError')throw new Error('request_timeout');
  throw error;
 } finally { clearTimeout(timeoutId); }
}

async function flushDriveQueue(){
 if(driveBusy)return;
 driveBusy=true;
 while(pendingDriveDirection!==null){
  const direction=pendingDriveDirection;pendingDriveDirection=null;
  try{await request('/api/drive','POST',{direction});lastDriveRequest=direction;}
  catch(err){
   pendingDriveDirection=null;
   heldDirection=null;
   clearDriveTimer();
   releaseDrivePointer();
   document.querySelectorAll('.drive-btn.pressed').forEach(b=>b.classList.remove('pressed'));
   if(err.message==='emergency_stop_latched')toast('Emergency brake is active. Press Resume first.');
   else if(err.message==='mode_locked')toast('Switch to MANUAL before driving.');
   else toast('Connection timed out. The car will stop via its safety timeout; reconnect to Wi-Fi.');
  }
 }
 driveBusy=false;
}
function queueDrive(direction){
 // A single latest-value queue prevents request pile-ups. Safety stop is retained
 // unless the user actually presses and holds a new direction afterward.
 pendingDriveDirection=direction;flushDriveQueue();
}
function clearDriveTimer(){if(driveTimer){clearInterval(driveTimer);driveTimer=0}}
function releaseDrivePointer(){activeDrivePointerId=null;activeDriveButton=null;}
function stopDrive(sendStop=true){
 const wasDriving=heldDirection!==null&&heldDirection!=='stop';heldDirection=null;clearDriveTimer();releaseDrivePointer();
 document.querySelectorAll('.drive-btn.pressed').forEach(b=>b.classList.remove('pressed'));
 setFace('stop',localEstop);
 if(sendStop&&!localEstop&&(wasDriving||lastDriveRequest!=='stop'||pendingDriveDirection!==null))queueDrive('stop');
}
function startDrive(direction, button, pointerId=null){
 if(localEstop||activeMode!=='manual')return;
 if(direction==='stop'){stopDrive(true);return;}
 heldDirection=direction;activeDrivePointerId=pointerId;activeDriveButton=button;
 document.querySelectorAll('.drive-btn').forEach(b=>b.classList.toggle('pressed',b===button));
 setFace(direction,localEstop);queueDrive(direction);clearDriveTimer();
 // Refresh well inside the firmware's 500 ms hold-to-drive timeout, without flooding the server.
 driveTimer=setInterval(()=>{if(heldDirection===direction)queueDrive(direction)},220);
}
document.querySelectorAll('[data-drive]').forEach(button=>{
 button.addEventListener('pointerdown',event=>{
  event.preventDefault();
  if(button.dataset.drive==='stop'){stopDrive(true);return;}
  if(localEstop||activeMode!=='manual')return;
  // Track the pointer that owns the active direction. An old button's pointerup
  // must never cancel a newer direction selected immediately afterward.
  activeDrivePointerId=event.pointerId;activeDriveButton=button;
  try{button.setPointerCapture(event.pointerId)}catch(_e){}
  startDrive(button.dataset.drive,button,event.pointerId);
 });
 const finishPointer=event=>{
  if(event.pointerId!==activeDrivePointerId||event.currentTarget!==activeDriveButton)return;
  stopDrive(true);
 };
 button.addEventListener('pointerup',finishPointer);
 button.addEventListener('pointercancel',finishPointer);
 button.addEventListener('lostpointercapture',event=>{
  if(event.pointerId===activeDrivePointerId&&event.currentTarget===activeDriveButton)stopDrive(true);
 });
});
window.addEventListener('blur',()=>stopDrive(true));
document.addEventListener('visibilitychange',()=>{if(document.hidden)stopDrive(true)});
function setSpeedVisual(level){activeLevel=level;document.querySelectorAll('.speed-btn').forEach(b=>b.classList.toggle('active',b.dataset.level===level));$('levelValue').textContent=level.toUpperCase();setFace(activeCommand,localEstop)}
document.querySelectorAll('.speed-btn').forEach(btn=>btn.addEventListener('click',async()=>{try{await request('/api/speed','POST',{level:btn.dataset.level});setSpeedVisual(btn.dataset.level);toast('Drive profile: '+btn.dataset.level.toUpperCase())}catch(e){toast('Could not change speed. Authenticate again if needed.')}}));
document.querySelectorAll('[data-mode-choice]').forEach(btn=>btn.addEventListener('click',async()=>{
 const mode=btn.dataset.modeChoice;
 try{await request('/api/mode','POST',{mode});setModeVisual(mode);
  if(mode==='auto'){setSpeedVisual('slow');setFace('forward');runAutoVisual();toast('AUTO demo started: short timed pattern, no obstacle sensor.');}
  else if(mode==='qlearning'){setFace('stop');$('rlStatus').textContent='READY · PRESS RUN POLICY PREVIEW';toast('Q-learning simulation selected; physical motors held stopped.');}
  else{stopDrive(false);setFace('stop');toast('Manual control ready.');}
 }catch(e){toast(e.message==='emergency_stop_latched'?'Release emergency brake first.':'Mode command rejected.');}
}));
$('rlTrainBtn').addEventListener('click',runQLearningPreview);

function setBrakeUI(active){localEstop=active;$('resumeBtn').disabled=!active;$('brakeBtn').textContent=active?'⚠ EMERGENCY BRAKE — ACTIVE':'⚠ EMERGENCY BRAKE';if(active){stopDrive(false);$('commandValue').textContent='BRAKE';$('commandValue').className='tile-value pink';setFace('brake',true)}else if(!rlPreviewActive&&activeMode==='manual'){setFace('stop',false)}}
$('brakeBtn').addEventListener('click',async()=>{stopDrive(false);localEstop=true;try{await request('/api/brake');setBrakeUI(true);toast('Emergency brake latched.')}catch(_e){toast('Brake request failed — check connection.')}});
$('resumeBtn').addEventListener('click',async()=>{try{await request('/api/resume');setBrakeUI(false);stopDrive(false);toast('Brake unlocked. Car remains stopped.')}catch(_e){toast('Resume request failed.')}});
$('stopBtn').addEventListener('click',()=>{stopDrive(true);setModeVisual('manual');request('/api/mode','POST',{mode:'manual'}).catch(()=>{});});$('hornBtn').addEventListener('click',async()=>{try{await request('/api/horn');setFace('dance');setTimeout(()=>setFace('stop'),1100);toast('Horn command sent.')}catch(_e){toast('Horn request failed.')}});
function showServoAngle(angle){angle=Math.max(0,Math.min(180,Math.round(Number(angle)||0)));servoAngle=angle;$('servoValue').textContent=angle;$('telemetryServo').textContent=angle;return angle}
async function flushServoQueue(){
 if(servoBusy)return;
 servoBusy=true;
 try {
  while(pendingServoAngle!==null){
   const waitMs=lastServoSentAt?Math.max(0,SERVO_MIN_SEND_INTERVAL_MS-(Date.now()-lastServoSentAt)):0;
   if(waitMs>0)await new Promise(resolve=>setTimeout(resolve,waitMs));
   // Read after waiting so rapid slider input collapses to the latest requested angle.
   const angle=pendingServoAngle;pendingServoAngle=null;
   if(angle===lastServoSent)continue;
   try{await request('/api/servo','POST',{angle:String(angle)});lastServoSent=angle;lastServoSentAt=Date.now();$('servoStatus').textContent='servo target '+angle+'°';}
   catch(e){$('servoStatus').textContent=e.message==='servo_not_attached'?'Servo not detected':(e.message==='rate_limited'?'Servo command paced for stability':'Command not accepted');}
  }
 } finally {servoBusy=false;}
}
function queueServo(angle,immediate=false){
 angle=showServoAngle(angle);pendingServoAngle=angle;
 if(immediate){if(servoSendTimer){clearTimeout(servoSendTimer);servoSendTimer=0;}flushServoQueue();}
 else if(!servoBusy&&!servoSendTimer){servoSendTimer=setTimeout(()=>{servoSendTimer=0;flushServoQueue()},SERVO_MIN_SEND_INTERVAL_MS);}
}
$('servoSlider').addEventListener('pointerdown',()=>{servoDragging=true});
$('servoSlider').addEventListener('input',()=>queueServo($('servoSlider').value,false));
$('servoSlider').addEventListener('change',()=>{servoDragging=false;queueServo($('servoSlider').value,true)});
$('servoSlider').addEventListener('pointerup',()=>{servoDragging=false;queueServo($('servoSlider').value,true)});
$('servoSlider').addEventListener('pointercancel',()=>{servoDragging=false;queueServo($('servoSlider').value,true)});
document.querySelectorAll('[data-angle]').forEach(b=>b.addEventListener('click',()=>{$('servoSlider').value=b.dataset.angle;servoDragging=false;queueServo(b.dataset.angle,true)}));
const keyDriveMap={ArrowUp:'forward',w:'forward',W:'forward',ArrowDown:'backward',s:'backward',S:'backward',ArrowLeft:'left',a:'left',A:'left',ArrowRight:'right',d:'right',D:'right'};window.addEventListener('keydown',e=>{if(['INPUT','TEXTAREA'].includes(document.activeElement.tagName)||activeMode!=='manual')return;if(e.code==='Space'){e.preventDefault();stopDrive(true);return}const direction=keyDriveMap[e.key];if(direction&&!localEstop){e.preventDefault();if(!e.repeat)startDrive(direction,null)}});window.addEventListener('keyup',e=>{if(keyDriveMap[e.key])stopDrive(true)});
function formatUptime(value){const n=Math.max(0,Math.floor(Number(value)||0));const h=Math.floor(n/3600),m=Math.floor((n%3600)/60),s=n%60;return h?`${String(h).padStart(2,'0')}:${String(m).padStart(2,'0')}:${String(s).padStart(2,'0')}`:`${String(m).padStart(2,'0')}:${String(s).padStart(2,'0')}`}
async function refreshState(){if(stateBusy)return;stateBusy=true;try{const d=await request('/api/state','GET');$('connectionIndicator').classList.remove('off');$('connectionText').textContent=DEMO_MODE?'INTERACTIVE PREVIEW':'CAR ONLINE';$('commandValue').textContent=(d.command||'stop').toUpperCase();$('commandValue').className='tile-value '+(d.estop?'pink':'cyan');if(d.level&&d.level!==activeLevel)setSpeedVisual(d.level);const mode=d.mode||'manual';if(mode!==activeMode)setModeVisual(mode);const heap=Math.max(0,Number(d.heap)||0),kb=Math.round(heap/1024);$('heapValue').textContent=kb+' KB';$('heapHero').textContent=kb+' KB';$('heapFill').style.width=Math.min(100,heap/220000*100)+'%';$('telemetryServo').textContent=d.servoAngle??0;if(!servoDragging&&pendingServoAngle===null&&!servoBusy){$('servoValue').textContent=d.servoAngle??0;$('servoSlider').value=d.servoAngle??0;}if(!servoBusy&&pendingServoAngle===null)lastServoSent=Number(d.servoAngle??0);$('servoStatus').textContent=d.servoAttached?'SERVO READY':'SERVO NOT ATTACHED';setBrakeUI(!!d.estop);if(!rlPreviewActive){setFace(d.command||'stop',!!d.estop);if(mode==='qlearning')setVehicleMotion('stop',!!d.estop)}}catch(e){$('connectionIndicator').classList.add('off');$('connectionText').textContent='LOCAL LINK ISSUE';if(!rlPreviewActive)setFace('error')}finally{stateBusy=false}}
setFace('stop');setModeVisual('manual');
if(DEMO_MODE){$('connectionText').textContent='INTERACTIVE PREVIEW';demoState.uptime=0;refreshState();setInterval(()=>{demoState.uptime++;if(!stateBusy)refreshState()},1500)}else{refreshState();setInterval(refreshState,1500)}
</script>
</body>
</html>
)SPARKHTML";

static void routeIndex() {
  addSecurityHeaders();
  webServer.send_P(200, "text/html; charset=utf-8", PAGE_HTML);
}

#endif // SPARK_CAR_CAPTIVE_PORTAL_H
