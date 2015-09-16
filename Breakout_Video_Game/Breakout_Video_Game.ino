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
#define ILI9340_ORANGE   0xFAE0

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
int WIDTH;
int HEIGHT;
int score;
int oScore;
bool bricks [14][8]; // 8 rows, 14 columns
int p1; // Player 1 position
int op1; // Old player 1 position
int paddleWidth; // Length of the paddle
int paddleHeight; //Height of the paddle

void setup() 
{
  // put your setup code here, to run once:
  pinMode(A0, INPUT); //Init paddle
  
  tft.begin(); // Init display
  tft.setRotation(0);
  tft.fillScreen(ILI9340_BLACK); // Clear the screen
  WIDTH = tft.width(); //ILI9340_TFTWIDTH  240
  HEIGHT = tft.height(); //ILI9340_TFTHEIGHT 320
  
  score = 0;
  oScore = !score;
  
  //Init Bricks array
  for (int i = 0; i < 14; i++) 
  {
        for (int j = 0; j < 8; j++) 
        {
          bricks [i][j] = true;
        }
  }
  
  p1 = WIDTH/2;
  op1 = !p1;
  paddleWidth = 32;
  paddleHeight = 5;
}

void loop() 
{
  // put your main code here, to run repeatedly:

// Draw the score for player 1
  if (score != oScore) { // If the score has changed...
    tft.drawRect(22, 32, 20, 32, ILI9340_BLACK); // Then erase the old score...
    score = oScore; // Set the old score to the current score...
    tft.drawChar(22, 32, '4', ILI9340_WHITE, ILI9340_BLACK, 4); // And draw the new score
  }
  

// Draw Bricks
int boarder = 2;
int brickWidth = 15;
int brickHeight = 5;
int bricksTopY = 66;
int pad = 1;
int playerTopY = 280;

for (int i = 0; i < 14; i++) 
  {
        for (int j = 0; j < 8; j++) 
        {
          if (bricks [i][j])
          {
            uint16_t color = ILI9340_RED;
            switch (j)
            {
              case 0 ... 1:
                color = ILI9340_RED;
                break;
              case 2 ... 3:
                color = ILI9340_ORANGE;
                break;
              case 4 ... 5:
                color = ILI9340_GREEN;
                break;
              case 6 ... 7:
                color = ILI9340_YELLOW;
                break;
            }
            tft.fillRect(i+boarder+(i*(brickWidth+pad)), j+bricksTopY+(j*(brickHeight+pad)), brickWidth, brickHeight, color); 
          }
        }
  }

  if (p1 != op1) // Erase the old paddle if the position changed 
  { 
    //tft.drawFastHLine(op1 - paddleWidth/2, playerTopY, paddleWidth, ILI9340_BLACK); 
    tft.fillRect( op1 - paddleWidth/2, playerTopY, paddleWidth, paddleHeight, ILI9340_BLACK);
    op1 = p1; 
  } 
  //Draw paddle
  //tft.drawFastHLine(p1 - paddleWidth/2, playerTopY, paddleWidth, ILI9340_BLUE); // Draw the new paddle
  tft.fillRect( p1 - paddleWidth/2, playerTopY, paddleWidth, paddleHeight, ILI9340_BLUE);
  
  // Move the paddles
  p1 = map(analogRead(A0), 0, 1023, (WIDTH - paddleWidth/2)-boarder, paddleWidth/2 + boarder);
}
