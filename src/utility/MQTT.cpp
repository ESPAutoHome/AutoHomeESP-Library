#include "MQTT.h"

MQTT::MQTT() {}

MQTT::~MQTT() {}

void MQTT::reconnect(PubSubClient pubclient, char const *mqtt_channel, char const *mqtt_client, char const *mqtt_user, char const *mqtt_password)
{

  if (!pubclient.connected())
  {
    Serial.println("mqtt_channel: " + String(mqtt_channel));
    Serial.println("mqtt_client: " + String(mqtt_client));
    Serial.println("mqtt_user: " + String(mqtt_user));
    Serial.println("mqtt_password: " + String(mqtt_password));
    Serial.print("Attempting MQTT connection...");

    /* Connect to diffrent chennels */
    if (pubclient.connect(mqtt_client, mqtt_user, mqtt_password))
    {
      Serial.println("connected");
      pubclient.publish("outTopic", "hello world");

      pubclient.subscribe(mqtt_channel);
      pubclient.subscribe("/autohome");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(pubclient.state());
      Serial.println(" try again in 5 seconds"); // ESP.restart();
      Serial.println(" lost connection, resetting");
      delay(5000);
      ESP.restart();
    }
  }
}
