#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

// These are the pins used
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)
#define VS1053_CS       6     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define CARDCS          5     // Card chip select pin
#define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

void init_SD () {
  Serial.println("connect to SD card ...");
  while (!SD.begin(CARDCS)) {
    Serial.println("...");
    delay(500);
  }
  Serial.println("SD OK!");
}

class MUSIC_FEATHER {
  private:
  Adafruit_VS1053_FilePlayer player =
    Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);
  elapsedMillis timer;
  bool state = false;
  void (*on_start) ();
  void (*on_end) ();
    
  public:
  
  void init () {
    Serial.println("connect to VS1053 ...");
    while (!player.begin()) {
      Serial.println("...");
      delay(500);
    }
    Serial.println("VS1053 OK!");
    
    // Set volume for left, right channels. lower numbers == louder volume!
    player.setVolume(10,10);
    
    // If DREQ is on an interrupt pin we can do background audio playing
    player.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  }

  /*bool play (uint8_t folder, uint8_t file) {
    if (isPlaying())
      player.stopPlaying();
    TRACK * t = tracks.get(folder,file);
    if (t==NULL) return false;
    String n = tracks.get(folder,file)->name();
    player.startPlayingFile(n.c_str());
    bool started = isPlaying();
    if (started && on_start!=NULL) on_start();
    return started;
  }*/

  void stop () {
    player.stopPlaying();
  }

  void play (String path) {
    player.startPlayingFile(path.c_str());
    bool started = isPlaying();
    if (started && on_start!=NULL) on_start();
  }

  bool isPlaying () {
    return player.playingMusic; 
  }

  void handleStart (void (*func)()) {
    on_start = func;  
  }

  void handleEnd (void (*func)()) {
    on_end = func;  
  }

  void run () {
    if (timer>250) {
      timer = 0;
      if (state==true && isPlaying()==false && on_end!=NULL) on_end();
      state = isPlaying();
    }
  }
};


/*
bool play (int id, int cat, MUSIC_FEATHER * out) {
  File dir = SD.open("/");
  while (true) {
    File folder =  dir.openNextFile();
    if (! folder) break;
    if (folder.isDirectory()) {
      String folder_name = folder.name();
      folder_name.toLowerCase();
      if (folder_name.toInt()==id) {
        while (true) {
          File sub_folder = folder.openNextFile();
          if (! sub_folder) break;
          if (sub_folder.isDirectory()) {
            String sub_folder_name = sub_folder.name();
            sub_folder_name.toLowerCase();
            if (sub_folder_name.toInt()==cat) {
              File audio_file = sub_folder.openNextFile();
              String file_name = audio_file.name();
              file_name.toLowerCase();
              String path = "/";
              path += folder_name;
              path += "/";
              path += sub_folder_name;
              path += "/";
              path += file_name;
              out->play(path);
              audio_file.close();
              sub_folder.close();
              folder.close();
              return true;
            }
          }
          sub_folder.close();
        }
      }  
    }
    folder.close();
  }
  dir.close();
  return false;
}*/
