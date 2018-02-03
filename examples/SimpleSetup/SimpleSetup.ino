/* This example is a simple setup example to show you how to setup the AutoHome library. */

#include <AutoHome.h>

AutoHome autohome;

/* This function will be called every time a packet is received from the mqtt topic. */
/* This is registered in the setup() */
void mqtt_callback(char* topic, byte* payload, unsigned int length){

  if(!autohome.mqtt_callback(topic, payload, length)){

      String packet = "";

      for (int i = 0; i < length; i++) {
        packet = packet + (char)payload[i];
      }

      Serial.print(packet);

  }
  
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  /* This registers the function that gets called when a packet is recieved. */
  autohome.setPacketHandler(mqtt_callback);

  /* This starts the library and connects the esp to the wifi and the mqtt broker */
  autohome.begin();

}

void loop() {

  /* This needs to be called in the loop as it handels the reconection to the mqtt server if it disconnects*/
  autohome.loop();

}
