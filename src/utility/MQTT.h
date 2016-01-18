#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

class MQTT {

public:
	MQTT();
	~MQTT();
	void reconnect(PubSubClient pubclient, char const* mqtt_channel, char const* mqtt_client, char const* mqtt_user, char const* mqtt_password);
};

#endif
