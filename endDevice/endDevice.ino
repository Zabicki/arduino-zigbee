//#include "DHT.h"
#include <SoftwareSerial.h>
#include <MyXBee.h>
#include <XBee.h>


#define DHT11_PIN 7
#define trigPin 12
#define echoPin 11


//DHT dht;
SoftwareSerial xbee(2,3);

XBeeTransmitRequestUtils xbeeResponse = XBeeTransmitRequestUtils();
 
void setup()
{
  Serial.begin(9600);
  xbee.begin(9600);
  //dht.setup(DHT11_PIN);
  pinMode(trigPin, OUTPUT); //Pin, do którego podłączymy trig jako wyjście
  pinMode(echoPin, INPUT); //a echo, jako wejście
  xbeeResponse.setSerial(xbee);
}

/*
byte request[100];
byte payload[100];

byte frameType;
byte frameId;
byte msbLength;
byte lsbLength;
byte destinationAddress;
byte broadcastRadius;
byte options;
byte payloadLength;
//byte payload[15] = {0x7B, 0x0A, 0x22, 0x72, 0x65, 0x64, 0x22, 0x3A, 0x20, 0x32, 0x35, 0x35, 0x2C, 0x0A, 0x7D};
byte address16bit[2] = {0xFF, 0xFE};
long checksum;

int index = 0;
int startPayload = 16;

byte payloadArray[30];

bool transmissionStarted = false;
*/


 
void loop()
{
  xbeeResponse.read();
  if (xbeeResponse.frameReceived) {
    String payload = String((char*)xbeeResponse.payload);
    payload.replace("[", "{");
    payload.replace("]", "}");
    Serial.println(payload);
  }
  /*
    if (xbee.available()) {
      byte val = xbee.read();
      Serial.print("Received value from coordinator: " + val);
      
      if (val == 89) { //Y
        int wilgotnosc = dht.getHumidity();
        Serial.println(wilgotnosc);
        xbee.print(wilgotnosc, DEC);
      }
      else if (val == 78) { //N
        int temperatura = dht.getTemperature();
        Serial.println(temperatura);
        xbee.print(temperatura, DEC);
      }
      else if (val == 68) { //D
        long czas, dystans;
 
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
       
        czas = pulseIn(echoPin, HIGH);
        dystans = czas / 58;

        Serial.println(dystans);
        xbee.print(dystans, DEC);
      }
  }
  */
}
