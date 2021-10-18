# AutoHomeESP Arduino Library
This is an Arduino library that enables the ESP8266 to function as a home automation node.

Librarys needed to run this library:

	- PubSubClient - By Nick O'Leary - version 2.7.0
	- // not used any more // WiFiManager - by tzapu,tablatronix - version 2.0.3-alpha 
	- WiFiManager - by tzapu,	- version 0.16 
	- ArduinoJson - by Benoit Blanchon - version 6.15.0
	- ESP_DoubleResetDetector - by Khoi Hoang - version 1.0.3 - https://github.com/khoih-prog/ESP_DoubleResetDetector
	
Flash size settings for upload:

	-Flash Size 1M 64K SPIFFS

### Current implemented functions

```

setPacketHandler("callback function"); // Sets the callback function. This gets called when a packet is received from the MQTT server.

loop(); // This needs to be placed in the loop function. This handles all the backend for the library.

begin("Wifi SSID", "Wifi Password", "MQTT Broker IP", "MQTT Username","MQTT Password", "Device Host Name", "MQTT Subscribe Topic") // This sets up all the information to connect to the wifi and MQTT Broker.

begin("MQTT Broker IP", "MQTT Username","MQTT Password", "Device Host Name", "MQTT Subscribe Topic") // This sets up all the information to connect to the wifi and MQTT Broker using WiFiManager.

begin() // This is the super simple method. All you need to do is connect to the esp and setup the wifi and mqtt from the webpage.

sendPacket("Message"); // This sends a message to the mqtt topic that is stated in the begin function.

sendPacket("Topic", "Message"); // This sends a message to a specified topic.

resetSettings(); // This will reset all the setings by formating the SPIFFS.

setWatchdogTimeout(int timeoutMills);  // This will change the default timeout value for watchdog.


```

### Current examples

```

SimpleSetup - this works out of the box and will print out any mqtt packet that is received. Modify this to do what evere you want.

AdvancedSetup - This enables you to input any details using the variables in the program without the need for using the WiFiManager. 


```

built into the libary is some universall MQTT calls, 
- topic - - payload - 
/autohome : SCAN 
	gives
		String responce = "SCANRET:" + String(p_device_name) + ":" + String(j_device_type) + ":" + String(p_device_serial) + ":" + String(p_mqtt_channel) + ":" + String(WiFi.RSSI()) ;
		Device name : Device tyep : Device serial number : MQTT channel : wifi signal strenght in dB

- topic - - payload -
/autohome : INFO :"name"
	gives
		returns the same as above but only for the device named.
- topic - - payload -
/autohome/watchdog : WATCHDOG
	gives
		nothing
