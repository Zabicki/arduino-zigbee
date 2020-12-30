#include <CommonUtils.h>
#include <SPI.h>


#define DHT11_PIN 7
#define TRIG_PIN 12
#define ECHO_PIN 11

byte coordinatorAddress[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

SoftwareSerial xbee(2,3);

XBeeTransmitRequestUtils xbeeResponse = XBeeTransmitRequestUtils();
DHT dht;
 
void setup()
{
  Serial.begin(9600);
  xbee.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  xbeeResponse.setSerial(&xbee);
  dht.setup(DHT11_PIN);
}
 
void loop()
{
  xbeeResponse.read(2000);
  if (xbeeResponse.frameReceived) {
    Serial.println("Frame received");
    for (int i = 0; i < 50; i++) {
      Serial.println(xbeeResponse.frame[i], HEX);
    }
    
    String payload = xbeeResponse.getPayloadAsString();
    Serial.println(payload);
    
    JsonMessage message(payload);
    String value;
    String response;

    Serial.println(message.getDevice() + " " + message.getAction() + " " + message.getOption());

    if (message.getDevice() == DHT11) {
      value = String(handleDHT11(message));
      if (message.getOption() == "humidity") {
        response = "[\"humidity\":" + value + "]";
      }
      else if (message.getOption() == "temperature") {
        response = "[\"temperature\":" + value + "]";
      }
    }
    else if (message.getDevice() == HCSR04) {
      value = String(getDistance());
      response = "[\"distance\":" + value + "]";
    }

    Serial.println(response);

    TransmitRequestFrame frame = TransmitRequestFrame();
    frame.setSerial(&xbee);
    frame.sendPacket(coordinatorAddress, response);
    xbeeResponse.read(2000);
    xbeeResponse.frameReceived = false;
  }
}

int getDistance() {
  int time, distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  time = pulseIn(ECHO_PIN, HIGH);
  distance = time / 58;

  return distance;
}

int handleDHT11(JsonMessage message) {
  if (message.getOption() == ("temperature")) {
    return dht.getHumidity();
  }
  else if (message.getOption() == ("humidity")) {
    return dht.getTemperature();
  }
  else 
    return -1;
}
