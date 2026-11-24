#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <esp32-hal-rmt.h>
#include <esp_system.h>
#include <optional>

#include "ac_protocol.h"
#include "app_icon.h"
#include "panel.h"

namespace {
constexpr uint8_t kLedPin = 8;
constexpr uint8_t kIrPin = 4;
constexpr char kSsid[] = "AIHome";
constexpr char kPassword[] = "12345678";
const IPAddress kApAddress(192, 168, 4, 1);
const IPAddress kApMask(255, 255, 255, 0);
constexpr uint32_t kApCheckIntervalMs = 2500;
constexpr const char *kModeNames[] = {"自动", "制冷", "除湿", "送风", "制热"};

enum class Key {
    Power, Warmer, Mode, Economy, Fan, Cooler, Cool, Heat, Vertical, Horizontal,
    Quiet, Turbo, Timer, Light, DryAux, Sleep, Auto, Dry, FanMode
};

constexpr const char *kKeyNames[] = {
    "power", "warmer", "mode", "economy", "fan", "cooler", "cool", "heat",
    "vertical", "horizontal", "quiet", "turbo", "timer", "light", "dryAux", "sleep", "auto", "dry", "fanMode"
};

WebServer server(80);
struct Room {
    ac::State requested;
    std::optional<ac::State> queued;
    bool transmitFailed = false;
    bool sent = false;
};
Room living;
Room bedroom;
Room *transmittingRoom = nullptr;
bool irReady = false;
rmt_data_t irSymbols[(ac::kMaxPulseCount + 1) / 2];
bool apReady = false;
bool ledOn = false;
uint32_t lastApCheck = 0;
uint32_t lastAttempt = 0, connectedSince = 0;
bool wasConnected = false, reconnectPending = false;
Preferences preferences;
String homeSsid, homePassword;
String serialConfig;
bool readingConfig = false;

bool saveNetwork(const String &ssid, const String &password) {
    if (ssid.isEmpty() || ssid.length() > 32 || password.length() < 8 || password.length() > 63) return false;
    JsonDocument doc;
    doc["ssid"] = ssid;
    doc["password"] = password;
    String value;
    serializeJson(doc, value);
    if (preferences.putString("network", value) != value.length()) return false;
    homeSsid = ssid;
    homePassword = password;
    reconnectPending = true;
    return true;
}

void sendJson(int code, const char *body) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(code, "application/json", body);
}

void sendError(int code, const char *reason) {
    JsonDocument doc;
    doc["error"] = reason;
    String body;
    serializeJson(doc, body);
    sendJson(code, body.c_str());
}

void sendAcState(const Room &room) {
    JsonDocument doc;
    const ac::State &visible = room.queued ? *room.queued : room.requested;
    doc["ready"] = irReady;
    doc["sent"] = room.sent;
    doc["pending"] = static_cast<unsigned>(transmittingRoom == &room) + static_cast<unsigned>(room.queued.has_value());
    doc["failed"] = room.transmitFailed;
    doc["power"] = visible.power;
    doc["mode"] = kModeNames[static_cast<size_t>(visible.mode)];
    doc["temperature"] = ac::effectiveTemperature(visible);
    doc["fan"] = ac::effectiveFan(visible);
    doc["economy"] = visible.economy;
    doc["vertical"] = visible.verticalSwing;
    doc["horizontal"] = visible.horizontalSwing;
    doc["quiet"] = visible.quiet;
    doc["turbo"] = visible.turbo;
    doc["timer"] = visible.timer;
    doc["light"] = visible.light;
    doc["dryAux"] = visible.dry || visible.auxiliaryHeat;
    doc["sleep"] = visible.sleep;
    String body;
    serializeJson(doc, body);
    sendJson(200, body.c_str());
}

bool writeIr(const ac::State &next) {
    if (!irReady || !rmtTransmitCompleted(kIrPin)) return false;
    const ac::Packet packet = ac::encode(next);
    ac::Timings timings;
    const size_t pulseCount = ac::makeTimings(packet, timings);
    const size_t symbolCount = (pulseCount + 1) / 2;
    for (size_t i = 0; i < symbolCount; ++i) {
        const size_t index = 2 * i;
        // At 500 kHz each tick is 2 µs, so the 40 ms inter-section gap fits one symbol field.
        irSymbols[i].duration0 = (timings[index] + 1) / 2;
        irSymbols[i].level0 = 1;
        irSymbols[i].duration1 = index + 1 < pulseCount ? (timings[index + 1] + 1) / 2 : 500;
        irSymbols[i].level1 = 0;
    }
    return rmtWriteAsync(kIrPin, irSymbols, symbolCount);
}

bool startTransmission(Room &room, const ac::State &next) {
    if (!writeIr(next)) return false;
    room.requested = next;
    transmittingRoom = &room;
    return true;
}

void serviceTransmission() {
    Room *completed = transmittingRoom;
    if (completed) {
        if (!rmtTransmitCompleted(kIrPin)) return;
        completed->sent = true;
        transmittingRoom = nullptr;
    }

    // If both rooms have commands queued, let the other room go next.
    Room *first = completed == &living ? &bedroom : &living;
    Room *second = first == &living ? &bedroom : &living;
    for (Room *room : {first, second}) {
        if (!room->queued) continue;
        const ac::State next = *room->queued;
        room->queued.reset();
        if (startTransmission(*room, next)) return;
        room->transmitFailed = true;
    }
}

void setMode(ac::State &next, ac::Mode mode) {
    next.mode = mode;
    next.economy = false;
    next.quiet = false;
    next.turbo = false;
    next.sleep = false;
    next.dry = false;
    next.auxiliaryHeat = false;
}

const char *applyKey(Key key, ac::State &next) {
    constexpr struct { Key key; ac::Mode mode; } modeKeys[] = {
        {Key::Auto, ac::Mode::Auto}, {Key::Cool, ac::Mode::Cool}, {Key::Dry, ac::Mode::Dry},
        {Key::FanMode, ac::Mode::Fan}, {Key::Heat, ac::Mode::Heat}
    };
    for (const auto &shortcut : modeKeys) {
        if (key == shortcut.key) {
            setMode(next, shortcut.mode);
            next.power = true;
            return nullptr;
        }
    }
    if (!next.power && key != Key::Power) {
        return "power_off";
    }
    switch (key) {
        case Key::Power:
            next.power = !next.power;
            if (!next.power) next.timer = false;
            break;
        case Key::Warmer:
        case Key::Cooler:
            if (next.economy || next.mode == ac::Mode::Auto || next.mode == ac::Mode::Dry) {
                return "temperature_locked";
            }
            if (key == Key::Warmer && next.temperature < ac::kMaximumTemperature) ++next.temperature;
            if (key == Key::Cooler && next.temperature > ac::kMinimumTemperature) --next.temperature;
            break;
        case Key::Mode:
            setMode(next, static_cast<ac::Mode>((static_cast<unsigned>(next.mode) + 1) % 5));
            break;
        case Key::Economy:
            if (next.mode != ac::Mode::Cool) return "economy_cooling_only";
            next.economy = !next.economy;
            if (next.economy) {
                next.quiet = false;
                next.turbo = false;
                next.sleep = false;
            }
            break;
        case Key::Fan:
            if (next.economy) return "fan_locked";
            next.fan = (next.fan + 1) % (ac::kMaximumFan + 1);
            break;
        case Key::Auto:
        case Key::Cool:
        case Key::Dry:
        case Key::FanMode:
        case Key::Heat:
            break;
        case Key::Vertical:
            next.verticalSwing = !next.verticalSwing;
            next.swingAuto = next.verticalSwing || next.horizontalSwing;
            break;
        case Key::Horizontal:
            next.horizontalSwing = !next.horizontalSwing;
            next.swingAuto = next.verticalSwing || next.horizontalSwing;
            break;
        case Key::Quiet:
            if (next.economy) return "fan_locked";
            next.quiet = !next.quiet;
            if (next.quiet) next.turbo = false;
            break;
        case Key::Turbo:
            if (next.economy) return "fan_locked";
            next.turbo = !next.turbo;
            if (next.turbo) {
                next.quiet = false;
                next.sleep = false;
            }
            break;
        case Key::Timer:
            next.timer = !next.timer;
            break;
        case Key::Light:
            next.light = !next.light;
            break;
        case Key::DryAux:
            if (next.mode == ac::Mode::Heat) next.auxiliaryHeat = !next.auxiliaryHeat;
            else if (next.mode == ac::Mode::Cool || next.mode == ac::Mode::Dry) next.dry = !next.dry;
            else return "dry_aux_mode";
            break;
        case Key::Sleep:
            if (next.economy) return "fan_locked";
            next.sleep = !next.sleep;
            if (next.sleep) next.turbo = false;
            break;
    }
    return nullptr;
}

void handleAcKey(Room &room) {
    if (server.header("Content-Type") != "application/json") {
        sendError(415, "expected_json");
        return;
    }
    const String body = server.arg("plain");
    JsonDocument doc;
    if (body.length() > 48 || deserializeJson(doc, body) || !doc["key"].is<const char *>()) {
        sendError(400, "unknown_key");
        return;
    }
    const JsonString name = doc["key"].as<JsonString>();
    size_t index = 0;
    while (index < sizeof(kKeyNames) / sizeof(kKeyNames[0])) {
        if (name.size() == strlen(kKeyNames[index]) &&
            memcmp(name.c_str(), kKeyNames[index], name.size()) == 0) break;
        ++index;
    }
    if (index == sizeof(kKeyNames) / sizeof(kKeyNames[0])) {
        sendError(400, "unknown_key");
        return;
    }

    if (room.queued) {
        sendError(429, "transmit_busy");
        return;
    }
    ac::State next = room.requested;
    if (const char *error = applyKey(static_cast<Key>(index), next)) {
        sendError(409, error);
        return;
    }
    if (!irReady) {
        sendError(503, "transmit_failed");
        return;
    }
    if (transmittingRoom) {
        room.queued = next;
    } else if (!startTransmission(room, next)) {
        sendError(503, "transmit_failed");
        return;
    }
    room.transmitFailed = false;
    sendAcState(room);
}

void sendLedState() {
    sendJson(200, ledOn ? "{\"on\":true}" : "{\"on\":false}");
}

void handleLedCommand() {
    if (server.header("Content-Type") != "application/json") {
        sendError(415, "expected_json");
        return;
    }
    const String body = server.arg("plain");
    if (body != "true" && body != "false") {
        sendError(400, "expected_boolean");
        return;
    }
    ledOn = body == "true";
    digitalWrite(kLedPin, ledOn ? LOW : HIGH);
    sendLedState();
}

bool apIsRunning() {
    return WiFi.AP.started() && WiFi.softAPIP() == kApAddress;
}

void startAccessPoint() {
    WiFi.mode(WIFI_AP_STA);
    apReady = WiFi.softAPConfig(kApAddress, kApAddress, kApMask) && WiFi.softAP(kSsid, kPassword);
}

void beginHomeConnection() {
    WiFi.disconnect(false, false);
    if (!homeSsid.isEmpty()) WiFi.begin(homeSsid.c_str(), homePassword.c_str());
    lastAttempt = millis();
    reconnectPending = false;
}

void serviceAccessPoint() {
    const uint32_t now = millis();
    if (reconnectPending) beginHomeConnection();
    const bool connected = WiFi.status() == WL_CONNECTED;
    if (connected != wasConnected) {
        wasConnected = connected;
        server.stop();
        server.begin();
        MDNS.end();
        if (connected) {
            connectedSince = now;
            if (MDNS.begin("aihome")) MDNS.addService("http", "tcp", 80);
            Serial.printf("HOME_CONNECTED http://%s/ | http://aihome.local/\n", WiFi.localIP().toString().c_str());
        } else {
            lastAttempt = now;
        }
    }
    // Give an already-connected setup phone time to read the new address.
    if (connected && apReady && now - connectedSince > 60000 && WiFi.softAPgetStationNum() == 0) {
        WiFi.softAPdisconnect(true);
        apReady = false;
    }
    if (now - lastApCheck < kApCheckIntervalMs) return;
    lastApCheck = now;
    if (!connected && (homeSsid.isEmpty() || now - lastAttempt >= 30000)) {
        if (!apIsRunning()) startAccessPoint();
        if (!homeSsid.isEmpty()) {
            WiFi.reconnect();
            lastAttempt = now;
        }
    }
}

void sendNetworkState() {
    JsonDocument doc;
    doc["connected"] = WiFi.status() == WL_CONNECTED;
    doc["ssid"] = homeSsid;
    doc["ip"] = WiFi.localIP().toString();
    String body;
    serializeJson(doc, body);
    sendJson(200, body.c_str());
}

void configureNetwork() {
    // Credentials can only be changed from the local setup hotspot.
    if (!apReady || server.client().localIP() != kApAddress) {
        sendError(403, "connect_to_AIHome_to_configure");
        return;
    }
    if (!saveNetwork(server.arg("ssid"), server.arg("password"))) {
        sendError(400, "invalid_network_or_storage_error");
        return;
    }
    server.sendHeader("Location", "/wifi");
    server.send(303);
}

constexpr char kAppManifest[] PROGMEM = R"JSON({
    "id": "/",
    "name": "李工的 AIHome",
    "short_name": "AIHome",
    "lang": "zh-CN",
    "start_url": "/",
    "scope": "/",
    "display": "standalone",
    "background_color": "#eefaff",
    "theme_color": "#d7f4ff",
    "icons": [{"src": "/apple-touch-icon.png", "sizes": "180x180", "type": "image/png"}]
})JSON";

constexpr char kWifiPage[] PROGMEM = R"HTML(<!doctype html><html lang="zh-CN"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>AIHome Wi-Fi</title><style>body{font:18px system-ui;max-width:480px;margin:40px auto;padding:20px}input,button{box-sizing:border-box;width:100%;padding:14px;margin:8px 0}a{display:block;margin:20px 0}</style><h1>家庭 Wi-Fi 设置</h1><p>连接 AIHome 热点后填写家里的 2.4GHz Wi-Fi。保存后自动连接，密码断电保留。</p><form method="post" action="/wifi"><label>Wi-Fi 名称<input name="ssid" maxlength="32" required></label><label>密码<input name="password" type="password" minlength="8" maxlength="63" required></label><button>保存并连接</button></form><p id="status">正在查询…</p><p>连接成功后，让手机连回家里的 Wi-Fi，再打开 <a href="http://aihome.local/">http://aihome.local/</a></p><a href="/">返回控制面板</a><script>async function poll(){try{const r=await fetch('/api/network',{cache:'no-store'});const n=await r.json();document.getElementById('status').textContent=n.connected?'已连接 '+n.ssid+'，控制地址 http://'+n.ip+'/':'尚未连接 '+n.ssid+'。请确认名称、密码及 2.4GHz 已开启。'}catch{}setTimeout(poll,3000)}poll()</script></html>)HTML";

void printStatus() {
    Serial.printf("Home: %s | SSID: %s | IP: %s | status=%d\n", WiFi.status() == WL_CONNECTED ? "connected" : "disconnected", homeSsid.c_str(), WiFi.localIP().toString().c_str(), static_cast<int>(WiFi.status()));
    if (apReady) {
        Serial.printf("Wi-Fi: %s\nPassword: %s\nPanel: http://%s/\n",
                      kSsid, kPassword, WiFi.softAPIP().toString().c_str());
    }
    Serial.printf("AP: %s | clients=%u | uptime=%lu ms | reset=%d\n",
                  apReady && apIsRunning() ? "ready" : WiFi.status() == WL_CONNECTED ? "off (home connected)" : "recovering",
                  static_cast<unsigned>(WiFi.softAPgetStationNum()),
                  static_cast<unsigned long>(millis()), static_cast<int>(esp_reset_reason()));
    Serial.printf("IR TX: shared GPIO%u %s\n", kIrPin, irReady ? "ready" : "error");
}
}  // namespace

void setup() {
    Serial.begin(115200);
    pinMode(kLedPin, OUTPUT);
    digitalWrite(kLedPin, HIGH);
    constexpr bool kCarrierActiveLow = false;
    irReady = rmtInit(kIrPin, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_2, 500000) &&
              rmtSetCarrier(kIrPin, true, kCarrierActiveLow, 38000, 0.33f) &&
              rmtSetEOT(kIrPin, LOW);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("aihome");
    WiFi.setAutoReconnect(true);
    // USB-powered remote: prioritize command latency over modem power saving.
    WiFi.setSleep(false);
    preferences.begin("aihome", false);
    JsonDocument saved;
    if (!deserializeJson(saved, preferences.getString("network", "{}"))) {
        homeSsid = String(saved["ssid"] | "");
        homePassword = String(saved["password"] | "");
    }
    const char *headers[] = {"Content-Type"};
    server.collectHeaders(headers, 1);
    const auto sendPanel = [] {
        server.sendHeader("Cache-Control", "no-store");
        server.sendHeader("Content-Security-Policy", "default-src 'none'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'; manifest-src 'self'; img-src 'self'; frame-ancestors 'none'; base-uri 'none'");
        server.send_P(200, "text/html; charset=utf-8", kPanel);
    };
    server.on("/", HTTP_GET, sendPanel);
    server.on("/bedroom", HTTP_GET, sendPanel);
    server.on("/manifest.webmanifest", HTTP_GET, [] {
        server.send_P(200, "application/manifest+json", kAppManifest);
    });
    server.on("/apple-touch-icon.png", HTTP_GET, [] {
        server.send_P(200, "image/png", reinterpret_cast<PGM_P>(kAppIcon), sizeof(kAppIcon));
    });
    server.on("/api/ac", HTTP_GET, [] { sendAcState(living); });
    server.on("/api/ac", HTTP_POST, [] { handleAcKey(living); });
    server.on("/api/bedroom/ac", HTTP_GET, [] { sendAcState(bedroom); });
    server.on("/api/bedroom/ac", HTTP_POST, [] { handleAcKey(bedroom); });
    server.on("/api/led", HTTP_GET, sendLedState);
    server.on("/api/led", HTTP_POST, handleLedCommand);
    server.on("/api/network", HTTP_GET, sendNetworkState);
    server.on("/wifi", HTTP_GET, [] {
        server.sendHeader("Cache-Control", "no-store");
        server.send_P(200, "text/html; charset=utf-8", kWifiPage);
    });
    server.on("/wifi", HTTP_POST, configureNetwork);
    server.onNotFound([] { sendError(404, "not_found"); });
    if (homeSsid.isEmpty()) startAccessPoint();
    beginHomeConnection();
    server.begin();
    lastApCheck = millis();
}

void loop() {
    serviceTransmission();
    serviceAccessPoint();
    server.handleClient();
    const int command = Serial.read();
    if (readingConfig && command >= 0) {
        if (command == '\n') {
            JsonDocument doc;
            const bool ok = !deserializeJson(doc, serialConfig) && saveNetwork(String(doc["ssid"] | ""), String(doc["password"] | ""));
            Serial.println(ok ? "CONFIG_SAVED" : "CONFIG_ERROR");
            serialConfig = "";
            readingConfig = false;
        } else if (serialConfig.length() < 256) serialConfig += static_cast<char>(command);
        else { readingConfig = false; serialConfig = ""; }
        delay(1);
        return;
    }
    if (command == 'c') { readingConfig = true; serialConfig = ""; }
    if (command == '?') printStatus();
    if (command == 's' && !transmittingRoom) {
        const int count = WiFi.scanNetworks();
        Serial.printf("SCAN %d\n", count);
        for (int i = 0; i < count; ++i) Serial.printf("SSID %s | channel %d | RSSI %d\n", WiFi.SSID(i).c_str(), static_cast<int>(WiFi.channel(i)), static_cast<int>(WiFi.RSSI(i)));
        WiFi.scanDelete();
    }
    if (command == 't') {
        if (transmittingRoom || !writeIr(ac::State{})) {
            Serial.println("IR_TEST busy or failed");
        } else {
            const uint32_t start = millis();
            while (!rmtTransmitCompleted(kIrPin) && millis() - start < 1000) delay(2);
            Serial.printf("IR_TEST %s (%lu ms)\n",
                          rmtTransmitCompleted(kIrPin) ? "ok (power off)" : "timed out",
                          static_cast<unsigned long>(millis() - start));
        }
    }
    delay(3);
}
