#include <WiFiNINA.h>
#include <CommonUtils.h>


class MyServerUtils {
  public:
    
    #define RED 6
    #define GREEN 5
    #define BLUE 3
    
    MyServerUtils(SoftwareSerial* ss) {
      xbee = ss;
      xbee->begin(9600);
      //server = WiFiServer(80);
    }
    
    void setupWiFi(char* ssid, char* pass) {
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
        delay(10000);
      }

      server.begin();                           // start the web server on port 80
      printWifiStatus();                        // you're connected now, so print out the status
    }
      
    void setSerial(Stream &serialStream) {
      serial = &serialStream;
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
    
    void handleClientRequest() {
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
        DynamicJsonDocument doc(100);
        DeserializationError err = deserializeJson(doc, body);

        int redValue = doc["red"];
        int greenValue = doc["green"];
        int blueValue = doc["blue"];

        Serial.println("Red: " + String(redValue));
        Serial.println("Green: " + String(greenValue));
        Serial.println("Blue: " + String(blueValue));
      
        analogWrite(RED, 255 - redValue);  
        analogWrite(GREEN, 255 - greenValue);
        analogWrite(BLUE, 255 - blueValue); 
        responseOkHeader(client);
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
  
  private:
    Stream* serial;
    WiFiServer server = WiFiServer(80);
    int status = WL_IDLE_STATUS;

    // define endpoints
    const String getTemp = "/temperature";
    const String getHum = "/humidity";
    const String setLedColor = "/ledColor";
    const String getDistance = "/distance";
    int keyIndex = 0; // network key Index number (needed only for WEP)
    SoftwareSerial* xbee;
    
    byte endDeviceAddress[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xC1, 0x8A, 0x42};
  
};
