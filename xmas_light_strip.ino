#include <Adafruit_NeoPixel.h>

#define PIN 3
#define PIXEL_CNT 80
#define BRIGHTNESS 180

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel s = Adafruit_NeoPixel(PIXEL_CNT, 3, NEO_RGB + NEO_KHZ800);


struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

#define COLOR_SET_CNT    6
#define INTENSITY_CNT    10


RGB buf[PIXEL_CNT]; // internal buffer
RGB color_set[INTENSITY_CNT][COLOR_SET_CNT];

void setup() {
  Serial.begin(9600);
  s.begin();
  clear_strip();
  s.show(); // Initialize all pixels to 'off'
  Serial.print(F("Number of pixels: "));
  Serial.println(s.numPixels());
  delay(500); // let things settle
  s.setBrightness(BRIGHTNESS);
  define_color_sets();

}

void loop() {

  effect_check(1);
  clear_strip_delay();

  while (1) {
    effect_fade(15, 0);
    clear_strip_delay();

    effect_red_green(15);
    clear_strip_delay();

    effect_fade(15, 1);
    clear_strip_delay();

    effect_sparkle(15);
    clear_strip_delay();

    effect_fade(15, 2);
    clear_strip_delay();
  }
} // main()


void clear_strip_delay() {
  clear_strip();
  s.show();
  delay(1000);
} // clear_strip_delay()


void preset_5_colors_random(int intensity, bool really_random) {
  static bool done = false;
  if (really_random) {
    done = false; //override the static setting of this from a previous run
  }
  static byte pidx[PIXEL_CNT]; // color_set index for each pixel in strip
  if (!done) {
    for (uint16_t i = 0; i < PIXEL_CNT; i++) {
      pidx[i] = random(COLOR_SET_CNT - 1);
      if (!really_random) {
        done = true;  // not setting this flag results in really fun and random colors!
      }
    }
  }
  uint16_t i = 0;
  while (i < PIXEL_CNT) {
    buf[i].r = color_set[intensity][pidx[i]].r;
    buf[i].g = color_set[intensity][pidx[i]].g;
    buf[i].b = color_set[intensity][pidx[i]].b;
    i++;
  }
} // preset_5_colors_random()


void preset_5_colors(int intensity) {
  uint16_t i = 0;
  while (i < PIXEL_CNT) {
    for (uint16_t j = 0; j < 5; j++) {
      buf[i].r = color_set[intensity][j].r;
      buf[i].g = color_set[intensity][j].g;
      buf[i].b = color_set[intensity][j].b;
      i++;
    }
  }
} // preset_5_colors()


void effect_sparkle(unsigned long run_time_sec) {
  static const int CNT = 5;
  static const int EFFECT_DELAY = 1000;
  RGB val[CNT];

  // preset pixel values
  val[0] = {255, 255, 255};
  val[1] = {190, 190, 190};
  val[2] = {130, 130, 130};
  val[3] = {110, 110, 110};
  val[4] = {25, 25, 25};

  // start with a random baseline
  int r;
  for (uint16_t i = 0; i < PIXEL_CNT; i++) {
    r = random(CNT);
    s.setPixelColor(i, s.Color(val[r].r, val[r].g, val[r].b));
  }
  s.show();
  delay(EFFECT_DELAY);

  // continually pick N (5) pixels to replace with random value

  unsigned long start_time = millis();
  unsigned long run_time_msec = run_time_sec * 1000;

  uint16_t rpos; // random pixel position
  while (millis() - start_time < run_time_msec) {
    for (uint16_t i = 0; i < 5; i++) {
      rpos = random(PIXEL_CNT);
      r = random(CNT);
      s.setPixelColor(rpos, s.Color(val[r].r, val[r].g, val[r].b));
    }
    s.show();
    delay(EFFECT_DELAY);
  }
} // effect_sparkle()


// old school switch between red and green
void effect_red_green(unsigned long run_time_sec) {
  unsigned long start_time = millis();
  unsigned long run_time_msec = run_time_sec * 1000;
   
  while (millis() - start_time < run_time_msec) {
    // red-green ... fill green then populate reds
    s.fill(s.Color(0, 255, 0), 0, 0);
    for (int i = 0; i < PIXEL_CNT; i++) {
      if ( ((i % 10) < 5)) {
        s.setPixelColor(i, s.Color(255, 0, 0));
      }
    }
    s.show();
    delay(500 + random(500));

    // green-red ... fill red then populate greens
    s.fill(s.Color(255, 0, 0), 0, 0);
    for (int i = 0; i < PIXEL_CNT; i++) {
      if ( ((i % 10) < 5)) {
        s.setPixelColor(i, s.Color(0, 255, 0));
      }
    }
    s.show();
    delay(500 + random(500));
  }
} // effect_red_green()


// sweep through the strand back and forth
void effect_check(int repeat_cnt) {
  int reps = 0;
  while (reps++ < repeat_cnt) {
    // going up
    for (int i = 0; i < PIXEL_CNT; i++) {
      clear_strip();
      s.setPixelColor(i, s.Color(255, 255, 255));
      s.show();
      delay(25);
    }
    // going down
    for (int i = PIXEL_CNT - 1; i > -1; i--) {
      clear_strip();
      s.setPixelColor(i, s.Color(255, 255, 255));
      s.show();
      delay(25);
    }

  } // while()
} // effect_check()



// Mode:
//   0: standard fixed distribution of pixels with fading
//   1: random selection of pixels with fading
//   2: whacky random effect
void effect_fade(unsigned long run_time_sec, int mode) {
  static int delta_delay = 50;
  int reps = 0;

  unsigned long start_time = millis();
  unsigned long run_time_msec = run_time_sec * 1000;
  while (millis() - start_time < run_time_msec) {
    // step down
    for (int intensity = 0; intensity < INTENSITY_CNT; intensity++) {
      if (mode == 1) {
        preset_5_colors_random(intensity, false);
      }
      else if (mode == 2) {
        preset_5_colors_random(intensity, true);
      }
      else { // mode is 0
        preset_5_colors(intensity);
      }
      show_buf();
      delay(delta_delay + random(30));
    } // for intensity

    // step up
    for (int intensity = INTENSITY_CNT - 1; intensity != 0; intensity--) {
      if (mode == 1) {
        preset_5_colors_random(intensity, false);
      }
      else if (mode == 2) {
        preset_5_colors_random(intensity, true);
      }
      else { // mode is 0
        preset_5_colors(intensity);
      }

      show_buf();
      delay(delta_delay + random(40, 75));
    } // for intensity

  } // while() repeat count
} // effect_fade_1()


void define_color_sets() {
  // manually define the starting (high) intensity level
  color_set[0][0] = {255, 20, 15}; // red
  color_set[0][1] = {0, 255, 0}; // green
  color_set[0][2] = {240, 50, 240}; // purple
  color_set[0][3] = {255, 170, 0}; // orange
  color_set[0][4] = {15, 0, 255}; // blue
  color_set[0][5] = {255, 191, 255}; // white

  // automatically calculate the lower intensity tiers
  float scaler;
  float delta_pct = 0.05;

  for (int intensity = 1; intensity < INTENSITY_CNT; intensity++) {
    scaler = 1.0 - (intensity * delta_pct);
    for (int color = 0; color < COLOR_SET_CNT; color++) {
      color_set[intensity][color].r = (uint8_t)(color_set[0][color].r * scaler);
      color_set[intensity][color].g = (uint8_t)(color_set[0][color].g * scaler);
      color_set[intensity][color].b = (uint8_t)(color_set[0][color].b * scaler);
    }
  }
} // define_color_sets()



void show_buf() {
  for (uint16_t i = 0; i < PIXEL_CNT; i++) {
    s.setPixelColor(i, s.Color(buf[i].r, buf[i].g, buf[i].b));
  }
  s.show();
} // show_buf()


void clear_strip() {
  uint32_t blank = s.Color(0, 0, 0);
  for (uint16_t i = 0; i < s.numPixels(); i++) {
    s.setPixelColor(i, blank);
  }
}

void test_pattern1(uint32_t wait) {
  clear_strip();
  uint32_t c = s.Color(255, 0, 0);
  uint32_t i = s.numPixels() - 1;

  while (1) {
    s.setPixelColor(i, c);
    s.show();
    delay(wait);
    i--;
    if (i == -1) {
      i = s.numPixels() - 1;
      c = s.Color(random(0, 255), random(0, 255), random(0, 255));
    }
  } // while(1)
} // test_pattern()
