/*
 * MUBU 
 * Interactive Audio Player
 * 
 * MUBU is an audio player which plays audio files based on a selected category and scanned RFID tags.
 * Each tag can thus be associated with one or multiple audio files.
 * 
 * MUBU has a single button user interface. Via button presses users can cycle through categories.
 * Audio files are started when the associated tags are scanned.
 * 
 * MUBU indicates the system state via a RGB LED ring.
 * 
 * 
 * The circuit:
 * - Adafruit Feather M0 Basic Proto
 * - Adafruit Music Maker FeatherWing
 * - PN532-MOD (over I2C)
 * - IMU
 * - NeoPixel Ring - 16
 * 
 * 
 * Created 23 01 2023
 *  By Beat Rossmy
 *
 *  http://...
 */

//#define DEBUG 1

String file_path_buffer;

#include <elapsedMillis.h>


#include "rfid.h"
#include "file_searcher.h"
#include "music_player.h"
#include "leds.h"
#include "button.h"

int category = 1;
RGB colors [4] = {{255,255,255},{200,50,10},{10,200,50},{50,10,200}};  

BUTTON button;
MUSIC_FEATHER audio;
RFID_READER rfid;
LED_ANIMATION led_ring;

void press () {
  category = (category%3)+1;
  rfid.reset();
  Serial.println("press");
}

void play_tag (RFID_READING reading) {
  audio.stop();
  bool started = false;
  file_path_buffer = "/";
  started = search(SD.open("/"),reading.id,0,file_path_buffer);
  if (!started) {
    file_path_buffer = "/";
    started = search(SD.open("/"),reading.id,category,file_path_buffer);
  }
  if (started) {
    Serial.println(file_path_buffer);
    audio.play(file_path_buffer);
    led_ring.set_animation(CIRCLE,colors[category],colors[0]);
  }
}

void audio_end () {
  led_ring.set_animation(BREATH,colors[0],colors[0]);
  rfid.reset();
}

void setup(void) {
  Serial.begin(115200);

  file_path_buffer.reserve(100);

  #ifdef DEBUG
  delay(5000);
  Serial.println("MuBu V1.1");
  Serial.println("Hi, I am MuBu!");
  #endif

  // Init SD 
  init_SD();

  // Init Button
  button.init(A5);
  button.setHandlePress(&press);

  // PN532 RFID/NFC Reader
  rfid.init();
  rfid.setHandleNewTag(&play_tag);
  rfid.setHandleLastTag(&play_tag);

  // Music Feather
  audio.init();
  audio.handleEnd(&audio_end);

  // main UI element
  led_ring.init();
}

void loop(void) {
  // check if button is pressed, then execute callback
  button.run();
  // refresh RFID reader and execute callbacks id new cards are found
  rfid.run();
  // check if audio is stopped
  audio.run();
  // update leds
  led_ring.run();
}
