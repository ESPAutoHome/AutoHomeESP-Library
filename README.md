# AutoHomeESP Arduino Library
This is an Arduino library that enables the ESP8266 to function as a home automation node.

Librarys needed to run this library:

	-PubSubClient
	-WiFiManager

### Current implemented functions

```

setPacketHandler("callback function"); // Sets the callback function. This gets called when a packet is received from the MQTT server.

loop(); // This needs to be placed in the loop function. This handles all the backend for the library.

begin("Wifi SSID", "Wifi Password", "MQTT Broker IP", "MQTT Username","MQTT Password", "Device Host Name", "MQTT Subscribe Topic") // This sets up all the information to connect to the wifi and MQTT Broker.

begin("MQTT Broker IP", "MQTT Username","MQTT Password", "Device Host Name", "MQTT Subscribe Topic") // This sets up all the information to connect to the wifi and MQTT Broker using WiFiManager.

sendPacket("Message"); // This sends a message to the mqtt topic that is stated in the begin function;

sendPacket("Topic", "Message"); // This sends a message to a specified topic


```
