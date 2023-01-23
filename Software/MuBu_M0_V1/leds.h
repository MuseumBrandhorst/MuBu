#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define PIN        11
#define NUMPIXELS  16

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

elapsedMillis led_timer;

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

RGB led_buffer [NUMPIXELS];

void init_leds () {
  pixels.begin();
  pixels.setBrightness(255);
}

bool run_leds () {
  if (led_timer>25) {
    led_timer = 0;
    return true;  
  }
  return false;
}

void clear_led_buffer () {
  for (int i=0; i<NUMPIXELS; i++)
    led_buffer[i] = {0,0,0};  
}

void update_leds () {
  pixels.clear();
  for (int p=0; p<NUMPIXELS; p++) pixels.setPixelColor(p, led_buffer[p].get());
  pixels.show();  
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
