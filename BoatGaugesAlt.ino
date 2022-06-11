#include <TFT_eSPI.h>
#include "gauge1.h" // coolant pressure
#include "gauge2.h" // coolant temp
#include "gauge3.h" // oil pressure
#include "gauge4.h" // speedometer
#include "gauge5.h" // tachometer
#include "gauge6.h" // battery voltage
#include "font.h"

TFT_eSPI tft = TFT_eSPI();           // invoke tft library
TFT_eSprite img = TFT_eSprite(&tft); // sprite for gauge

#define RAD_PER_DEG    0.01745 // number of radian per degree

// needle indicator circle constants
#define SX        120 // x coordinate
#define SY        120 // y coordinate
#define RADIUS     76 // radius

// gauge params
int chosenOne = 4;                              // selected gauge index
int minValue[6] = {  0,  20,  0,  0,  0,  80 }; // min values for different gauges
int maxValue[6] = { 40, 100, 60, 80, 70, 160 }; // max values for different gauges

int dbounce = 0;               // simple button debounce variable to prevent skipping multiple gauges as a result of button press
int randomADC = 0;             // simulates potentiometer value from analogRead(14)
int currentADC = 0;            // simulates change in potentiometer value
int result = 0;                // mapped potentiometer value to gauge min/max range
int angle;                     // mapped potentiometer value to corresponding angle on a circle
int a1, a2;                    // min/max angles for needle indicator bounding box

// needle indicator triangle coordinates
double tri_x1, tri_y1, tri_x2, tri_y2, tri_x3, tri_y3;

// randomADC generator timing variables
unsigned long cur_millis = 0;
unsigned long old_millis = 0;

// super smooth ADC change
bool dir = true; // direction: 1 = increment, 0 = decrement

void setup() {
  pinMode(12, INPUT_PULLUP); // button

  // initialise tft library
  tft.init();
  tft.setRotation(0);         // set the display image orientation to 0, 1, 2 or 3
  //tft.setSwapBytes(true);     // swap the byte order for pushImage() and pushPixels() - corrects endianness
  tft.fillScreen(TFT_ORANGE); // fill default background color
  //tft.setPivot(60, 60);       // set TFT pivot point (use when rendering rotated sprites)

  img.setSwapBytes(true);     // required to load background sprite
  img.createSprite(240, 240); // create sprite in RAM

  // set gauge text position, size, color, font
  img.setTextDatum(4);
  img.setTextColor(TFT_BLACK, 0xAD55);
  img.setFreeFont(&Orbitron_Medium_28);
}

void loop() {
  //// generate new random ADC value once per second
  //cur_millis = millis();
  //if (cur_millis - old_millis > 1000) {
  //  randomADC = random(4095);
  //  old_millis = cur_millis;
  //}

  //// simulate smooth change in potentiometer value
  //if (currentADC <= randomADC) {
  //  currentADC++;
  //} else {
  //  currentADC--;
  //}

  // simulate super smooth change in potentiometer value for demonstration of dial range accuracy
  // particularly RPM gauge which changes in +/- 100 RPM increments
  if (dir) {
    currentADC++;
    if (currentADC >= 4095) {
      dir = false;
    }
  } else {
    currentADC--;
    if (currentADC <= 0) {
      dir = true;
    }
  }

  // check button state with debounce logic
  if (digitalRead(12) == 0) {
    if (dbounce == 0) {
      dbounce = 1; // set to prevent skipping gauges if button is held
      chosenOne++; // select next gauge

      // reset gauge index on overflow
      if (chosenOne >= 6) {
        chosenOne = 0;
      }
    }
  } else {
    dbounce = 0;
  }

  // read potentiometer value and map it for selected gauge min/max range
  //result = map(analogRead(14), 0, 4095, minValue[chosenOne], maxValue[chosenOne]);
  result = map(currentADC, 0, 4095, minValue[chosenOne], maxValue[chosenOne]);

  // map obtained result to corresponding angle on a circle
  angle = map(result, minValue[chosenOne], maxValue[chosenOne], 0, 267);

  // shift mapped angle by 136 degrees along the circle
  // this will match the zero postion on the gauge
  angle = angle + 136;

  // push currently selected gauge image to "img" sprite
  switch (chosenOne) {
    case 0: img.pushImage(0, 0, 240, 240, gauge1); break;
    case 1: img.pushImage(0, 0, 240, 240, gauge2); break;
    case 2: img.pushImage(0, 0, 240, 240, gauge3); break;
    case 3: img.pushImage(0, 0, 240, 240, gauge4); break;
    case 4: img.pushImage(0, 0, 240, 240, gauge5); break;
    case 5: img.pushImage(0, 0, 240, 240, gauge6); break;
  }

  // push measured value to "img" sprite
  if (chosenOne == 5) {
    // battery voltage with 2 decimal places
    img.drawFloat(result / 10.00, 2, 120, 114);
  } else if (chosenOne == 4) {
    // tachometer (with x100 multiplier)
    img.drawString(String(result * 100), 120, 114);
  }  else {
    img.drawString(String(result), 120, 114);
  }

  // debug: push raw potentiometer ADC value to "img" sprite
  //img.drawString(String(analogRead(22)), 30, 10, 2);

  // set min & max bounding box angles
  a1 = angle - 4;
  a2 = angle + 4;

  // check angle limits (end stops)
  if (a1 < 0) {
    a1 = angle - 4 + 359; // rotate angle clockwise
  }

  if (a2 >= 359) {
    a2 = angle + 4 - 359; // rotate angle counter clockwise
  }

  // determine needle indicator coordinates
  if (result <= minValue[chosenOne] + 4) {
    // triangle coordinates at minimum end stop
    tri_x1 = RADIUS * cos(RAD_PER_DEG * angle) + SX;
    tri_y1 = RADIUS * sin(RAD_PER_DEG * angle) + SY;
    tri_x2 = (RADIUS - 20) * cos(RAD_PER_DEG * angle) + SX;
    tri_y2 = (RADIUS - 20) * sin(RAD_PER_DEG * angle) + SY;
    tri_x3 = (RADIUS - 20) * cos(RAD_PER_DEG * (a2 + 2)) + SX;
    tri_y3 = (RADIUS - 20) * sin(RAD_PER_DEG * (a2 + 2)) + SY;
  } else if (result >= maxValue[chosenOne] - 4) {
    // triangle coordinates at maximum end stop
    tri_x1 = RADIUS * cos(RAD_PER_DEG * angle) + SX;
    tri_y1 = RADIUS * sin(RAD_PER_DEG * angle) + SY;
    tri_x2 = (RADIUS - 20) * cos(RAD_PER_DEG * (a1 - 2)) + SX;
    tri_y2 = (RADIUS - 20) * sin(RAD_PER_DEG * (a1 - 2)) + SY;
    tri_x3 = (RADIUS - 20) * cos(RAD_PER_DEG * angle) + SX;
    tri_y3 = (RADIUS - 20) * sin(RAD_PER_DEG * angle) + SY;
  } else {
    // triangle coordinates between min and max end stops
    tri_x1 = RADIUS * cos(RAD_PER_DEG * angle) + SX;
    tri_y1 = RADIUS * sin(RAD_PER_DEG * angle) + SY;
    tri_x2 = (RADIUS - 20) * cos(RAD_PER_DEG * a1) + SX;
    tri_y2 = (RADIUS - 20) * sin(RAD_PER_DEG * a1) + SY;
    tri_x3 = (RADIUS - 20) * cos(RAD_PER_DEG * a2) + SX;
    tri_y3 = (RADIUS - 20) * sin(RAD_PER_DEG * a2) + SY;
  }

  // draw red triangle corresponding to measure value on "img" sprite
  img.fillTriangle(tri_x1, tri_y1, tri_x2, tri_y2, tri_x3, tri_y3, TFT_RED);

  // push resulting gauge sprite (gauge image + measure value + red triangle needle) to display
  img.pushSprite(0, 0);
}