#ifndef Wifi_H
#define Wifi_H

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WebServer.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#endif

#include <DNSServer.h>	 //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

class Wifi
{

public:
	Wifi();
	~Wifi();
	void begin(char const *wifi_ssid, char const *wifi_password);
	void begin();
};

#endif
