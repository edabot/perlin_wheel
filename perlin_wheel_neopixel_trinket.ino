#include <FastLED.h>

#define LED_PIN 12
#define NUM_LEDS 16
#define BRIGHTNESS 96
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

const float circleRadius = NUM_LEDS / PI;
const float angle = 2 * PI / NUM_LEDS;
float xOffsets[NUM_LEDS];
float yOffsets[NUM_LEDS];

int speedH = 4;   //Hue value is 16-bit
int scaleH = 50;
int speedS = 7;
int scaleS = 30;
int speedV = 10;
int scaleV = 50;

float colorStart = 0;     
float colorRange = .7;  //Range of each section of color 1 = 100%
float colorSpeed = .1;    //Speed of color cycling
int colorMax = 255;

String mode = "none"; // options 1: normal, 2: complementary, 3: triad

// The leds
CRGB leds[NUM_LEDS];

float x, y, zH, zS, zV;

uint8_t colorLoop = 1;

void setup()
{
  Serial.begin(9600);
  LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);

  x = 0.0;
  y = 0.0;
  zH = 100;
  zS = 50;
  zV = 0;
  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    float thisAngle = angle * i;
    float xoffset = cos(thisAngle) * circleRadius;
    float yoffset = sin(thisAngle) * circleRadius;
    xOffsets[i] = xoffset;
    yOffsets[i] = yoffset;
  }
}

void fillnoise()
{

  for (int i = 0; i < NUM_LEDS; i++)
  {
    int noiseValH = inoise8(x + xOffsets[i] * scaleH, y + yOffsets[i] * scaleH, zH);
    int noiseValS = inoise8(x + xOffsets[i] * scaleS, y + yOffsets[i] * scaleS, zS);
    int noiseValV = inoise8(x + xOffsets[i] * scaleV, y + yOffsets[i] * scaleS, zV);

    int hue = (int) (noiseValH * colorRange + colorStart);
    if (mode=="TRIAD"){
      if (noiseValH > colorMax * 5 / 8) {
        hue = hue + colorMax * 2 / 3 - colorMax * colorRange * 5 / 8;
      } else if (noiseValH > colorMax * 3 / 8) {
        hue = hue + colorMax / 3 - colorMax * colorRange * 3 / 8;
      }
    }
    if (mode=="COMPLEMENTARY") {
      if (noiseValH > colorMax / 2) {
        hue = hue + colorMax / 2 - colorMax * colorRange / 2;
      } 
    }
    hue = hue % colorMax;
    
    int saturation = constrain(noiseValS + 70, 0, 255);
    int value = constrain(noiseValV - 20, 0, 255);
    
    leds[i] = CHSV(hue, saturation, value);
  }

  zH += speedH;
  zS += speedS;
  zV += speedV;

  // apply slow drift to X and Y, just for visual variation.
  x += speedS / 8;
  y -= speedS / 16;
  
  colorStart += colorSpeed;
  if (colorStart > colorMax) { colorStart -= colorMax; }

}


void loop()
{
  fillnoise();

  LEDS.show();
  delay(20);
}
