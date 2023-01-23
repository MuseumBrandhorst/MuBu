#include <SD.h>

template <uint8_t FO_NR, uint8_t FI_NR>
struct FSystem {
  bool folders [FO_NR];
  bool fileStructure [FO_NR][FI_NR];

  void init () {
    for (uint8_t fo = 0; fo < FO_NR; fo++) {
      String fo_name = "";
      fo_name += "/";
      fo_name += fo;
      if (SD.exists(fo_name)) {
        //Serial.println(fo_name);
        folders[fo] = true;
        for (int fi = 0; fi < FI_NR; fi++) {
          String fi_name = "";
          fi_name += fo_name;
          fi_name += "/track0";
          fi_name += ((fi<10)?"0":"");
          fi_name += fi;
          fi_name += ".mp3";
          if (SD.exists(fi_name.c_str())) {
            fileStructure[fo][fi] = true;
            //Serial.println(fi_name);
          }
        }
      }
    }
  }

  bool exists (uint8_t folder, uint8_t file) {
    if (folder>=FO_NR || file>=FI_NR) return false;
    else return fileStructure[folder][file]; 
  }

  String get_name (uint8_t folder, uint8_t file) {
    String n = "";
    n += "/";
    n += folder;
    n += "/";
    n += "track0";
    n += ((file<10)?"0":"");
    n += file;
    n += ".mp3";
    return n;
  }
  
};


FSystem<4, 16> files;
