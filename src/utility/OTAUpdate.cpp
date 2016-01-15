#include "OTAUpdate.h"

OTAUpdate::OTAUpdate(){}

OTAUpdate::~OTAUpdate(){}

void OTAUpdate::begin(char const* host){

  ArduinoOTA.setHostname(host);

  ArduinoOTA.onStart([]() {
    
    Serial.println("OTA Update Starting");
    
  });
  
  ArduinoOTA.onEnd([]() {
    
    Serial.println("OTA Update Finished");
    
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    
    Serial.printf("OTA Update Progress: %u%%\n", (progress / (total / 100)));
    
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();

}
