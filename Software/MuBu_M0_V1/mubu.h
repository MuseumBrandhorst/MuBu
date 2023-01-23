enum MUBU_STATES {
  START_UP,
  WAITING_FOR_SCAN,
  SCANNING,
  NEW_CONTENT_FOUND,
  SAME_CONTENT_FOUND,
  AUDIO_STARTED,
  AUDIO_PLAYING
};

struct MUBU {
  MUBU_STATES state = START_UP;
  long state_time = millis();
  RFID_READING last_reading;

  int category = 1;

  AUDIO_FILE last_file;

  // LOGS
  int category_switch = 0;
  int rfid_scans = 0;
  int wrong_scans = 0;
  int started_files = 0;
  AUDIO_FILE file_logs [100];
  // LOGS

  void button_press () {
    category = (category % 3) + 1;
    // LOG
    category_switch++;
  }

  void set_state (MUBU_STATES s) {
    state = s;
    state_time = millis();
  }

  RGB getColor (int mode) {
    if (mode == 1) return {255, 0, 0};
    if (mode == 2) return {0, 255, 0};
    if (mode == 3) return {0, 0, 255};
    return {255, 255, 255};
  }

  

  void run () {
    unsigned long t = millis() - state_time;

    // STATE MACHINE
    if (state == START_UP && t > 500)
      set_state(WAITING_FOR_SCAN);
    if (state == AUDIO_STARTED && audio.is_playing() && t > 500)
      set_state(AUDIO_PLAYING);
    if (state == AUDIO_PLAYING && audio.is_playing() == false)
      set_state(WAITING_FOR_SCAN);

    // ACTIONs ACCORDING TO STATES
    if (run_rfid()) {
      RFID_READING reading = read_rfid();

      if (reading.state == CARD) {
        
        if (last_file.is_new(category, reading.id) || state == WAITING_FOR_SCAN) {
          Serial.print("ID: ");
          Serial.println(reading.id);
          bool track_started = false;
          if (files.exists(category, reading.id)) {
            Serial.println("File exists in Category!");
            last_file.set(category, reading.id);
            track_started = audio.play(last_file.category, last_file.nr);
          }
          else if (files.exists(0, reading.id)) {
            Serial.println("Fallback File exists!");
            last_file.set(0, reading.id);
            track_started = audio.play(last_file.category, last_file.nr);
          }
          else {
            // wrong scans = already playing
            wrong_scans++;
          }
          if (track_started) {
            set_state(AUDIO_STARTED);
            last_reading.set(reading);
            // LOG
            Serial.println(last_file.category);
            Serial.println(last_file.nr);
            file_logs[started_files].category = last_file.category;
            file_logs[started_files].nr = last_file.nr;
            started_files++;
          }
        }
      }
    }

    // LEDS
    if (run_leds()) {
      unsigned long t = millis() - last_reading.time;
      clear_led_buffer();

      RGB base = getColor(category);
      RGB main = getColor(last_file.category);

      if (state == START_UP || state == WAITING_FOR_SCAN)
        breathing(t, base.get(0.01), base);
      else if (state == AUDIO_STARTED || state == AUDIO_PLAYING)
        playing(t, base.get(0.01), main);

      update_leds();
    }

  }

  void save_log () {
    set_state(WAITING_FOR_SCAN);
    audio.player->stopPlaying();
    delay(100);
    File dataFile = SD.open("logs/datalog.txt", FILE_WRITE);
    if (dataFile) {
      Serial.println("LOG opened");
      dataFile.println("===============");
      
      dataFile.print("category_switch ");
      dataFile.println(category_switch);
      
      dataFile.print("rfid_scans ");
      dataFile.println(rfid_scans);

      dataFile.print("wrong_scans ");
      dataFile.println(wrong_scans);

      dataFile.print("started_files ");
      dataFile.println(started_files);

      dataFile.println("file_logs:");
      for (int i=0; i<100; i++) {
        int c = file_logs[i].category;
        int id = file_logs[i].nr;
        if (!(c==0 && id==0)) {
          dataFile.print(c);
          dataFile.print("/");
          dataFile.println(id);
        }
      }
      
      dataFile.close();

      Serial.println("LOG closed");
    }
  }
};

MUBU MuBu;
