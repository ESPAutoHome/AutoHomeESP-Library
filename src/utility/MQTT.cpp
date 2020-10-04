#include "MQTT.h"

MQTT::MQTT() {}

MQTT::~MQTT() {}

bool MQTT::reconnect(
    PubSubClient pubclient,
    char const *mqtt_channel,
    char const *mqtt_client_name,
    char const *mqtt_user,
    char const *mqtt_password)
{
  if (!pubclient.connected())
  {
    Serial.println("Attempting MQTT connection...");
    Serial.println("mqtt_channel: " + String(mqtt_channel));
    Serial.println("mqtt_client_name: " + String(mqtt_client_name));
    Serial.println("mqtt_user: " + String(mqtt_user));
    Serial.println("mqtt_password: " + String(mqtt_password));

    /* Connect to diffrent chennels */
    if (pubclient.connect(mqtt_client_name, mqtt_user, mqtt_password))
    {
      Serial.println("Connected successfully to MQTT");
      String message = "hello world from " + String(mqtt_client_name);
      pubclient.publish("outTopic", message.c_str());

      pubclient.subscribe(mqtt_channel);
      pubclient.subscribe("/autohome");
      return true;
    }
    else
    {
      Serial.print("MQTT connection failed, rc=");
      Serial.println(pubclient.state());
      return false;
    }
  }
}

String MQTT::getValue(String data, char separator, int index)
{

  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

char MQTT::mqtt_callback(
    PubSubClient pubclient,
    char *topic,
    byte *payload,
    unsigned int length,
    char const *device_name,
    char const *device_type,
    char const *device_serial,
    char const *mqtt_channel,
    int32_t RSSI)
{
  String packet = "";
  for (int i = 0; i < length; i++)
  {
    packet = packet + (char)payload[i];
  }

  char autohomeTopic[] = "/autohome";
  if (strcmp(topic, autohomeTopic) == 0)
  {
    Serial.println("Autohome Packet");
    if (packet.equals("SCAN"))
    {
      Serial.println("Autohome Scan Packet");
      String responce = "SCANRET:" +
                        String(device_name) + ":" +
                        String(device_type) + ":" +
                        String(device_serial) + ":" +
                        String(mqtt_channel) + ":" +
                        String(RSSI);
      pubclient.publish("/autohome", responce.c_str());
    }
    else if (getValue(packet, ':', 0).equals("INFO"))
    {
      if (String(device_name).equals(getValue(packet, ':', 1)))
      {
        String responce = "INFORES:" + String(device_name) + ":" +
                          String(device_type) + ":" +
                          String(device_serial) + ":" +
                          String(mqtt_channel) + ":" +
                          String(RSSI);
        pubclient.publish("/autohome", responce.c_str());
      }
    }
    return 1;
  }
  return 0;
}
