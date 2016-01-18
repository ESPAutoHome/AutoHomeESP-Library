#ifndef OTAUpdate_H
#define OTAUpdate_H

#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

class OTAUpdate {

public:
	OTAUpdate();
	~OTAUpdate();
	void begin(char const* host);
};

#endif
