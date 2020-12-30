#include <MyXBee.h>
#include <CoordinatorUtils.h>

#include "arduino_secrets.h"


#define RED 6
#define GREEN 5
#define BLUE 3

// WiFi credentials stores in arduino_secrets.h file
char ssid[] = SECRET_SSID;    // network SSID
char pass[] = SECRET_PASS;    // network password

SoftwareSerial xbee(2,3);
MyServerUtils server(&xbee);

void setup() {
  Serial.begin(9600);
  setPinModes();
  server.setSerial(Serial);
  server.setupWiFi(ssid, pass);
}

void loop() {
  server.handleClientRequest();
}

void setPinModes() {
  pinMode(RED, OUTPUT); //Konfiguracja wyprowadzeń jako wyjścia
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}
