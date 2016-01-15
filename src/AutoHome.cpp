#include "AutoHome.h"

Wifi wifi;
OTAUpdate ota;
WiFiClient espClient;
PubSubClient pubclient(espClient);
MQTT mqtt;

char const* p_mqtt_channel;
char const* p_host;
char const* p_mqtt_user;
char const* p_mqtt_password;

AutoHome::AutoHome(){}

AutoHome::~AutoHome(){}

void AutoHome::setPacketHandler(void(*mqttcallback)(char*,uint8_t*,unsigned int)){

	pubclient.setCallback(mqttcallback);

}

void AutoHome::begin(char const* wifi_ssid, char const* wifi_password, char const* mqtt_ip, char const* mqtt_user, char const* mqtt_password, char const* host, char const* mqtt_channel){

	Serial.println("AutoHome Starting");

	wifi.begin(wifi_ssid, wifi_password);

	ota.begin(host);

	pubclient.setServer(mqtt_ip, 1883);

	mqtt.reconnect(pubclient, mqtt_channel, host, mqtt_user, mqtt_password);

	p_mqtt_channel = mqtt_channel;
	p_host = host;
	p_mqtt_user = mqtt_user;
	p_mqtt_password = mqtt_password;	

}

void AutoHome::loop(){

  if (!pubclient.connected()) {
    mqtt.reconnect(pubclient, p_mqtt_channel, p_host, p_mqtt_user, p_mqtt_password);
  }

  pubclient.loop();
  ArduinoOTA.handle();

}
