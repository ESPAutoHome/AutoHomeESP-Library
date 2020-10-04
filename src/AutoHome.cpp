#include "AutoHome.h"
#define DRD_TIMEOUT 10
#define DRD_ADDRESS 0
#define RETRY_DELAY_MS 5000

enum AutoHomeConnectionState
{
	wifi_config_portal,
	trying_to_connect_to_wifi,
	connected_to_wifi,
	connected_to_wifi_and_mqtt
};

AutoHomeConnectionState connectionState = trying_to_connect_to_wifi;

#ifdef ARDUINO_ARCH_ESP32
fs::SPIFFSFS *fileSystem = &SPIFFS;
#else
FS *fileSystem = &LittleFS;
#endif

unsigned long lastRetryTime = 0;
Wifi wifi;
OTAUpdate ota;
WiFiClient espClient;
PubSubClient pubclient(espClient);
MQTT mqtt;
WiFiManager wifiManager;
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

char const *p_mqtt_channel;
char const *p_host;
char const *p_mqtt_user;
char const *p_mqtt_password;
char const *p_mqtt_server;
char const *p_mqtt_port;
char const *p_device_name;
char const *p_device_type;
char const *p_device_serial;

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

AutoHome::AutoHome() {}

AutoHome::~AutoHome() {}

void saveConfigCallback()
{
	Serial.println("Should save config");
	shouldSaveConfig = true;
}

void AutoHome::resetSettings()
{
	fileSystem->format();
}

void AutoHome::begin()
{
	for (int i = 0; i < len(j_mqtt_server); i++)
	{
		j_mqtt_server[i] = 0;
	}

	Serial.println("Mounting FS...");

#ifdef ARDUINO_ARCH_ESP32
	if (fileSystem->begin(true))
#else
	if (fileSystem->begin())
#endif
	{
		Serial.println("Mounted file system");
		if (fileSystem->exists("/AutoHome_config.json"))
		{
			//file exists, reading and loading
			Serial.println("Reading config file");
			File configFile = fileSystem->open("/AutoHome_config.json", "r");

			if (configFile)
			{
				Serial.println("Config file opened");

				DynamicJsonDocument json(2048);
				DeserializationError dError = deserializeJson(json, configFile);
				if (!dError)
				{
					Serial.println("Json successfully parsed");

					strcpy(j_mqtt_server, json["j_mqtt_server"]);
					strcpy(j_mqtt_port, json["j_mqtt_port"]);
					strcpy(j_mqtt_user, json["j_mqtt_user"]);
					strcpy(j_mqtt_password, json["j_mqtt_password"]);
					strcpy(j_mqtt_channel, json["j_mqtt_channel"]);
					strcpy(j_host, json["j_host"]);
					strcpy(j_device_name, json["j_device_name"]);
					strcpy(j_device_type, json["j_device_type"]);
					strcpy(j_device_serial, json["j_device_serial"]);
				}
				else
				{
					Serial.println("Failed to parse the json config file");
					Serial.println(dError.c_str());
				}
				configFile.close();
			}
		}
		fileSystem->end();
	}
	else
	{
		Serial.println("Failed to mount FS");
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

	// Only show the config portal if we explicit call wifiManager.startConfigPortal(...)
	wifiManager.setEnableConfigPortal(false);

	// Checks if the mqtt server is set
	bool isMqttServerSet = false;
	for (int i = 0; i < len(j_mqtt_server); i++)
	{
		if (j_mqtt_server[i] != 0)
			isMqttServerSet = true;
	}

	// Checks if we should show the config portal
	if (drd.detectDoubleReset() || !isMqttServerSet)
	{
		if (!isMqttServerSet)
		{
			Serial.println("The Mqtt settings are not set, starts the config portal");
		}
		else
		{
			Serial.println("Double reset detected, starts the config portal");
		}

		wifiManager.setConfigPortalBlocking(false);
		wifiManager.startConfigPortal("Autohome");
		connectionState = wifi_config_portal;
	}
	else if (!wifiManager.autoConnect())
	{
		Serial.println("Failed to connect to wifi, will retry");
		connectionState = trying_to_connect_to_wifi;
		lastRetryTime = millis();
	}
	else
	{
		connectionState = connected_to_wifi;
		connectedToWifi();
	}
}

AutoHome::void connectedToWifi()
{
	Serial.println("");
	Serial.println("Wi-Fi connected");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	if (shouldSaveConfig)
	{
		Serial.println("Mounting FS...");
#ifdef ARDUINO_ARCH_ESP32
		if (fileSystem->begin(true))
#else
		if (fileSystem->begin())
#endif
		{

			Serial.println("FS mounted");
			strcpy(j_mqtt_server, custom_mqtt_server.getValue());
			strcpy(j_mqtt_port, custom_mqtt_port.getValue());
			strcpy(j_mqtt_user, custom_mqtt_user.getValue());
			strcpy(j_mqtt_password, custom_mqtt_password.getValue());
			strcpy(j_mqtt_channel, custom_mqtt_channel.getValue());
			strcpy(j_host, custom_host.getValue());
			strcpy(j_device_name, custom_device_name.getValue());
			strcpy(j_device_type, custom_device_type.getValue());
			strcpy(j_device_serial, custom_device_serial.getValue());
			Serial.println("Saving config");
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

			File configFileSave = fileSystem->open("/AutoHome_config.json", "w");
			if (!configFileSave)
			{
				Serial.println("Failed to open config file for writing");
			}
			if (serializeJson(json, configFileSave) == 0)
			{
				Serial.println("Failed to write config file");
			}
			serializeJson(json, Serial);

			configFileSave.close();
			Serial.println("Successfully saved config file");
			if (fileSystem->exists("/AutoHome_config.json"))
			{
				Serial.println("Reading back config file");
				File configFileReadBack = fileSystem->open("/AutoHome_config.json", "r");
				while (configFileReadBack.available())
				{
					Serial.print((char)configFileReadBack.read());
				}
				configFileReadBack.close();
			}
			else
			{
				Serial.println("Saved file does not exist, is the FS in read-only mode?");
			}
			fileSystem->end();
		}
		else
		{
			Serial.println("Failed to mount FS");
		}
	}

	String port = String(j_mqtt_port);
	Serial.println("MQTT settings:");
	Serial.println("mqtt_server: " + String(j_mqtt_server));
	Serial.println("mqtt_port: " + port);
	pubclient.setServer(j_mqtt_server, port.toInt());

	delay(30);

	p_mqtt_channel = j_mqtt_channel;
	p_host = j_host;
	p_mqtt_user = j_mqtt_user;
	p_mqtt_password = j_mqtt_password;

	p_device_name = j_device_name;
	p_device_type = j_device_type;
	p_device_serial = j_device_serial;

	if (mqtt.reconnect(pubclient, p_mqtt_channel, p_host, p_mqtt_user, p_mqtt_password))
	{
		connectionState = connected_to_wifi_and_mqtt;
	}

	ota.begin(j_host);
}

void AutoHome::loop()
{
	drd.loop();
	unsigned long currentTime = millis();
	switch (connectionState)
	{

	// The config portal is open
	case wifi_config_portal:
	{
		wifimanager.process();
		if (shouldSaveConfig)
		{
			connectedToWifi();
			shouldSaveConfig = false;
		}
	}

	// Try reconnect to the Wi-Fi
	case trying_to_connect_to_wifi:
	{
		if (currentTime - lastRetryTime > RETRY_DELAY_MS)
		{
			lastRetryTime = currentTime;
			Serial.println("Trying to connect to Wi-Fi...");
			if (wifiManager.autoConnect())
			{
				connectedToWifi();
			}
			else
			{
				Serial.println("Failed to connect to Wi-Fi, will retry in a little while");
				connectionState = trying_to_connect_to_wifi;
			}
		}
	}

	// Connected to wifi but not MQTT
	case connected_to_wifi:
	{
		ArduinoOTA.handle();
		if (currentTime - lastRetryTime > RETRY_DELAY_MS)
		{
			lastRetryTime = currentTime;
			if (mqtt.reconnect(pubclient, p_mqtt_channel, p_host, p_mqtt_user, p_mqtt_password))
			{
				connectionState = connected_to_wifi_and_mqtt;
			}
			else
			{
				Serial.println("Failed to connect to MQTT, will retry in a little while");
				connectionState = connected_to_wifi;
			}

			// Checks if we have lost connection to the wifi
			if (!wifiManager.autoConnect())
			{
				Serial.println("Lost connection to the Wi-Fi");
				connectionState = trying_to_connect_to_wifi;
			}
		}
	}

	// Connected to wifi and MQTT
	case connected_to_wifi_and_mqtt:
	{
		ArduinoOTA.handle();
		if (!pubclient.connected())
		{
			Serial.println("Lost connection to the MQTT");
			connectionState = connected_to_wifi;
			return;
		}
		pubclient.loop();
	}
	}
}

void AutoHome::setPacketHandler(void (*mqttcallback)(char *, uint8_t *, unsigned int))
{
	pubclient.setCallback(mqttcallback);
}

void AutoHome::sendPacket(char const *message)
{
	pubclient.publish(p_mqtt_channel, message);
}

void AutoHome::sendPacket(char const *topic, char const *message)
{
	pubclient.publish(topic, message);
}

String AutoHome::getValue(String data, char separator, int index)
{
	return mqtt.getValue(data, separator, index);
}

char AutoHome::mqtt_callback(char *topic, byte *payload, unsigned int length)
{
	return mqtt.mqtt_callback(
		pubclient,
		topic,
		payload,
		length,
		p_device_name,
		p_device_type,
		p_device_serial,
		p_mqtt_channel,
		WiFi.RSSI());
}
