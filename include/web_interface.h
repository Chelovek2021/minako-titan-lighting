#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <AsyncWebServer.h>
#include "lighting_manager.h"

class WebInterface {
public:
  void setupRoutes(AsyncWebServer& server, LightingManager& manager);

private:
  LightingManager* lightManager;
  AsyncWebServer* webServer;
  
  // Route handlers
  void handleGetStatus(AsyncWebServerRequest* request);
  void handleSetBrightness(AsyncWebServerRequest* request);
  void handleSetMode(AsyncWebServerRequest* request);
  void handleSetSpeed(AsyncWebServerRequest* request);
  void handleSetColor(AsyncWebServerRequest* request);
  void handleTurnSignal(AsyncWebServerRequest* request);
  void handleBrakes(AsyncWebServerRequest* request);
  void handleParking(AsyncWebServerRequest* request);
  void handleEscort(AsyncWebServerRequest* request);
  void handleAllOff(AsyncWebServerRequest* request);
  void handleAllOn(AsyncWebServerRequest* request);
  void serveWebUI(AsyncWebServerRequest* request);
};

#endif // WEB_INTERFACE_H