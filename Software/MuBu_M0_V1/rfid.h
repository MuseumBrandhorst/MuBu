#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (0)
#define PN532_RESET (1)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void init_rfid () {
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    //Serial.print("Didn't find PN53x board");
    //while (1); // halt
    Serial.print("Looking for PN53x board");
    while (!versiondata) {
      delay(500);
      versiondata = nfc.getFirmwareVersion();
      Serial.print("*");
    }
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
}

enum RFID_STATE {NO_CARD,CARD};

struct RFID_READING {
  RFID_STATE state = NO_CARD;
  unsigned long time;
  int uid = 0;
  int id = 0;

  void set (RFID_READING s) {
    state = s.state;
    time = s.time;
    uid = s.uid;
    id = s.id;  
  }
};

RFID_READING read_rfid () {
  RFID_READING reading;
  uint8_t uid_buf[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;
  bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid_buf[0], &uidLength, 50); // <= timeout
  if (success) {
    reading.state = CARD;
    for (uint8_t i=0; i < uidLength; i++) {
      reading.uid = (reading.uid<<8)|uid_buf[i];
    }
    reading.id = uid_map.get(reading.uid);
    reading.time = millis();
  }
  return reading;
}

elapsedMillis rfid_timer;

bool run_rfid () {
  if (rfid_timer>100) {
    rfid_timer = 0;
    return true;  
  }  
  return false;
}
