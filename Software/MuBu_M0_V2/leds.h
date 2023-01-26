#include <Adafruit_NeoPixel.h>
#define PIN        11
#define NUMPIXELS  16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

struct RGB {
  float r,g,b;

  void add (RGB c) {
    r = r + c.r;
    if (r>255) r = 255;  
    g = g + c.g;
    if (g>255) g = 255;
    b = b + c.b;
    if (b>255) b = 255;
  }

  void dim (float v) {
    r *= v;
    if (r>255) r = 255; 
    g *= v;
    if (g>255) g = 255; 
    b *= v;
    if (b>255) b = 255; 
  }

  int get () {
    return pixels.Color(r,g,b);  
  }

  RGB get (float v) {
    return {r*v,g*v,b*v};  
  }
};

enum ANIMATION_TYPE {NONE,BREATH,CIRCLE};

class LED_ANIMATION {
  private:
  //static const NUMPIXELS = 16;
  RGB led_buffer [NUMPIXELS];
  Adafruit_NeoPixel * pix = &pixels;
  elapsedMillis timer;

  RGB main = {255,255,255};
  RGB base = {255,255,255};
  ANIMATION_TYPE mode = BREATH;

  public:
  void init () {
    pix->begin();
    pix->setBrightness(64);
    update();
    #ifdef DEBUG
    Serial.println("NeoPixel init.");
    #endif
  }

  void set_animation (ANIMATION_TYPE m, RGB c1, RGB c2) {
    main = c1;
    base = c2;
    mode = m;  
  }
  
  void breathing (unsigned long t, RGB base, RGB main) {
    float v = (sin((float(t)/1000.0))+1.0);
    v = (v*v*v*v)/16.0;
    RGB col = main.get(v);
    col.add(base);
    for (int i=0; i<NUMPIXELS; i++)
      led_buffer[i].add(col);
  }
  
  void playing (unsigned long t, RGB base, RGB main) {
    for (int i=0; i<NUMPIXELS; i++) {
      float phase = float(i)/float(NUMPIXELS)*PI*2.0;
      float v = (sin(phase+(float(t)/300.0))+1.0);
      v = (v*v*v*v)/16.0;
      RGB col = main.get(v);
      col.add(base);
      led_buffer[i].add(col);
    }
  }

  void run () {
    if (timer>25) {
      timer = 0;
      clear();
      if (mode==BREATH) breathing(millis(),base.get(0.01),main);
      else if (mode==CIRCLE) playing(millis(),base.get(0.01),main);
      update();
    }
  }

  void clear () {
    for (int i=0; i<NUMPIXELS; i++)
      led_buffer[i] = {0,0,0};  
  }
  
  void update () {
    pix->clear();
    for (int p=0; p<NUMPIXELS; p++) pix->setPixelColor(p, led_buffer[p].get());
    pix->show();  
  }
};
