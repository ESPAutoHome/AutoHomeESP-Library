#ifndef AutoHome_H
#define AutoHome_H

#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include "utility/Wifi.h"
#include "utility/OTAUpdate.h"
#include "utility/MQTT.h"

#define MQTT_SIGNATURE void (*mqttcallback)(char*,uint8_t*,unsigned int)

class AutoHome {
private:
	MQTT_CALLBACK_SIGNATURE;

public:
	AutoHome();
	~AutoHome();
	void setPacketHandler(MQTT_SIGNATURE);
	void begin(char const* wifi_ssid, char const* wifi_password, char const* mqtt_ip, char const* mqtt_user, char const* mqtt_password, char const* host, char const* mqtt_channel);
	void loop();
};

#endif
