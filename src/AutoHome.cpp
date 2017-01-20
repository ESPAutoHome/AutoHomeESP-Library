#include "AutoHome.h"

Wifi wifi;
OTAUpdate ota;
WiFiClient espClient;
PubSubClient pubclient(espClient);
MQTT mqtt;
WiFiManager wifiManager;

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
			
			String responce = "SCANRET:" + String(p_device_name) + ":" + String(j_device_type) + ":" + String(p_device_serial) + ":" + String(p_mqtt_channel);
			
			sendPacket("/autohome", responce.c_str());
			
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
	
	SPIFFS.format();
	
}

void AutoHome::begin(){
	
	Serial.println("mounting FS...");

	if (SPIFFS.begin()) {
		
		Serial.println("mounted file system");
		if (SPIFFS.exists("/AutoHome_config.json")) {
		  //file exists, reading and loading
		  Serial.println("reading config file");
		  File configFile = SPIFFS.open("/AutoHome_config.json", "r");
		  
		  if (configFile) {
			Serial.println("opened config file");
			size_t size = configFile.size();
			// Allocate a buffer to store contents of the file.
			std::unique_ptr<char[]> buf(new char[size]);

			configFile.readBytes(buf.get(), size);
			DynamicJsonBuffer jsonBuffer;
			JsonObject& json = jsonBuffer.parseObject(buf.get());
			json.printTo(Serial);
			
			if (json.success()) {
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
				
			  Serial.println("failed to load json config");
			  
			}
			
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

	if (!wifiManager.autoConnect()){
	  
		Serial.println("failed to connect and hit timeout");
		delay(3000);
		//reset and try again, or maybe put it to deep sleep
		ESP.reset();
		delay(5000);
	  
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println("Started Up");
	
	strcpy(j_mqtt_server, custom_mqtt_server.getValue());
	strcpy(j_mqtt_port, custom_mqtt_port.getValue());
	strcpy(j_mqtt_user, custom_mqtt_user.getValue());
	strcpy(j_mqtt_password, custom_mqtt_password.getValue());
	strcpy(j_mqtt_channel, custom_mqtt_channel.getValue());
	strcpy(j_host, custom_host.getValue());
	strcpy(j_device_name, custom_device_name.getValue());
	strcpy(j_device_type, custom_device_type.getValue());
	strcpy(j_device_serial, custom_device_serial.getValue());
	
	if (shouldSaveConfig) {
		
		Serial.println("saving config");
		DynamicJsonBuffer jsonBuffer;
		JsonObject& json = jsonBuffer.createObject();
		json["j_mqtt_server"] = j_mqtt_server;
		json["j_mqtt_port"] = j_mqtt_port;
		json["j_mqtt_user"] = j_mqtt_user;
		json["j_mqtt_password"] = j_mqtt_password;
		json["j_mqtt_channel"] = j_mqtt_channel;
		json["j_host"] = j_host;
		json["j_device_name"] = j_device_name;
		json["j_device_type"] = j_device_type;
		json["j_device_serial"] = j_device_serial;

		File configFile = SPIFFS.open("/AutoHome_config.json", "w");
		if (!configFile) {
		  Serial.println("failed to open config file for writing");
		}

		json.printTo(Serial);
		json.printTo(configFile);
		configFile.close();
		
		Serial.println("local ip");
		Serial.println(WiFi.localIP());
		
	}

	String port = String(j_mqtt_port);
	
	pubclient.setServer(j_mqtt_server, port.toInt());

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

	Serial.println("AutoHome Starting");

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

	Serial.println("AutoHome Starting");

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
	
	ArduinoOTA.handle();

	if (!pubclient.connected()) {
		mqtt.reconnect(pubclient, p_mqtt_channel, p_host, p_mqtt_user, p_mqtt_password);
	}

	pubclient.loop();

}

void AutoHome::sendPacket(char const* message){

	pubclient.publish(p_mqtt_channel, message);

}

void AutoHome::sendPacket(char const* topic, char const* message){

	pubclient.publish(topic, message);

}
