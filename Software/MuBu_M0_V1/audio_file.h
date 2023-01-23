struct AUDIO_FILE {
  uint8_t category = 0;
  uint8_t nr = 0;

  bool is_new (int c, int n) {
    return (category!=c) || (nr!=n);
  }

  void set (uint8_t c, uint8_t n) {
    category = c;
    nr = n;
  }
  
  const char * get_file_name () {
    String filepath = "/";
    filepath += category;
    filepath += "/";
    filepath += "track00";
    filepath += nr;
    filepath += ".mp3";
    return filepath.c_str();  
  }  
};
