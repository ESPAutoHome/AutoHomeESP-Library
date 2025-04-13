#ifndef AutoHome_H
#define AutoHome_H

#include <Arduino.h>
#include <FS.h>

#include <AsyncMqttClient.h>

#ifdef ARDUINO_ARCH_ESP32
#include <SPIFFS.h>
#else
#include <LittleFS.h>
#endif
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP_DoubleResetDetector.h>

#include "utility/Wifi.h"
#include "utility/OTAUpdate.h"

#define MQTT_SIGNATURE void (*mqttcallback)(char *, uint8_t *, unsigned int)

class AutoHome
{
private:
	void connectedToWifi();

public:
	AutoHome();
	~AutoHome();
	void setPacketHandler(MQTT_SIGNATURE);
	void begin();
	void loop();
	void sendPacket(char const *message);
	void sendPacket(char const *topic, const char *message);
	void resetSettings();
	void setWatchdogTimeout(int timeout);
	char mqtt_callback(char *topic, byte *payload, unsigned int length);
	String getValue(String data, char separator, int index);
};

#endif
