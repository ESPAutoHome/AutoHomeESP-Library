#include "Wifi.h"

Wifi::Wifi(){}

Wifi::~Wifi(){}

void Wifi::begin(char const* wifi_ssid, char const* wifi_password){

  WiFiManager wifiManager;

  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  wifiManager.autoConnect(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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

  wifiManager.autoConnect();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Started Up");

}