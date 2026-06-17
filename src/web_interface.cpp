#include "web_interface.h"
#include <ArduinoJson.h>

void WebInterface::setupRoutes(AsyncWebServer& server, LightingManager& manager) {
  lightManager = &manager;
  webServer = &server;
  
  // Status endpoint
  server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest* request) {
    this->handleGetStatus(request);
  });
  
  // Brightness control
  server.on("/api/brightness", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleSetBrightness(request);
  });
  
  // Mode control
  server.on("/api/mode", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleSetMode(request);
  });
  
  // Speed control
  server.on("/api/speed", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleSetSpeed(request);
  });
  
  // Color control
  server.on("/api/color", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleSetColor(request);
  });
  
  // Turn signals
  server.on("/api/turn", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleTurnSignal(request);
  });
  
  // Brake control
  server.on("/api/brake", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleBrakes(request);
  });
  
  // Parking lights
  server.on("/api/parking", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleParking(request);
  });
  
  // Escort mode
  server.on("/api/escort", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleEscort(request);
  });
  
  // All off
  server.on("/api/all-off", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleAllOff(request);
  });
  
  // All on
  server.on("/api/all-on", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleAllOn(request);
  });
  
  // Web UI
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
    this->serveWebUI(request);
  });
  
  // Serve static files
  server.serveStatic("/", SPIFFS, "/web/");
}

void WebInterface::handleGetStatus(AsyncWebServerRequest* request) {
  DynamicJsonDocument doc(512);
  LightingManager::LightState state = lightManager->getState();
  
  doc["mode"] = state.mode;
  doc["brightness"] = state.brightness;
  doc["speed"] = state.speed;
  doc["color"] = state.color;
  doc["enabled"] = state.enabled;
  doc["left_turn"] = state.left_turn;
  doc["right_turn"] = state.right_turn;
  doc["brake"] = state.brake;
  doc["parking"] = state.parking;
  
  String response;
  serializeJson(doc, response);
  request->send(200, "application/json", response);
}

void WebInterface::handleSetBrightness(AsyncWebServerRequest* request) {
  if (request->hasParam("value")) {
    uint8_t brightness = request->getParam("value")->value().toInt();
    lightManager->setSideStripBrightness(brightness);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing value parameter\"}");
  }
}

void WebInterface::handleSetMode(AsyncWebServerRequest* request) {
  if (request->hasParam("mode")) {
    int mode = request->getParam("mode")->value().toInt();
    lightManager->setSideStripMode((LightingManager::LightMode)mode);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing mode parameter\"}");
  }
}

void WebInterface::handleSetSpeed(AsyncWebServerRequest* request) {
  if (request->hasParam("speed")) {
    uint16_t speed = request->getParam("speed")->value().toInt();
    lightManager->setSideStripSpeed(speed);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing speed parameter\"}");
  }
}

void WebInterface::handleSetColor(AsyncWebServerRequest* request) {
  if (request->hasParam("color")) {
    String colorStr = request->getParam("color")->value();
    uint32_t color = strtol(colorStr.c_str(), NULL, 16);
    lightManager->setSideStripColor(color);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing color parameter\"}");
  }
}

void WebInterface::handleTurnSignal(AsyncWebServerRequest* request) {
  if (request->hasParam("direction") && request->hasParam("active")) {
    String direction = request->getParam("direction")->value();
    bool active = request->getParam("active")->value() == "true";
    
    if (direction == "left") {
      lightManager->setLeftTurn(active);
    } else if (direction == "right") {
      lightManager->setRightTurn(active);
    }
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing parameters\"}");
  }
}

void WebInterface::handleBrakes(AsyncWebServerRequest* request) {
  if (request->hasParam("active")) {
    bool active = request->getParam("active")->value() == "true";
    lightManager->setBrake(active);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing active parameter\"}");
  }
}

void WebInterface::handleParking(AsyncWebServerRequest* request) {
  if (request->hasParam("active")) {
    bool active = request->getParam("active")->value() == "true";
    lightManager->setParking(active);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing active parameter\"}");
  }
}

void WebInterface::handleEscort(AsyncWebServerRequest* request) {
  if (request->hasParam("enabled") && request->hasParam("duration")) {
    bool enabled = request->getParam("enabled")->value() == "true";
    uint8_t duration = request->getParam("duration")->value().toInt();
    lightManager->setEscortMode(enabled, duration);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"Missing parameters\"}");
  }
}

void WebInterface::handleAllOff(AsyncWebServerRequest* request) {
  lightManager->allOff();
  request->send(200, "application/json", "{\"status\":\"all lights off\"}");
}

void WebInterface::handleAllOn(AsyncWebServerRequest* request) {
  uint8_t brightness = 255;
  if (request->hasParam("brightness")) {
    brightness = request->getParam("brightness")->value().toInt();
  }
  lightManager->allOn(brightness);
  request->send(200, "application/json", "{\"status\":\"all lights on\"}");
}

void WebInterface::serveWebUI(AsyncWebServerRequest* request) {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MinakoTitan Lighting Control</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
      min-height: 100vh;
      padding: 20px;
    }
    
    .container {
      max-width: 600px;
      margin: 0 auto;
    }
    
    .header {
      text-align: center;
      color: white;
      margin-bottom: 30px;
    }
    
    .header h1 {
      font-size: 2em;
      margin-bottom: 5px;
    }
    
    .card {
      background: white;
      border-radius: 15px;
      padding: 20px;
      margin-bottom: 20px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.2);
    }
    
    .card-title {
      font-size: 1.3em;
      margin-bottom: 15px;
      color: #333;
      border-bottom: 2px solid #2a5298;
      padding-bottom: 10px;
    }
    
    .control-group {
      margin-bottom: 20px;
    }
    
    .control-label {
      display: block;
      margin-bottom: 8px;
      color: #555;
      font-weight: 500;
    }
    
    input[type="range"] {
      width: 100%;
      height: 6px;
      border-radius: 3px;
      background: #ddd;
      outline: none;
      -webkit-appearance: none;
    }
    
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 20px;
      height: 20px;
      border-radius: 50%;
      background: #2a5298;
      cursor: pointer;
    }
    
    input[type="range"]::-moz-range-thumb {
      width: 20px;
      height: 20px;
      border-radius: 50%;
      background: #2a5298;
      cursor: pointer;
      border: none;
    }
    
    .value-display {
      float: right;
      background: #f0f0f0;
      padding: 5px 10px;
      border-radius: 5px;
      color: #2a5298;
      font-weight: bold;
    }
    
    select {
      width: 100%;
      padding: 10px;
      border: 2px solid #ddd;
      border-radius: 5px;
      font-size: 1em;
      cursor: pointer;
      background-color: white;
    }
    
    select:focus {
      outline: none;
      border-color: #2a5298;
    }
    
    .button-group {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
    }
    
    button {
      padding: 12px 20px;
      border: none;
      border-radius: 8px;
      font-size: 1em;
      cursor: pointer;
      font-weight: 600;
      transition: all 0.3s ease;
    }
    
    .btn-primary {
      background: #2a5298;
      color: white;
    }
    
    .btn-primary:hover {
      background: #1e3c72;
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(42, 82, 152, 0.3);
    }
    
    .btn-danger {
      background: #ff6b6b;
      color: white;
    }
    
    .btn-danger:hover {
      background: #ee5a52;
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(255, 107, 107, 0.3);
    }
    
    .btn-success {
      background: #51cf66;
      color: white;
    }
    
    .btn-success:hover {
      background: #40c057;
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(81, 207, 102, 0.3);
    }
    
    .toggle-group {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
    }
    
    .toggle-btn {
      padding: 12px;
      border: 2px solid #ddd;
      border-radius: 8px;
      background: white;
      cursor: pointer;
      font-weight: 600;
      transition: all 0.3s ease;
    }
    
    .toggle-btn.active {
      background: #2a5298;
      color: white;
      border-color: #2a5298;
    }
    
    .color-picker-wrapper {
      display: flex;
      gap: 10px;
      flex-wrap: wrap;
    }
    
    .color-preset {
      width: 40px;
      height: 40px;
      border-radius: 50%;
      cursor: pointer;
      border: 3px solid #ddd;
      transition: transform 0.2s;
    }
    
    .color-preset:hover {
      transform: scale(1.1);
    }
    
    .status-text {
      text-align: center;
      margin-top: 15px;
      padding: 10px;
      background: #f0f0f0;
      border-radius: 5px;
      color: #555;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>🚲 MinakoTitan</h1>
      <p>Lighting Control System</p>
    </div>
    
    <div class="card">
      <div class="card-title">🌈 Side Strip Effects</div>
      
      <div class="control-group">
        <label class="control-label">Mode</label>
        <select id="modeSelect" onchange="changeMode()">
          <option value="0">OFF</option>
          <option value="1">Static</option>
          <option value="2">Blink</option>
          <option value="3">Pulse</option>
          <option value="4">Chase</option>
          <option value="5">Music Reactive</option>
          <option value="6">Rainbow</option>
          <option value="7">Strobe</option>
        </select>
      </div>
      
      <div class="control-group">
        <label class="control-label">Brightness <span class="value-display" id="brightnessValue">255</span></label>
        <input type="range" id="brightness" min="0" max="255" value="255" oninput="changeBrightness()">
      </div>
      
      <div class="control-group">
        <label class="control-label">Speed <span class="value-display" id="speedValue">1000ms</span></label>
        <input type="range" id="speed" min="100" max="3000" value="1000" step="100" oninput="changeSpeed()">
      </div>
      
      <div class="control-group">
        <label class="control-label">Color Presets</label>
        <div class="color-picker-wrapper">
          <div class="color-preset" style="background: #FF0000;" onclick="selectColor('FF0000')"></div>
          <div class="color-preset" style="background: #00FF00;" onclick="selectColor('00FF00')"></div>
          <div class="color-preset" style="background: #0000FF;" onclick="selectColor('0000FF')"></div>
          <div class="color-preset" style="background: #FFFF00;" onclick="selectColor('FFFF00')"></div>
          <div class="color-preset" style="background: #FF00FF;" onclick="selectColor('FF00FF')"></div>
          <div class="color-preset" style="background: #00FFFF;" onclick="selectColor('00FFFF')"></div>
          <div class="color-preset" style="background: #FFFFFF;" onclick="selectColor('FFFFFF')"></div>
        </div>
      </div>
    </div>
    
    <div class="card">
      <div class="card-title">💡 Front Lights</div>
      <div class="button-group" style="grid-template-columns: 1fr;">
        <button class="btn-primary" onclick="setHeadlight(255)">Headlights ON</button>
        <button class="btn-danger" onclick="setHeadlight(0)">Headlights OFF</button>
      </div>
    </div>
    
    <div class="card">
      <div class="card-title">🔄 Turn Signals</div>
      <div class="button-group">
        <button class="toggle-btn" id="leftTurnBtn" onclick="toggleLeftTurn()">← LEFT</button>
        <button class="toggle-btn" id="rightTurnBtn" onclick="toggleRightTurn()">RIGHT →</button>
      </div>
    </div>
    
    <div class="card">
      <div class="card-title">🛑 Rear Lights</div>
      <div class="button-group">
        <button class="toggle-btn" id="brakeBtn" onclick="toggleBrake()">BRAKE</button>
        <button class="toggle-btn" id="parkingBtn" onclick="toggleParking()">PARKING</button>
      </div>
    </div>
    
    <div class="card">
      <div class="card-title">👁️ Escort Mode</div>
      <div class="control-group">
        <label class="control-label">Duration: <span id="escortDuration">120</span>s</label>
        <input type="range" id="escortTime" min="30" max="300" value="120" step="10" oninput="updateEscortDuration()">
      </div>
      <button class="btn-success" onclick="startEscort()" style="width: 100%;">Enable Escort Mode</button>
    </div>
    
    <div class="card">
      <div class="card-title">⚙️ Master Controls</div>
      <div class="button-group">
        <button class="btn-success" onclick="allOn()">ALL ON</button>
        <button class="btn-danger" onclick="allOff()">ALL OFF</button>
      </div>
    </div>
  </div>
  
  <script>
    let leftTurnActive = false;
    let rightTurnActive = false;
    let brakeActive = false;
    let parkingActive = false;
    
    async function apiCall(endpoint, method = 'GET', params = {}) {
      try {
        let url = endpoint;
        if (method === 'GET' && Object.keys(params).length > 0) {
          url += '?' + new URLSearchParams(params).toString();
        }
        
        const options = {
          method: method,
          headers: { 'Content-Type': 'application/json' }
        };
        
        if (method === 'POST') {
          url += '?' + new URLSearchParams(params).toString();
        }
        
        const response = await fetch(url, options);
        return await response.json();
      } catch (error) {
        console.error('API Error:', error);
      }
    }
    
    function changeMode() {
      const mode = document.getElementById('modeSelect').value;
      apiCall('/api/mode', 'POST', { mode: mode });
    }
    
    function changeBrightness() {
      const brightness = document.getElementById('brightness').value;
      document.getElementById('brightnessValue').textContent = brightness;
      apiCall('/api/brightness', 'POST', { value: brightness });
    }
    
    function changeSpeed() {
      const speed = document.getElementById('speed').value;
      document.getElementById('speedValue').textContent = speed + 'ms';
      apiCall('/api/speed', 'POST', { speed: speed });
    }
    
    function selectColor(color) {
      apiCall('/api/color', 'POST', { color: color });
    }
    
    function setHeadlight(value) {
      apiCall('/api/brightness', 'POST', { value: value });
    }
    
    function toggleLeftTurn() {
      leftTurnActive = !leftTurnActive;
      const btn = document.getElementById('leftTurnBtn');
      btn.classList.toggle('active', leftTurnActive);
      apiCall('/api/turn', 'POST', { direction: 'left', active: leftTurnActive });
    }
    
    function toggleRightTurn() {
      rightTurnActive = !rightTurnActive;
      const btn = document.getElementById('rightTurnBtn');
      btn.classList.toggle('active', rightTurnActive);
      apiCall('/api/turn', 'POST', { direction: 'right', active: rightTurnActive });
    }
    
    function toggleBrake() {
      brakeActive = !brakeActive;
      const btn = document.getElementById('brakeBtn');
      btn.classList.toggle('active', brakeActive);
      apiCall('/api/brake', 'POST', { active: brakeActive });
    }
    
    function toggleParking() {
      parkingActive = !parkingActive;
      const btn = document.getElementById('parkingBtn');
      btn.classList.toggle('active', parkingActive);
      apiCall('/api/parking', 'POST', { active: parkingActive });
    }
    
    function updateEscortDuration() {
      const duration = document.getElementById('escortTime').value;
      document.getElementById('escortDuration').textContent = duration;
    }
    
    function startEscort() {
      const duration = document.getElementById('escortTime').value;
      apiCall('/api/escort', 'POST', { enabled: true, duration: duration });
    }
    
    function allOn() {
      apiCall('/api/all-on', 'POST', { brightness: 255 });
    }
    
    function allOff() {
      apiCall('/api/all-off', 'POST', {});
    }
  </script>
</body>
</html>
  )";
  
  request->send(200, "text/html", html);
}