#include "AutoHome.h"
#define DRD_TIMEOUT 10
#define DRD_ADDRESS 0

Wifi wifi;
OTAUpdate ota;
WiFiClient espClient;
PubSubClient pubclient(espClient);
MQTT mqtt;
WiFiManager wifiManager;
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

char const* p_mqtt_channel;
char const* p_host;
char const* p_mqtt_user;
char const* p_mqtt_password;
char const* p_mqtt_server;
char const* p_mqtt_port;
char const* p_device_name;
char const* p_device_type;
char const* p_device_serial;

char j_mqtt_channel[50];
char j_host[20];
char j_mqtt_user[30];
char j_mqtt_password[30];
char j_mqtt_server[30];
char j_mqtt_port[10];
char j_device_name[20];
char j_device_type[20];
char j_device_serial[20];

bool shouldSaveConfig = false;

AutoHome::AutoHome(){}

AutoHome::~AutoHome(){}

void AutoHome::setPacketHandler(void(*mqttcallback)(char*,uint8_t*,unsigned int)){

	pubclient.setCallback(mqttcallback);

}

String AutoHome::getValue(String data, char separator, int index)
{

  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";

}

char AutoHome::mqtt_callback(char* topic, byte* payload, unsigned int length){

	String packet = "";

	for (int i = 0; i < length; i++) {
		packet = packet + (char)payload[i];
	}

	char autohomeTopic[] = "/autohome";

    if(strcmp(topic, autohomeTopic) == 0){
		
		Serial.println("Autohome Packet");
		
		if(packet.equals("SCAN")){
			
			Serial.println("Autohome Scan Packet");
			
			String responce = "SCANRET:" + String(p_device_name) + ":" + String(j_device_type) + ":" + String(p_device_serial) + ":" + String(p_mqtt_channel) + ":" + String(WiFi.RSSI());
			
			sendPacket("/autohome", responce.c_str());
			
		} else if(getValue(packet, ':',0).equals("INFO")){
			if(String(p_device_name).equals(getValue(packet, ':',1))){
				String responce = "INFORES:" + String(p_device_name) + ":" + String(j_device_type) + ":" + String(p_device_serial) + ":" + String(p_mqtt_channel) + ":" + String(WiFi.RSSI());
				sendPacket("/autohome", responce.c_str());
			}
		}
		
		return 1;
		
	}

	return 0;
  
}

void saveConfigCallback() {
	
  Serial.println("Should save config");
  shouldSaveConfig = true;
  
}

void AutoHome::resetSettings(){
	
	// LittleFS.format();		// esp8266
	   SPIFFS.format();
}

void AutoHome::begin(){
	
	Serial.println("mounting FS...");

//	if (LittleFS.begin()) {	// ESP8266
	if (SPIFFS.begin()) {	// ESP32
	
		Serial.println("mounted file system");
//		if (LittleFS.exists("/AutoHome_config.json")) {	// ESP8266
		if (SPIFFS.exists("/AutoHome_config.json")) {	// ESP32
		  //file exists, reading and loading
		  Serial.println("reading config file");
	//	  File configFile = LittleFS.open("/AutoHome_config.json", "r");	// esp8266
		  File configFile = SPIFFS.open("/AutoHome_config.json", "r");	// ESP32
		  
		  if (configFile) {
			Serial.println("opened config file");

			DynamicJsonDocument json(2048);
			DeserializationError dError = deserializeJson(json,configFile);			
			if (!dError) {
			  Serial.println("\nparsed json");

			  strcpy(j_mqtt_server, json["j_mqtt_server"]);
			  strcpy(j_mqtt_port, json["j_mqtt_port"]);
			  strcpy(j_mqtt_user, json["j_mqtt_user"]);
			  strcpy(j_mqtt_password, json["j_mqtt_password"]);
			  strcpy(j_mqtt_channel, json["j_mqtt_channel"]);
			  strcpy(j_host, json["j_host"]);
			  strcpy(j_device_name, json["j_device_name"]);
			  strcpy(j_device_type, json["j_device_type"]);
			  strcpy(j_device_serial, json["j_device_serial"]);

			} else {
			  Serial.println(dError.c_str());
			  Serial.println("failed to load json config");
			  
			}
			configFile.close();
		  }
		  
		}
	
	} else {
		
		Serial.println("failed to mount FS");
	
	}
	
	WiFiManagerParameter custom_mqtt_server("j_mqtt_server", "MQTT Server IP", j_mqtt_server, 30);
	WiFiManagerParameter custom_mqtt_port("j_mqtt_port", "MQTT Server Port", j_mqtt_port, 10);
	WiFiManagerParameter custom_mqtt_user("j_mqtt_user", "MQTT Username", j_mqtt_user, 30);
	WiFiManagerParameter custom_mqtt_password("j_mqtt_password", "MQTT password", j_mqtt_password, 30);
	WiFiManagerParameter custom_mqtt_channel("j_mqtt_channel", "MQTT channel", j_mqtt_channel, 50);
	WiFiManagerParameter custom_host("j_host", "Host Name", j_host, 20);
	WiFiManagerParameter custom_device_name("j_device_name", "Device Name", j_device_name, 20);
	WiFiManagerParameter custom_device_type("j_device_type", "Device Type", j_device_type, 20);
	WiFiManagerParameter custom_device_serial("j_device_serial", "Device Serial Number", j_device_serial, 20);
	
	wifiManager.setSaveConfigCallback(saveConfigCallback);
	
	wifiManager.addParameter(&custom_mqtt_server);
	wifiManager.addParameter(&custom_mqtt_port);
	wifiManager.addParameter(&custom_mqtt_user);
	wifiManager.addParameter(&custom_mqtt_password);
	wifiManager.addParameter(&custom_mqtt_channel);
	wifiManager.addParameter(&custom_host);
	wifiManager.addParameter(&custom_device_name);
	wifiManager.addParameter(&custom_device_type);
	wifiManager.addParameter(&custom_device_serial);

	wifiManager.setConfigPortalTimeout(180);

	if(drd.detectDoubleReset()){
		wifiManager.startConfigPortal("Autohome");
	} else if (!wifiManager.autoConnect()){
	  
		Serial.println("failed to connect and hit timeout");
		delay(3000);
		//reset and try again, or maybe put it to deep sleep
	//	ESP.reset();	// ESP8266
		ESP.restart();	// esp32
		delay(5000);
	  
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println("Started Up");
	if (shouldSaveConfig) {
		strcpy(j_mqtt_server, custom_mqtt_server.getValue());
		strcpy(j_mqtt_port, custom_mqtt_port.getValue());
		strcpy(j_mqtt_user, custom_mqtt_user.getValue());
		strcpy(j_mqtt_password, custom_mqtt_password.getValue());
		strcpy(j_mqtt_channel, custom_mqtt_channel.getValue());
		strcpy(j_host, custom_host.getValue());
		strcpy(j_device_name, custom_device_name.getValue());
		strcpy(j_device_type, custom_device_type.getValue());
		strcpy(j_device_serial, custom_device_serial.getValue());
		Serial.println("saving config");
		DynamicJsonDocument json(2048);
		json["j_mqtt_server"] = j_mqtt_server;
		json["j_mqtt_port"] = j_mqtt_port;
		json["j_mqtt_user"] = j_mqtt_user;
		json["j_mqtt_password"] = j_mqtt_password;
		json["j_mqtt_channel"] = j_mqtt_channel;
		json["j_host"] = j_host;
		json["j_device_name"] = j_device_name;
		json["j_device_type"] = j_device_type;
		json["j_device_serial"] = j_device_serial;

//		File configFileSave = LittleFS.open("/AutoHome_config.json", "w");	// ESP8266
		File configFileSave = SPIFFS.open("/AutoHome_config.json", "w");	// ESP32
		if (!configFileSave) {
		  Serial.println("failed to open config file for writing");
		}
		if(serializeJson(json,configFileSave) == 0){
					Serial.println("failed to write");
		}
		serializeJson(json,Serial);

		configFileSave.close();
		Serial.println("saved config");
	//	if (LittleFS.exists("/AutoHome_config.json")) {		// ESP8266
		if (SPIFFS.exists("/AutoHome_config.json")) {		// ESP32
		  Serial.println("reading back config file");
		//  File configFileReadBack = LittleFS.open("/AutoHome_config.json", "r");	// ESP8266
		    File configFileReadBack = SPIFFS.open("/AutoHome_config.json", "r");	// ESP	
		while (configFileReadBack.available()) {
			Serial.print((char)configFileReadBack.read());
		}
  		  configFileReadBack.close();
		}else {
			Serial.println("saved file does not exist");
		}
	}
	// LittleFS.end();	// ESP8266
	SPIFFS.end();		// ESP32
	
	String port = String(j_mqtt_port);
	
	pubclient.setServer(j_mqtt_server, port.toInt());

	delay(30);

	mqtt.reconnect(pubclient, j_mqtt_channel, j_host, j_mqtt_user, j_mqtt_password);

	p_mqtt_channel = j_mqtt_channel;
	p_host = j_host;
	p_mqtt_user = j_mqtt_user;
	p_mqtt_password = j_mqtt_password;	
	
	p_device_name = j_device_name;
	p_device_type = j_device_type;
	p_device_serial = j_device_serial;
	
	ota.begin(j_host);
	
	
}

void AutoHome::begin(char const* wifi_ssid, char const* wifi_password, char const* mqtt_ip, char const* mqtt_user, char const* mqtt_password, char const* host, char const* mqtt_channel){

	Serial.println("AutoHome Starting a");

	wifi.begin(wifi_ssid, wifi_password);

	pubclient.setServer(mqtt_ip, 1883);

	mqtt.reconnect(pubclient, mqtt_channel, host, mqtt_user, mqtt_password);

	p_mqtt_channel = mqtt_channel;
	p_host = host;
	p_mqtt_user = mqtt_user;
	p_mqtt_password = mqtt_password;

	ota.begin(host);	

}

void AutoHome::begin(char const* mqtt_ip, char const* mqtt_user, char const* mqtt_password, char const* host, char const* mqtt_channel){

	Serial.println("AutoHome Starting b");

	wifi.begin();

	pubclient.setServer(mqtt_ip, 1883);

	mqtt.reconnect(pubclient, mqtt_channel, host, mqtt_user, mqtt_password);

	p_mqtt_channel = mqtt_channel;
	p_host = host;
	p_mqtt_user = mqtt_user;
	p_mqtt_password = mqtt_password;

	ota.begin(host);

}

void AutoHome::loop(){
	
	long currentTime = millis();

	ArduinoOTA.handle();

	if (!pubclient.connected()) {

		mqtt.reconnect(pubclient, p_mqtt_channel, p_host, p_mqtt_user, p_mqtt_password);
	}

	pubclient.loop();

	drd.loop();

}

void AutoHome::sendPacket(char const* message){

	pubclient.publish(p_mqtt_channel, message);

}

void AutoHome::sendPacket(char const* topic, char const* message){

	pubclient.publish(topic, message);

}
///////////////////////////////////////////////////////


// void ReadVariables(){
// 	LittleFS.begin();
// 	File verables = LittleFS.open("/verables", "r"); // opens the file on flash as "r" read
// 	size_t FileSize = verables.size();             // get file size in bytes
// 	if (FileSize >= 4) // check if file size is full or not.
//   {
//     verables.readBytes(*, FileSize);         // read file verables and put in buffer

//     spEEd = buf[0];
//     m0de = buf[2];
//     Brightness = buf[3]; // overall brightness
//   }
//   verables.close(); // close file.
//   	LittleFS.end();

// }

//   void WriteVerables()
// {
// 		LittleFS.begin();

//   File verables = LittleFS.open("/verables", "w"); // open file verbales on SPIFF file system to write to.
//   if (!verables)
//   {
//     Serial.println("error opening verbles file :C ");
//     return;
//   }
//   char verableE[3] = {spEEd, cOl, m0de, Brightness}; // creat char array
//   if (verables.print(verableE))                      // write char array to file system
//   {
//     Serial.print("wrote verables");
//   }
//   else
//   {
//     Serial.println("failed");
//   }

//   verables.close();
//   	LittleFS.end();

// }