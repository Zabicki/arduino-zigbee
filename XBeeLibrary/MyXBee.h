class TransmitRequestFrame {

  #define START_BYTE 0x7e
  #define ESCAPE 0x7d

  #define FRAME_TYPE 0x10
  #define FRAME_ID 0x01
  #define MSB_LENGTH 0x00
  #define BROADCAST_RADIUS 0x00
  #define OPTIONS 0x00

  //TODO zmienic 14 na hex
  //#define FRAME_SIZE_CHECKSUM 14
  
  public:
    //byte frameId;
    byte lsbLength;
    byte destinationAddress;
    byte payloadLength;
    //byte payload[15] = {0x7B, 0x0A, 0x22, 0x72, 0x65, 0x64, 0x22, 0x3A, 0x20, 0x32, 0x35, 0x35, 0x2C, 0x0A, 0x7D};
    byte address16bit[2] = {0xFF, 0xFE};
    byte checksum;

    void setSerial(Stream &serialStream) {
      serial = &serialStream;
    }

    byte sendByte(byte dataByte) {
      serial->write(dataByte);
      return dataByte;
    }

    //todo calculate checksum
    void sendPacket(byte* endDeviceAddress, byte* payload, int payloadSize) {
      sendByte(START_BYTE);
      sendByte(MSB_LENGTH);
      //TODO calculate length and translate to HEX
      int lsbDecByteLength = 14 + payloadSize;
      sendByte(lsbDecByteLength);
      checksum += sendByte(FRAME_TYPE);
      checksum += sendByte(FRAME_ID);

      for (int i = 0; i < 8; i++) {
        checksum += sendByte(endDeviceAddress[i]);
      }
      checksum += sendByte(0xFF);
      checksum += sendByte(0xFE);
      checksum += sendByte(BROADCAST_RADIUS);
      checksum += sendByte(OPTIONS);
      
      for (int i = 0; i < payloadSize; i++) {
        checksum += sendByte(payload[i]);
      }
      checksum = 0xFF - checksum;
      sendByte(checksum);
    }

    private:
      Stream* serial;
};

