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

class XBeeTransmitRequestUtils {

	#define TRANSMIT_FRAME_PAYLOAD_OFFSET 16

	public:
		byte frame[150];
		byte payload[100];
		int payloadIndex;
		int frameIndex;
		bool frameReceived;

    void setSerial(Stream &serialStream) {
      serial = &serialStream;
    }

		void readPacket() {
			while (serial->available()) {
				frame[frameIndex] = serial->read();
				frameIndex++;
			}
		}

		void read() {
			frameReceived = false;
			if (serial->available()) {
				readPacket(1000);
			}
		}

		void readPacket(unsigned long timeout) {
			payloadIndex = 0;
			frameIndex = 0;

			unsigned long start = millis();

			while ((millis() - start) < timeout) {
				readPacket();
			}

			for (int i = TRANSMIT_FRAME_PAYLOAD_OFFSET; i < frameIndex; i++) {
				payload[payloadIndex] = frame[i];
				payloadIndex++;	
			}

			frameReceived = true;
		}

    private:
      Stream* serial;
};















