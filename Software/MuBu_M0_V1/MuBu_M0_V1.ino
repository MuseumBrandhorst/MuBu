#include <elapsedMillis.h>


#include "FileSystem.h"
#include "uid_map.h"
#include "rfid.h"
#include "music_player.h"
#include "leds.h"
#include "audio_file.h"
#include "button.h"


#include "mubu.h"

RFID_READING last_reading;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hi, I am MuBu!");

  button.init();

  init_rfid();
  init_audio(); // <= SD init
  init_leds();

  files.init();
  Serial.println("done");
}

void loop(void) {
  if (button.check()) 
    MuBu.button_press();
  if (button.check_hold()) {
    Serial.println("SAVE LOG TO SD");
    MuBu.save_log();
  }

  MuBu.run();
}
