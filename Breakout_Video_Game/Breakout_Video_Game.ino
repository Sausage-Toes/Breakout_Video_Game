//Breakout begins with eight rows of bricks, with each two rows a different color. 
//The color order from the bottom up is yellow, green, orange and red. 
//Yellow bricks earn one point each, green bricks earn three points, orange bricks
//earn five points and the top-level red bricks score seven points each.
//The paddle shrinks to one-half its size after the ball has broken through the red
//row and hit the upper wall. Ball speed increases at specific intervals: 
//after four hits, after twelve hits, 
//and after making contact with the orange and red rows.

//red = 7
//orange = 5
//green = 3
//yellow = 1
//Cols = 14
//Rows = 2*4

//ILI9340_BLACK   0x0000
//ILI9340_BLUE    0x001F
//ILI9340_YELLOW  0xFFE0  
//ILI9340_GREEN   0x07E0
//ILI9340_RED     0xF800
//ILI9340_WHITE   0xFFFF
#define ILI9340_ORANGE   0xFBE0

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>

// Pins for the TFT
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _rst 9
#define _dc 8

//Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _mosi, _sclk, _rst, _miso);// Using software SPI is really not suggested, its incredibly slow
// TFT object // Use hardware SPI
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

void setup() 
{
  // put your setup code here, to run once:
  pinMode(A0, INPUT); // Paddle
  tft.begin(); // Init display
  tft.setRotation(0);
  tft.fillScreen(ILI9340_BLACK); // Clear the screen
  //ILI9340_TFTWIDTH  240
  WIDTH = tft.width();
  //ILI9340_TFTHEIGHT 320
  HEIGHT = tft.height();
  
}

void loop() 
{
  // put your main code here, to run repeatedly:


}
