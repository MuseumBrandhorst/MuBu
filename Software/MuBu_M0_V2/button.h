class BUTTON {
  private:
  int pin;
  bool state;
  long time;
  void (*on_press) ();
  void (*on_hold) ();

  public:
  void init(int p) {
    pin = p;
    pinMode(pin,INPUT_PULLUP);
    #ifdef DEBUG
    Serial.println("Button init done.");
    #endif
  }

  void setHandlePress (void (*func)()) {
    on_press = func;  
  }

  void run () {
    bool s = digitalRead(pin)==LOW;
    if (millis()-time>100 && s && !state) {
      state = s;
      time = millis();
      if (on_press != NULL) on_press();
    }
    if (millis()-time>3000 && s && state) {
      state = false;
      time = millis();
      if (on_hold!=NULL) on_hold();
    }
    state = s;
  }
};
