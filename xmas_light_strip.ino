#include <Adafruit_NeoPixel.h>

#define PIN 3

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel s = Adafruit_NeoPixel(80, 3, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  s.begin();
  s.show(); // Initialize all pixels to 'off'
  Serial.println(s.numPixels());
  delay(1500); // let things settle
  s.setBrightness(180);
}

void loop() {

  test_pattern(35);

} // main()



void test_pattern(uint32_t wait) {

  uint32_t blank = s.Color(0, 0, 0);
  for (int i = 0; i < s.numPixels(); i++) {
    s.setPixelColor(i, blank);
  }  

  uint32_t c = s.Color(127, 50, 80);
  uint32_t i = s.numPixels() - 1;
  
  while(1) {
    s.setPixelColor(i, c);
  
    s.show();
    delay(wait);  
    i--;
    if(i==-1) {
      i = s.numPixels() - 1;
      c = s.Color(random(0,255), random(0,255), random(0,255));
    }
  }
}
