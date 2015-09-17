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
int score=0;
int oScore=-1;
bool bricks [14][8]; // 8 rows, 14 columns
int boarder = 2; //left/right/top boarder width
int brickWidth = 15;
int brickHeight = 5;
int bricksTopY = 66; //y postion of top row of bricks
int pad = 1; //space between bricks
int playerTopY = 280; //y postion of player paddle
int p1; // Player 1 position
int op1; // Old player 1 position
int paddleWidth = 32; // Length of the paddle
int paddleHeight = 5; //Height of the paddle
int x; // x position of the ball
int y; // y position of the ball
int ox; // Old x position of the ball
int oy; // Old y position of the ball
double dx; // Delta x for the ball
double dy; // Delta y for the ball
int ball = 3;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(A0, INPUT); //Init paddle
  
  tft.begin(); // Init display
  tft.setRotation(0);
  tft.fillScreen(ILI9340_BLACK); // Clear the screen
  WIDTH = tft.width(); //ILI9340_TFTWIDTH  240
  HEIGHT = tft.height(); //ILI9340_TFTHEIGHT 320
  
  //Init Bricks array
  initBricks();

  //set initial paddle position
  p1 = WIDTH/2;
  op1 = !p1;

  //set initial ball postion and direcction
  x = WIDTH/2;
  y = (HEIGHT)/2;
  ox = !x;
  oy = !y;
  //dx = ball*2;
  //dy = ball*2;
  dx = 1;
  dy = 1;
}

void loop() 
{
  // put your main code here, to run repeatedly:
//  Serial.print("dx:");
//  Serial.print(dx);
//  Serial.print(" dy:");
//  Serial.println(dy);
  
// Draw the score for player 1
  if (score != oScore) 
  { 
    tft.setCursor(22,32);
    tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK);
    tft.setTextSize(4);
    tft.print(score);
    Serial.println(score);
    oScore = score;
  }


  
  // Draw the ball
  if (x != ox || y != oy) { tft.fillCircle(ox, oy, ball, ILI9340_BLACK); ox = x; oy = y; } // Erase the old ball if the position changed
  tft.fillCircle(x, y, ball, ILI9340_WHITE); // Draw the new ball
  // Update the ball
  x += dx;
  y += dy;
  
  // Draw paddle
  if (p1 != op1) // Erase the old paddle if the position changed 
  { 
    tft.fillRect( op1 - paddleWidth/2, playerTopY, paddleWidth, paddleHeight, ILI9340_BLACK);
    op1 = p1; 
  } 
  tft.fillRect( p1 - paddleWidth/2, playerTopY, paddleWidth, paddleHeight, ILI9340_BLUE);
  
  // Move the paddle
  p1 = map(analogRead(A0), 0, 1023, (WIDTH - paddleWidth/2)-boarder, paddleWidth/2 + boarder);


  
  if (y >= bricksTopY-2*pad && y <= bricksTopY + (8 * (brickHeight + 2*pad)))
  {
    int row = map(y+ball, bricksTopY-2*pad, bricksTopY + (8 * (brickHeight + 2*pad)),0,7);
    int col = map(x+ball, boarder, WIDTH-2*boarder-ball, 0,13);
    //int col = map(x+ball, boarder, WIDTH-boarder, 0,13);
    if (bricks[col][row]) 
    {
      bricks[col][row] = false;
      tft.fillRect(col+boarder+(col*(brickWidth+pad)), row+bricksTopY+(row*(brickHeight+pad)), brickWidth, brickHeight, ILI9340_BLACK);
      dy = -dy;
      
      score++;
      if (score == 112)
      {
        initBricks();
      }
    }
  }

  // Check if ball hits walls
  if (x <= 0 + ball || x >= WIDTH - ball)
  {
    dx = -dx;
  }

  // Check if ball hits ceiling or floor
  if (y <= bricksTopY || y >= HEIGHT - ball-10)
  {
     dy = -dy;
  }

  // Check if ball hits paddle
  if (y >= playerTopY - ball && y <=  playerTopY + ball + paddleHeight)
  {
    if ((x >= p1 - paddleWidth/2) && (x <= p1  + paddleWidth/2))
    {
      dy = -dy;
    }
  }

}

void initBricks()
{
  //Init Bricks array
  for (byte i = 0; i < 14; i++) 
  {
        for (byte j = 0; j < 8; j++) 
        {
          bricks [i][j] = true;
        }
  }
  //center ball
  x = WIDTH/2;
  y = (HEIGHT)/2;
  // Draw the Bricks
  drawBricks();
}

void drawBricks()
{
  for (byte  i = 0; i < 14; i++) 
  {
        for (byte  j = 0; j < 8; j++) 
        {
          uint16_t color = ILI9340_BLACK;
          if (bricks [i][j])
          {
            if (j == 1 || j == 0) color = ILI9340_RED;
            else if (j == 2 || j == 3) color = ILI9340_ORANGE;
            else if (j == 4 || j == 5) color = ILI9340_GREEN;
            else color = ILI9340_YELLOW;
            
            tft.fillRect(i+boarder+(i*(brickWidth+pad)), j+bricksTopY+(j*(brickHeight+pad)), brickWidth, brickHeight, color); 
          }
        }
  }
}

