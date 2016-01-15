#include "MQTT.h"

MQTT::MQTT(){}

MQTT::~MQTT(){}

void MQTT::reconnect(PubSubClient pubclient, char const* mqtt_channel, char const* mqtt_client, char const* mqtt_user, char const* mqtt_password){
    
  // Loop until we're reconnected
  while (!pubclient.connected()) {

    Serial.print("Attempting MQTT connection...");

    /* Connect to diffrent chennels */

    if (pubclient.connect(mqtt_client, mqtt_user, mqtt_password)) {
      Serial.println("connected");

      pubclient.subscribe(mqtt_channel);

    } else {
      Serial.print("failed, rc=");
      Serial.print(pubclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }

  }


}
