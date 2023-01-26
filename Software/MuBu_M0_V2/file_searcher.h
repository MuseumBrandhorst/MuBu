/*
   MUBU expects to find MP3 files organized in the following structure.
   On the SD card the audiofiles associated with one card ID are organized in folders, which names start with "ID_..." (e.g.: "1_intro").
   Within these folders, the files are either directly stored or organized within subfolders.
   Either way file or folder names start with the category number (e.g. "1_...").
   
   The category 0 is reserved for special audio files which should be played inependently from the
   currently selected category.

   XXX stand for the ID associated with specific RFID tags and is maualy set (see UID_MAP)

   The path to the audio file for ID 4 and category 2 looks like:

   SD CARD
   │
   └───folder "1_intro"
   | . |
   |   └───folder "1"
   |       track1.mp3
   |       track2.mp3
   |       track3.mp3
   │   2_story.mp3
   │   3_soundscape.mp3
   │   
   |   ...
   │
   └───folder "2_imageXY"
   |   track003.mp3
   │   track007.mp3
   |   ...
   . ...
*/

#include <SD.h>

bool starts_with_id (File e, int id) {
  String e_name = e.name();
  return (e_name.toInt()==id);
}

bool select_random_file_from_dierctory (File dir, String & path) {
  int entry_count = 0;
  while (true) {
    File entry = dir.openNextFile();
    if (! entry) break;
    entry_count++;
    entry.close();
  }
  int i = random(entry_count-1);
  entry_count = 0;
  dir.rewindDirectory();
  while (true) {
    File entry = dir.openNextFile();
    if (! entry) break;
    if (i==entry_count) {
      path = path + entry.name();
      return true;
    }
    entry_count++;
    entry.close();
  }
  return false;
}

bool search (File dir, int id, int cat, String & path) {
  bool state = false;
  // final level: take random file
  if (id==-1 && cat==-1) {
    Serial.println("-1 -1");
    state = select_random_file_from_dierctory(dir,path);
  }
  else {
    while (true) {
      File entry = dir.openNextFile();
      if (! entry) break;
      // directory with matching ID found
      if (starts_with_id(entry,id) && entry.isDirectory()==true) {
        // directory found => continue one level deeper
        path = path + entry.name() + "/";
        state = search(entry, cat, -1, path); // <= recursion: shift category to id, set category to -1
      } 
      // file with matching ID found
      else if (starts_with_id(entry,id) && entry.isDirectory()==false) {
        path = path + entry.name();
        return true;
      }
      // final level: take any file
      //else if (id==-1 && cat==-1 && entry.isDirectory()==false) {
      //  path = path + entry.name();
      //  return true;
      //}
      entry.close();
    }
  }
  dir.close();
  return state;
}

/*

String concatenate_path (String m_folder, String s_folder, String f_name) {
  return "/" + m_folder + "/" + s_folder + "/" + f_name;
}

bool look_for_file (int id, int cat, String & path) {
  path = "/";
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
              //path = "/" + folder_name + "/" + sub_folder_name + "/" + file_name;
              path = concatenate_path(folder_name,sub_folder_name,file_name);
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
