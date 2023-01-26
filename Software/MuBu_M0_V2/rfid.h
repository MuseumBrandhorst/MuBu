#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include "uid_map.h"

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

class RFID_READER {
  private:
  static const int IRQ_PIN = 0;
  static const int RST_PIN = 1;
  Adafruit_PN532 pn532 = Adafruit_PN532(IRQ_PIN, RST_PIN);
  elapsedMillis timer;

  static const int refresh_rate = 100;
  static const int retrigger_delay = 10000; // time before same tag is repeated

  RFID_READING last_reading;
  void (*on_new_tag)(RFID_READING);
  void (*on_last_tag)(RFID_READING);

  public:
  void init () {
    pn532.begin();
    uint32_t versiondata = pn532.getFirmwareVersion();
    
    if (! versiondata) {
      #ifdef DEBUG
      Serial.print("Looking for PN53x board");
      #endif
      
      while (!versiondata) {
        delay(500);
        pn532.begin();
        versiondata = pn532.getFirmwareVersion();
        #ifdef DEBUG
        Serial.print("...");
        #endif
      }
    }
    
    // Got ok data, print it out!
    #ifdef DEBUG
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    #endif
    
    pn532.setPassiveActivationRetries(0xFF);
    pn532.SAMConfig();  
  }

  void setHandleNewTag (void (*func)(RFID_READING)) {
    on_new_tag = func;
  }

  void setHandleLastTag (void (*func)(RFID_READING)) {
    on_last_tag = func;
  }

  RFID_READING get_last_reading () {
    return last_reading;  
  }

  void read () {
    RFID_READING reading;
    uint8_t uid_buf[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;
    bool success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid_buf[0], &uidLength, 50); // <= timeout
    if (success) {
      reading.state = CARD;
      for (uint8_t i=0; i < uidLength; i++)
        reading.uid = (reading.uid<<8)|uid_buf[i];
      reading.id = uid_map.get(reading.uid);
      reading.time = millis();

      if (reading.uid==last_reading.uid &&
      reading.time > last_reading.time+retrigger_delay &&
      on_last_tag!=NULL) {
        Serial.println("retrigger");
        on_last_tag(reading);
        last_reading = reading;
      }
      if (reading.uid!=last_reading.uid &&
      on_new_tag!=NULL) {
        Serial.println("trigger");
        on_new_tag(reading);
        last_reading = reading;
      }
    }
  }

  bool run () {
    if (timer>refresh_rate) {
      timer = 0;
      read();
    }
    return true;
  }

  void reset () {
    last_reading = {};
  }
};
