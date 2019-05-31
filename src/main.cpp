#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>

#define ANYLED_I2C_ADDR 0x90
#define NUM_LEDS    3

CRGB nav_leds[NUM_LEDS];

typedef struct rgb_s{
  uint8_t r;
  uint8_t g;
  uint8_t b;
}rgb_t;

rgb_t rgb;

bool update = false;

void receiveEvent(int x);
void requestEvent();
void led_loop();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Wire.setClock(100000);
  Wire.begin(ANYLED_I2C_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  memset(&rgb, 0, sizeof(rgb_t));
  FastLED.addLeds<NEOPIXEL, 9>(nav_leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 8>(nav_leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 7>(nav_leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 6>(nav_leds, NUM_LEDS);
}

void loop() {
  // put your main code here, to run repeatedly:
  led_loop();
}

void receiveEvent(int x){
  update = true;
  while( Wire.available() > 0) // 循环执行，直到数据包只剩下最后一个字符
  {
      rgb.r = Wire.read();
      rgb.g = Wire.read();
      rgb.b = Wire.read();
  }

  Serial.print("r: ");
  Serial.print(rgb.r, HEX);
  Serial.print(" g: ");
  Serial.print(rgb.g, HEX);
  Serial.print(" b: ");
  Serial.println(rgb.b, HEX);
}

//当主机通知从机上传数据，执行该事件
void requestEvent()
{
  Wire.write("hello");
}

void led_loop(){
  if(update){
    update = false;
    for(uint8_t i=0; i<NUM_LEDS; i++){
      nav_leds[i] = CRGB(rgb.r, rgb.g, rgb.b);
      FastLED.show();
    }
  }else{
    FastLED.show();
  }

}