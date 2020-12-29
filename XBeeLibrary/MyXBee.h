#include <ArduinoJson.h>
#include "DHT.h"

#define DHT11 "DHT11"
#define HCSR04 "HCSR04"

class TransmitRequestFrame {

  /*
  #define START_BYTE 0x7e
  #define ESCAPE 0x7d

  #define FRAME_TYPE 0x10
  #define FRAME_ID 0x01
  #define MSB_LENGTH 0x00
  #define BROADCAST_RADIUS 0x00
  #define OPTIONS 0x00
  */

  //TODO zmienic 14 na hex
  //#define FRAME_SIZE_CHECKSUM 14
  
  public:
    byte START_BYTE = 0x7E;
    byte FRAME_TYPE = 0x10;
    byte FRAME_ID = 0x01;
    byte MSB_LENGTH = 0x00;
    byte BROADCAST_RADIUS = 0x00;
    byte OPTIONS = 0x00;
    byte lsbLength;
    byte destinationAddress;
    byte payloadLength;
    byte address16bit[2] = {0xFF, 0xFE};
    byte checksum;

    void setSerial(Stream &serialStream) {
      serial = &serialStream;
    }

    byte sendByte(byte dataByte) {
      serial->write(dataByte);
      return dataByte;
    }

    //TODO calculate checksum
    void sendPacket(byte* endDeviceAddress, byte* payload, int payloadSize) {
      
      //send start byte
      sendByte(START_BYTE);
      
      //send msb length
      sendByte(MSB_LENGTH);
      
      byte lsbDecByteLength = 14 + payloadSize;
      
      //send frame length
      sendByte(lsbDecByteLength);
      
      //send frame type
      checksum += sendByte(FRAME_TYPE);
      
      //send frame id
      checksum += sendByte(FRAME_ID);

      //send destination device address
      for (int i = 0; i < 8; i++) {
        checksum += sendByte(endDeviceAddress[i]);
      }
      
      //send 16-bit destination address
      checksum += sendByte(0xFF);
      checksum += sendByte(0xFE);
      
      //send broadcast radius
      checksum += sendByte(BROADCAST_RADIUS);
      
      //send options
      checksum += sendByte(OPTIONS);
      
      //send payload
      for (int i = 0; i < payloadSize; i++) {
        checksum += sendByte(payload[i]);
      }
      checksum = 0xFF - checksum;
      
      //send checksum
      sendByte(checksum);
    }

    private:
      Stream* serial;
};

class XBeeTransmitRequestUtils {

  #define TRANSMIT_FRAME_PAYLOAD_OFFSET 16

  public:
    byte frame[150];
    byte payload[100];
    int payloadIndex;
    int frameIndex;
    bool frameReceived;
    bool invalidFrame;

    void setSerial(Stream &serialStream) {
      serial = &serialStream;
    }

    void readPacket() {
      while (serial->available()) {
        frame[frameIndex] = serial->read();
        if (frameIndex == 3) {
          if (frame[3] != 0x90) {
              invalidFrame = true;
          }
        }
        frameIndex++;
      }
    }

    void read(unsigned long timeout) {
      unsigned long start = millis();
      
      while (true) {
        if ((millis() - start) > timeout) {
          return;
        }
        if (serial->available()) {
          break;
        }
      }
      
      frameReceived = false;
      invalidFrame = false;
      if (serial->available()) {
        if (serial->peek() == 0x7e) {
          readPacket(400);
        }
        else {
          serial->read();
        }
      }
    }

    // timeout - max request receiving time 
    void readPacket(unsigned long timeout) {
      payloadIndex = 0;
      frameIndex = 0;

      unsigned long start = millis();

      while ((millis() - start) < timeout) {
        readPacket();
      }
      
      
      if (invalidFrame) {
        frameIndex = 0;
        payloadIndex = 0;
        frameReceived = false;
        invalidFrame = false;
      }

      for (int i = TRANSMIT_FRAME_PAYLOAD_OFFSET; i < frameIndex - 1; i++) {
        payload[payloadIndex] = frame[i];
        payloadIndex++;	
      }

      frameReceived = true;
    }
    
    bool isFrameReceived() {
      return frameReceived;
    }
    
    
    String getPayloadAsString() {
      //TODO fix curly brackets transfer
      //for now replace rectangular brackets with curly brackets
      String payloadString = String((char*)payload);
      payloadString.replace("[", "{");
      payloadString.replace("]", "}");
      return payloadString;
    }

  private:
    Stream* serial;
};


class JsonMessage {
  public:
    
    JsonMessage() {}
    
    JsonMessage(String payload) {
      DynamicJsonDocument doc(200);
      deserializeJson(doc, payload);
      
      char* tDevice = doc["device"];
      char* tAction = doc["action"];
      char* tOption = doc["option"];
      
      device = String(tDevice);
      action = String(tAction);
      option = String(tOption);
    }
    
    String getDevice() {
      return device;
    }
    
    String getAction() {
      return action;
    }
    
    String getOption() {
      return option;
    }
    
    String serializeRequest(String device, String action, String option) {
      DynamicJsonDocument doc(300);

      doc["device"] = "DHT11";
      doc["action"] = "GET";
      doc["option"] = "humidity";

      String jsonMsg;
      serializeJson(doc, jsonMsg);
      jsonMsg.replace("{", "[");
      jsonMsg.replace("}", "]");
      
      this->device = device;
      this->action = action;
      this->option = option;
      
      return jsonMsg;
    }
    
  private:
    String device;
    String action;
    String option;
};
