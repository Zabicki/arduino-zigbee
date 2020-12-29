#include <SPI.h>
#include <WiFiNINA.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MyXBee.h>
#include "arduino_secrets.h"


#define czerwona 3
#define zielona 5
#define niebieska 6

byte endDeviceAddress[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xC1, 0x8A, 0x42};

byte testFrame[34] = {0x7E, 0x00, 0x1C, 0x10, 0x01, 0x00, 0x7D, 0x33, 0xA2, 0x00, 0x41, 0xC1, 0x8A, 0x42, 0xFF, 0xFE, 0x00, 0x00, 0x6A, 0x65, 0x64, 0x65, 0x6E, 0x20, 0x64, 0x77, 0x61, 0x20, 0x74, 0x72, 0x7A, 0x79, 0x7D, 0x33};

byte testFrameJson[34] = {0x7E, 0x00, 0x1C, 0x10, 0x01, 0x00, 0x7D, 0x33, 0xA2, 0x00, 0x41, 0xC1, 0x8A, 0x42, 0xFF, 0xFE, 0x00, 0x00, 0x7B, 0x22, 0x6A, 0x65, 0x64, 0x65, 0x6E, 0x22, 0x3A, 0x20, 0x31, 0x32, 0x35, 0x7D, 0x5D, 0x3A};


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// define endpoints
const String getTemp = "/temperature";
const String getHum = "/humidity";
const String setLedColor = "/ledColor";
const String getDistance = "/distance";

WiFiServer server(80);
SoftwareSerial xbee(2,3);

void setup() {
  Serial.begin(9600);      // initialize serial communication
  setPinModes();
  setupWiFi();
  xbee.begin(9600);

}

void simpleResolveEndpoint(String request, WiFiClient client) {
  //v0.1 - only GET, simply take first line, without query string
  String httpMethod = request.substring(0, request.indexOf(' '));
  String pathWithQueryString = request.substring(request.indexOf(" ") + 1, request.indexOf(" ", request.indexOf(" ") + 1));
  String path = "";
  String queryString = "";
  String headers = "";
  String body = "";

  if (pathWithQueryString.indexOf("?") != -1) {
    path = pathWithQueryString.substring(0, pathWithQueryString.indexOf("?"));
    queryString = pathWithQueryString.substring(pathWithQueryString.indexOf("?") + 1, pathWithQueryString.length());
  }
  else {
    path = pathWithQueryString;
  }

  headers = request.substring(request.indexOf('\n') + 1, request.indexOf("{") - 2);
  body = request.substring(request.indexOf("{"), request.length());

  Serial.println("HTTP Method: <" + httpMethod + ">");
  Serial.println("Path with query string: <" + pathWithQueryString + ">");
  Serial.println("Path w/out query string: <" + path + ">");
  Serial.println("Query string: <" + queryString + ">");
  Serial.println("Headers: <" + headers + ">");
  Serial.println("Body: <" + body + ">");

  XBeeTransmitRequestUtils xbeeResponse = XBeeTransmitRequestUtils();
  xbeeResponse.setSerial(xbee);

  TransmitRequestFrame frame = TransmitRequestFrame();
  frame.setSerial(xbee);
  JsonMessage message;
  
  if (path.equals(getHum)) {
    Serial.println("Humidity endpoint");
    
    String jsonMsg = message.serializeRequest("DHT11", "GET", "humidity");
    Serial.println("Json message: " + jsonMsg);

    frame.sendPacket(endDeviceAddress, jsonMsg);
    xbeeResponse.read(2000); //read request status
    
    xbeeResponse.read(2000); //read response
    if (xbeeResponse.frameReceived) {
      Serial.println("Response received");
      String payload = xbeeResponse.getPayloadAsString();
      while (!payload.endsWith("}")) {
        payload = payload.substring(0, payload.length() - 1);
      }
      Serial.println(payload);
      for (int i = 0; i < 50; i++) {
        Serial.println(xbeeResponse.frame[i], HEX);
      }
      responseOkHeader(client);
      client.println(payload);
      client.println();
    }
    else {
      responseFailHeader(client);
    }
  }
  else if (path.equals(getTemp)) {
    Serial.println("Temperature endpoint");
    
    String jsonMsg = message.serializeRequest("DHT11", "GET", "temperature");
    Serial.println("Json message: " + jsonMsg);

    frame.sendPacket(endDeviceAddress, jsonMsg);
    xbeeResponse.read(2000); //read request status
    
    xbeeResponse.read(2000); //read response
    if (xbeeResponse.frameReceived) {
      Serial.println("Response received");
      String payload = xbeeResponse.getPayloadAsString();
      while (!payload.endsWith("}")) {
        payload = payload.substring(0, payload.length() - 1);
      }
      Serial.println(payload);
      for (int i = 0; i < 50; i++) {
        Serial.println(xbeeResponse.frame[i], HEX);
      }
      responseOkHeader(client);
      client.println(payload);
      client.println();
    }
    else {
      responseFailHeader(client);
    }
  }
  else if (path.equals(getDistance)) {
    Serial.println("Distance endpoint");
    
    String jsonMsg = message.serializeRequest("HCSR04", "GET", "distance");
    Serial.println("Json message: " + jsonMsg);

    frame.sendPacket(endDeviceAddress, jsonMsg);
    xbeeResponse.read(2000); //read request status
    
    xbeeResponse.read(2000); //read response
    if (xbeeResponse.frameReceived) {
      Serial.println("Response received");
      String payload = xbeeResponse.getPayloadAsString();
      while (!payload.endsWith("}")) {
        payload = payload.substring(0, payload.length() - 1);
      }
      Serial.println(payload);
      for (int i = 0; i < 50; i++) {
        Serial.println(xbeeResponse.frame[i], HEX);
      }
      responseOkHeader(client);
      client.println(payload);
      client.println();
    }
    else {
      responseFailHeader(client);
    }
  }
  else if (path.equals(setLedColor)) {
    DynamicJsonDocument doc(body.length());
    DeserializationError err = deserializeJson(doc, body);

    int redValue = doc["red"];
    int greenValue = doc["green"];
    int blueValue = doc["blue"];
  
    analogWrite(czerwona, redValue);  
    analogWrite(zielona, greenValue);
    analogWrite(niebieska, blueValue); 
    responseOkHeader(client);
  }
   else if (path.equals(getDistance)) {
    xbee.print('D');
    delay(300);
    String response = "";
    
    while (xbee.available()) {
      int tmp = xbee.read() - '0';
      response += tmp;
    }
    
    Serial.println("Received distance is: " + response + " [cm]");
    responseOkHeader(client);
    client.println("{\"Distance\" : " + response +"}");
    client.println();
  }
}

void loop() {
  /*
  TransmitRequestFrame frame = TransmitRequestFrame();
  frame.setSerial(xbee);
  byte payload[20] = {'[', '"', 't', 'e', 's', 't', '"', ' ', ':', '"', '1', '2', '3', '4', '"', ']'};
  frame.sendPacket(endDeviceAddress, payload, 17);
  delay(5000);
  
  
  for (int i = 0; i < 34; i++) {
    xbee.write(testFrameJson[i]);
  }
  delay(5000);
  
  */
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    String request = "";                    // make a String to hold incoming data from the client
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
      }
      // all data read from client, parse request
      else {
        Serial.println("Received request is: ");
        Serial.println("---------------------");
        Serial.println(request);
        Serial.println("---------------------");
        simpleResolveEndpoint(request, client);
        break;
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void responseOkHeader(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println();
}

void responseFailHeader(WiFiClient client) {
  client.println("HTTP/1.1 500 Internal Server Error");
  client.println();
  client.println("Some error occured during processing the request");
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void setPinModes() {
  pinMode(czerwona, OUTPUT); //Konfiguracja wyprowadzeń jako wyjścia
  pinMode(zielona, OUTPUT);
  pinMode(niebieska, OUTPUT);
}

void setupWiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(6000);
  }

  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}
