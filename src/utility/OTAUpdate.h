#ifndef OTAUpdate_H
#define OTAUpdate_H

#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESPmDNS.h>
#include <WiFi.h>
#else
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#endif

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>

class OTAUpdate
{

public:
	OTAUpdate();
	~OTAUpdate();
	void begin(char const *host);
};

#endif
