/*-------------------------------------------------------------------------
Spark Core library to control WS2811/WS2812 based RGB
LED devices such as Adafruit NeoPixel strips.
Currently handles 800 KHz and 400kHz bitstream on Spark Core,
WS2812, WS2812B and WS2811.
PLEASE NOTE that the NeoPixels require 5V level inputs
and the Spark Core only has 3.3V level outputs. Level shifting is
necessary, but will require a fast device such as one of the following:

[SN74HCT245N]
http://www.digikey.com/product-detail/en/SN74HCT245N/296-1612-5-ND/277258

[TXB0108PWR]
http://www.digikey.com/product-search/en?pv7=2&k=TXB0108PWR

If you have a Spark Shield Shield, the TXB0108PWR 3.3V to 5V level
shifter is built in.

Written by Phil Burgess / Paint Your Dragon for Adafruit Industries.
Modified to work with Spark Core by Technobly.
Contributions by PJRC and other members of the open source community.

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing products
from Adafruit!
--------------------------------------------------------------------*/

/* ======================= Includes ================================= */

#include <application.h>

#include "Spark_NeoPixel.h"

/* ======================= Prototype Defs =========================== */

void colorAll(uint32_t c, uint8_t wait);
void colorAllFade(uint32_t wait1,uint32_t wait2);
void colorAll(uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void flash(uint16_t wait);
uint32_t Wheel(byte WheelPos);
int setMode(String command);
int setAllColor(String command);
int setSingleColor(String command);


/* ======================= Spark_StrandTest.cpp ===================== */

#define PIN D7
#define LEDS 25
#define RAINBOW 0
#define FIX_COLOR 1
#define SINGLE_COLOR 2
#define ALL_COLOR 5
#define FLASHING 3
#define RAINBOWC 4
#define ALARM 6
#define RAINBOWC2 7
#define ROUND 8
#define HP 9


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// note: if not specified, D2 is selected for you.
// Parameter 3 = pixel type [ WS2812, WS2812B, WS2811 ]
// note: if not specified, WS2812B is selected for you.
// note: RGB order is automatically applied to WS2811,
// WS2812/WS2812B is GRB order.
//
// 800 KHz bitstream 800 KHz bitstream (most NeoPixel products ...
// ... WS2812 (6-pin part)/WS2812B (4-pin part) )
//
// 400 KHz bitstream (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)




Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN, WS2812B);
int mode = RAINBOWC2;
uint32_t color;
uint32_t colors[LEDS] = {NULL};
uint32_t colors2[LEDS] = {NULL};
uint32_t iColor[LEDS][3]; 
uint16_t led_start = 0;
uint8_t iBrightness[LEDS];
boolean iBlink[LEDS];


// Gamma correction improves appearance of midrange colors
const uint8_t gamma8[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
      3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
      7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
     13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
     20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
     30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
     42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
     58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
     76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
     97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
    122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
    150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
    182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
    218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
};

const uint8_t hex[]  = { 0,1,3,4,6,7,9,11,12,14,15,17,19,21,23,24 };

const uint8_t pent[] = { 2,5,8,10,13,16,18,20,22 };

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel. Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
    
    Spark.function("mode", setMode);
    Spark.function("single", setSingleColor);
    Spark.function("full", setAllColor);
    
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    
    color = strip.Color(0,0,0);
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        colors[i] = strip.Color(0,0,0);
        colors2[i] = strip.Color(0,0,0);
    }
    
   
   setLevel(0,strip.Color(255,255,255));
   setLevel(1,strip.Color(255,0,0));
   setLevel(2,strip.Color(0,255,0));
   setLevel(3,strip.Color(0,0,255));
   strip.show();
   delay(10000);
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  // Do not run more than one of these at a time, or the b/g tasks
  // will be blocked.
  //--------------------------------------------------------------
  //strip.setPixelColor(0, strip.Color(255, 0, 255));
  //strip.show();
  
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  
  //rainbow(20);
  
    if (mode==RAINBOW) {
        rainbow(20);
    }
    else if (mode==RAINBOWC) {
        rainbowCycle(20);
    }
    else if (mode==FLASHING) {
        flash(250);
    }
    else if (mode==FIX_COLOR) {
         colorWipe(strip.Color(0,0,0),50);
         led_start = (led_start + 1) % LEDS;
         colorWipe(color,50);
    }
    else if (mode == SINGLE_COLOR) {
        colorAll(50);
    }
    else if (mode == ALL_COLOR) {
        colorAllFade(1000,500);
    }
    else if (mode == RAINBOWC2) {
        rainbowCycle2(20);
    }
    else if (mode == ALARM) {
        for (uint32_t j = 40; j>5; j-=5) {
            color_alarm(j, strip.Color(255,255,255));
        }
        for (uint32_t j = 4; j>1; j-=1) {
            color_alarm(j, strip.Color(255,255,255));
        }
        for (uint32_t j = 0; j<=20; j++) {
            color_alarm(1, strip.Color(255,255,255));
        }
        for (uint32_t j = 0; j<=5; j++) {
            color_alarm(j, strip.Color(255,255,255));
        }
       for (uint32_t j = 0; j<=40; j+=5) {
            color_alarm(j, strip.Color(255,255,255));
        }
    }
    else if (mode == ROUND) {
        roundCycle(1000);
    }
    else if (mode == HP) {
        hexpent(500);
    }
    else
    {
        colorWipe(strip.Color(255, 0, 0), 50);
        colorWipe(strip.Color(0, 255, 0), 50); 
        colorWipe(strip.Color(0, 0, 255), 50);
    }
}

void setLevel(int16_t level, uint32_t color) {
    if (level==0) {
        strip.setPixelColor(24,color);
    }
    if (level==1) {
        for (uint16_t i=18;i<24;i++)
        strip.setPixelColor(i,color);
    }
    if (level==2) {
        for (uint16_t i=9;i<18;i++)
        strip.setPixelColor(i,color);
    }
    if (level==3) {
        for (uint16_t i=0;i<9;i++)
        strip.setPixelColor(i,color);
    }
}


void color_alarm(uint32_t wait, uint32_t color){
    uint16_t i,k;
    for(i=0; i<strip.numPixels(); i++) {
        for (k=0; k<strip.numPixels(); k++) {
           if (i==k) {
               strip.setPixelColor(k,color);
           } else {
               strip.setPixelColor(k,0,0,0);
           }
        }
        strip.show();
        delay(wait);
    }
    
    for(; i>0; --i) {
        for (k=0; k<strip.numPixels(); k++) {
           if (i==k) {
               strip.setPixelColor(k,color);
           } else {
               strip.setPixelColor(k,0,0,0);
           }
        }
        strip.show();
        delay(wait);
    }
}

int setMode(String command)
{
    if (command=="RAINBOW0") {
        mode = RAINBOW;
    }
    else if (command=="RAINBOW1") {
        mode = RAINBOWC;
    }
    else if (command=="FLASH") {
        mode = FLASHING;
    }
    else if (command=="RAINBOW2") {
        mode = RAINBOWC2;
    }
    else if (command=="ALARM") {
        mode = ALARM;
    }
    else if (command=="ROUND") {
        mode = ROUND;
    }
    else if (command=="HEXPENT") {
        mode = HP;
    }
    else
    {
        return -1;
    }
    
    return 1;
}


void splitArgStringToArray(String arguments, String *target){
    int numArgs = 0;
    int beginIdx = 0;
    int idx = arguments.indexOf(";");
    
    while (idx != -1) {
    	String arg = arguments.substring(beginIdx, idx);
    	arg.trim();
    	target[numArgs] = arg;
    
    	beginIdx = idx + 1;
    	idx = arguments.indexOf(";", beginIdx);
    	++numArgs;
    }
    
    // Single or last parameter
    String lastArg = arguments.substring(beginIdx);
    target[numArgs] = lastArg;
}



int setAllColor(String command) {
   mode = ALL_COLOR;
   
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        iBlink[i] = false;
        iBrightness[i] = 255;
        if ( i<command.length()) {
            String val = command.substring(i,i+1);
            if (val=="R") {
                iColor[i][0] = 255; iColor[i][1] = 0; iColor[i][2] = 0;
            }
            else if (val=="O") {
                iColor[i][0] = 255; iColor[i][1] = 160; iColor[i][2] = 0;
            }
            else if (val=="G") {
                iColor[i][0] = 0; iColor[i][1] = 255; iColor[i][2] = 0;
            }
            else if (val=="r") {
                iColor[i][0] = 255; iColor[i][1] = 0; iColor[i][2] = 0;
                iBlink[i] = true;
            }
            else if (val=="o") {
                iColor[i][0] = 255; iColor[i][1] = 160; iColor[i][2] = 0;
                iBlink[i] = true;
            }
            else if (val=="g") {
                iColor[i][0] = 0; iColor[i][1] = 255; iColor[i][2] = 0;
                iBlink[i] = true;
            }
            else {
                iColor[i][0] = 0; iColor[i][1] = 0; iColor[i][2] = 0;
            }
        }
        else {
            iColor[i][0] = 0; iColor[i][1] = 0; iColor[i][2] = 0;
        }
    }
   
   return command.length();
}


int setSingleColor(String command) {
    mode = SINGLE_COLOR;
    
   String data[4] = {NULL};
   splitArgStringToArray(command, data);
   
   int r = data[0].toInt();
   int g = data[1].toInt();
   int b = data[2].toInt();
   int pixel = data[3].toInt();
   if (pixel<0||pixel>LEDS) return -2;
   if (r<0||r>255) return -3;
   if (g<0||g>255) return -4;
   if (b<0||b>255) return -5;
   colors[pixel] = strip.Color(r,g,b);
   
   return 1;
}

// Set all pixels in the strip to a solid color, then wait (ms)
void colorAll(uint8_t wait) {
  uint16_t i;
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, colors[i]);
  }
  strip.show();
  delay(wait);
}


// Set all pixels in the strip to a solid color, then wait (ms)
void colorAll(uint32_t c, uint8_t wait) {
  uint16_t i;
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

// Set all pixels in the strip to a solid color, then wait (ms)
void colorAllFade(uint32_t wait1,uint32_t wait2) {
  uint16_t i,j;
  for(j=50; j<255; j+=5) {
      for(i=0; i<strip.numPixels(); i++) {
          if (iBlink[i]) {
              iBrightness[i] = j;
          }
      }
      delay(20);
      setColorWithBrightness();
  }
  delay(wait1);
  for(j=0; j<205; j+=5) {
      for(i=0; i<strip.numPixels(); i++) {
          if (iBlink[i]) {
              iBrightness[i] = 255-j;
          }
      }
      delay(20);
      setColorWithBrightness();
  }
  //delay(wait2);
}

void setColorWithBrightness() {
  uint8_t i, r, g, b, a;
      // Merge iColor with iBrightness, issue to NeoPixels
  for(i=0; i<LEDS; i++) {
    a = iBrightness[i] + 1;
    // First eye
    r = iColor[i][0];            // Initial background RGB color
    g = iColor[i][1];
    b = iColor[i][2];
    if(a) {
      r = (r * a) >> 8;          // Scale by brightness map
      g = (g * a) >> 8;
      b = (b * a) >> 8;
    }
    strip.setPixelColor(i, gamma8[r], gamma8[g], gamma8[b]);
  }
  strip.show();
}

// Fill the dots one after the other with a color, wait (ms) after each one
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=led_start; i<(strip.numPixels()+led_start); i++) {
      strip.setPixelColor(i%LEDS, c);
      strip.show();
      delay(wait);
  }
}

void flash(uint16_t wait) {
  uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i,strip.Color(0,0,0));
    }
    strip.show();
    delay(wait);
    
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i,strip.Color(255,255,255));
    }
    strip.show();
    delay(wait);
  
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



void hexpent(uint32_t wait) {
    uint16_t i;
    for(i=0;i<16;i++) {
        strip.setPixelColor(hex[i],0,0,255);
    }
    for(i=0;i<9;i++) {
        strip.setPixelColor(pent[i],0,0,0);
    }
    strip.show();
    delay(wait);
    for(i=0;i<9;i++) {
        strip.setPixelColor(pent[i],0,0,255);
    }
    for(i=0;i<16;i++) {
        strip.setPixelColor(hex[i],0,0,0);
    }
    strip.show();
    delay(wait);
}

void roundCycle(uint32_t wait) {
    uint16_t i, j;
    strip.setPixelColor(24,0,0,0);
    for (j=0; j<18;j++) {
        for(i=0; i<24; i++) {
            if (i<9                  &&          j/2==i) {
                strip.setPixelColor(i,255,255,255);
            }  else if (i>=9 && i<18 && ((j+1)%18)/2==i-9) {
                strip.setPixelColor(i,255,244,255);
            } else if (i>=18         && ((j+2)%18)/3==i-18) {
                strip.setPixelColor(i,255,255,255);
            } else {
                strip.setPixelColor(i,0,0,0);
            }
        }
        strip.show();
        delay(wait);
    }    
}



// Slightly different, this makes the rainbow equally distributed throughout, then wait (ms)
void rainbowCycle2(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< 9; i++) {
      strip.setPixelColor(i, Wheel(((i%9 * 256 / 9) + j) & 255));
    }
    for(i=9; i< 18; i++) {
      strip.setPixelColor(i, Wheel(((i%9 * 256 / 9) + j) & 255));
    }
    for(i=18; i< 24; i++) {
      strip.setPixelColor(i, Wheel(((i%6 * 256 / 6) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


// Slightly different, this makes the rainbow equally distributed throughout, then wait (ms)
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
