#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>

#define DEBUG_ANYLED_I2C  0

#define HAS_NAV           0

#define ANYLED_I2C_ADDR 0x90
#define NUM_LEDS    3

#if HAS_NAV
uint32_t nav_interval = 800;
uint32_t last_time = 0;

CRGB nav_leds[NUM_LEDS];

void nav_loop();
#endif
CRGB sync_leds[NUM_LEDS];

typedef struct rgb_s{
  uint8_t r;
  uint8_t g;
  uint8_t b;
}rgb_t;

rgb_t rgb;

bool update = false;



void receiveEvent(int x);
void led_setup();
void led_loop();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Wire.setClock(100000);
  Wire.begin(ANYLED_I2C_ADDR);
  Wire.onReceive(receiveEvent);
  memset(&rgb, 0, sizeof(rgb_t));
  
  #if HAS_NAV
    FastLED.addLeds<NEOPIXEL, 9>(nav_leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, 8>(nav_leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, 7>(sync_leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, 6>(sync_leds, NUM_LEDS);
  #else
    FastLED.addLeds<NEOPIXEL, 9>(sync_leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, 8>(sync_leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, 7>(sync_leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, 6>(sync_leds, NUM_LEDS);
  #endif

  led_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  led_loop();
  #if HAS_NAV
  nav_loop();
  #endif
}

void receiveEvent(int x){
  update = true;
  while( Wire.available() > 0)
  {
      rgb.r = Wire.read();
      rgb.g = Wire.read();
      rgb.b = Wire.read();
  }

#if DEBUG_ANYLED_I2C
  Serial.print("r: ");
  Serial.print(rgb.r, HEX);
  Serial.print(" g: ");
  Serial.print(rgb.g, HEX);
  Serial.print(" b: ");
  Serial.println(rgb.b, HEX);
#endif
}

void led_setup(){
  for(uint8_t i=0; i<NUM_LEDS; i++){
    #if HAS_NAV
      nav_leds[i] = CRGB::Red;
      sync_leds[i] = CRGB::Blue;
      FastLED.show();
      nav_leds[i] = CRGB::Black;
      sync_leds[i] = CRGB::Black;
      delay(20);
    #else
      sync_leds[i] = CRGB::Red;
      FastLED.show();
      sync_leds[i] = CRGB::Black;
      delay(20);
    #endif
  }
}

void led_loop(){
  if(update){
    update = false;
    for(uint8_t i=0; i<NUM_LEDS; i++){
      sync_leds[i] = CRGB(rgb.r, rgb.g, rgb.b);
      FastLED.show();
    }
  }else{
    FastLED.show();
  }

}

#if HAS_NAV
void nav_loop(){
    uint32_t now = millis();
    if(now -last_time <= nav_interval){
      for(uint8_t i=0; i<NUM_LEDS; i++){
        nav_leds[i] = CRGB::Red;
        FastLED.show();
      }
    }else if(now - last_time > nav_interval && now - last_time <= 2*nav_interval){
      for(uint8_t i=0; i<NUM_LEDS; i++){
        nav_leds[i] = CRGB::Black;
        FastLED.show();
      }
    }else{
      last_time = now;
    }
}

#endif