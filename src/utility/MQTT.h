#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

class MQTT
{

public:
	MQTT();
	~MQTT();

	// Tries to reconnect to the MQTT server
	// Returns true if successfully, otherwise false
	bool MQTT::reconnect(
		PubSubClient pubclient,
		char const *mqtt_channel,
		char const *mqtt_client_name,
		char const *mqtt_user,
		char const *mqtt_password);

	// Reads the value from a MQTT package
	String MQTT::getValue(String data, char separator, int index);

	// A default implementation of the mqtt_callback
	// It will echo the payload and the device information
	char MQTT::mqtt_callback(
		PubSubClient pubclient,
		char *topic,
		byte *payload,
		unsigned int length,
		char const *device_name,
		char const *device_type,
		char const *device_serial,
		char const *mqtt_channel,
		int32_t RSSI);
};

#endif
