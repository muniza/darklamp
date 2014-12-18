#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT  6
#define LIGHT_PIN    A0

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

volatile int state = 0; //dimmer=0 and rainbow=1
volatile int val = 0; //raw value from light sensor
volatile int scale = 0; //scaled value for dimmer (brightest when val is low)
volatile int time = 0; //time for rainbow state

void setup() {
  //set clock speed to max
  CLKPR = (1<<CLKPCE);
  CLKPR = 0;
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  //read light pin and scale
  val = analogRead(LIGHT_PIN);
  scale = 255 - ((((float)val) * 2 * 255) / 1023);
  
  //in case scale goes negative
  if (scale < 0) {
    scale = 0;
  }
  
  //activate rainbow when scale is high
  if (scale > 120) {
    time++;
  }
  if (time == 5000) {
    state = 1;
  }
  
  //play triggered light show
  startShow(state);
}

void startShow(int i) {
  switch(i){
    case 0: strip.setPixelColor(0, strip.Color(scale, 0, 0));    // Default state = dimmer
            strip.setPixelColor(1, strip.Color(0, scale, 0));
            strip.setPixelColor(2, strip.Color(0, 0, scale));
            strip.setPixelColor(3, strip.Color(scale, scale, 0));
            strip.setPixelColor(4, strip.Color(0, scale, scale));
            strip.setPixelColor(5, strip.Color(scale, 0, scale));
            strip.show();
            break;
    case 1: rainbowCycle(5);  // Rainbow
            break;           
  }
}

//rainbow show
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
  time = 0;
  state = 0;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
