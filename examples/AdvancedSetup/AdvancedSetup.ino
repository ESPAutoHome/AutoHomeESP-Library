/* This example is an advanced setup example to show you how to setup the AutoHome library. */
/* If you run this example with the variables filled out the esp will print out any */
/* received packets from the mqtt broker */

#include <AutoHome.h>

/* These variables need to be filled out with the information for your wifi details */
/* and the mqtt broker */
char const* wifi_ssid = "";
char const* wifi_password = "";
char const* mqtt_broker_ip = "";
char const* mqtt_user = "";
char const* mqtt_password = "";
char const* host_name = "";
char const* mqtt_topic = "";

AutoHome autohome;

/* This function will be called every time a packet is received from the mqtt topic. */
/* This is registered in the setup() */
void mqtt_callback(char* topic, byte* payload, unsigned int length){

  String packet = "";

  for (int i = 0; i < length; i++) {
    packet = packet + (char)payload[i];
  }

  Serial.print(packet);
  
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  /* This registers the function that gets called when a packet is recieved. */
  autohome.setPacketHandler(mqtt_callback);

  /* This starts the library and connects the esp to the wifi and the mqtt broker */
  autohome.begin(wifi_ssid, wifi_password, mqtt_broker_ip, mqtt_user, mqtt_password, host_name, mqtt_topic);

}

void loop() {

  /* This needs to be called in the loop as it handels the reconection to the mqtt server if it disconnects*/
  autohome.loop();

}
