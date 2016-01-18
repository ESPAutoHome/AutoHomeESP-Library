#ifndef Wifi_H
#define Wifi_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

class Wifi {

public:
	
	Wifi();
	~Wifi();
	void begin(char const* wifi_ssid, char const* wifi_password);

};

#endif
