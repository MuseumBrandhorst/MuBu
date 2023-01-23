struct BUTTON {
  int pin;
  bool state;
  long time;

  void init() {
    pinMode(pin,INPUT_PULLUP);  
  }

  bool check () {
    bool s = digitalRead(pin)==LOW;
    if (millis()-time>100 && s && !state) {
      state = s;
      time = millis();
      return true;
    }
    state = s;
    return false;
  }

  bool check_hold () {
    bool s = digitalRead(pin)==LOW;
    if (millis()-time>3000 && s && state) {
      state = false;
      time = millis();
      return true;
    }
    return false;
  }
};

BUTTON button = {A5};
