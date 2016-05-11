#include "Wifi.h"

Wifi::Wifi(){}

Wifi::~Wifi(){}

void Wifi::begin(char const* wifi_ssid, char const* wifi_password){

  WiFiManager wifiManager;

  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  if (!wifiManager.autoConnect(wifi_ssid, wifi_password)){
	  
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

}

void Wifi::begin(){

  WiFiManager wifiManager;

  Serial.print("Using AutoWifi");

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

}