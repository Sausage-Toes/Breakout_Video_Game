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
int pad = 2; //space between bricks
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
double odx; // Old Delta x for the ball
double ody; // Old Delta y for the ball
int ball = 1;
int speedDelay = 10;
int topPlayField = (2 * boarder) + (8 * 4) + 1; 

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
  Serial.print("w:");Serial.println(WIDTH);
  Serial.print("h:");Serial.println(HEIGHT);
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
  dx = 1;
  dy = 1;
  odx = dx;
  ody = dy;

  
  //tft.drawFastVLine(boarder-1, boarder, HEIGHT-2*boarder, ILI9340_WHITE);//left
  //tft.drawFastVLine(WIDTH-1, boarder, HEIGHT-2*boarder, ILI9340_WHITE);//right
  //tft.drawFastHLine(boarder, boarder, WIDTH-boarder, ILI9340_WHITE); //top
  //tft.drawFastHLine(boarder, HEIGHT-boarder, WIDTH-boarder, ILI9340_WHITE);//bottom
  
  tft.drawFastHLine(boarder, topPlayField-1, WIDTH-boarder, ILI9340_WHITE);
  tft.drawFastVLine(boarder-1, topPlayField, HEIGHT-topPlayField - boarder, ILI9340_WHITE);//left
  tft.drawFastVLine(WIDTH-1, topPlayField, HEIGHT-topPlayField - boarder, ILI9340_WHITE);//right
 
  
}

void loop() 
{
  // put your main code here, to run repeatedly:

  //tft.drawFastVLine(boarder + (brickWidth + pad), 0, HEIGHT, ILI9340_CYAN);

// Draw the score for player 1
  if (score != oScore) 
  { 
    //tft.setCursor(22,32);
    tft.setCursor(22,boarder+pad);
    tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK);
    tft.setTextSize(4);
    tft.print(score);
    Serial.print("score:");Serial.println(score);
    Serial.print("x:");Serial.println(x);
    Serial.print("y:");Serial.println(y);
    Serial.print("col:");Serial.println(getBrickCol(x));
    Serial.print("row:");Serial.println(getBrickRow(y));
    oScore = score;
    //if (score == 3) speedDelay = 99999;
  }

  // Draw the ball
  if (x != ox || y != oy) // Erase the old ball if the position changed
  { 
    tft.fillCircle(ox, oy, ball, ILI9340_BLACK);
    if (isInBrickArea(oy))
    {
      drawBrick (getBrickCol(ox), getBrickRow(oy));
    }
    ox = x; 
    oy = y; 
  } 
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


  //Check brick collisions
  if (isInBrickArea(y))
  {
    int col = getBrickCol(x);
    int row = getBrickRow(y);
    
    if (bricks[col][row]) 
    {
      bricks[col][row] = false;
      drawBrick (col, row);

      ody = dy;
      dy = -dy;
      
      score++;
      if (score == 112)
      {
        initBricks();
      }
    }
  }

  // Check if ball hits walls
  if (x <= boarder + ball || x >= WIDTH - boarder - ball)
  {
    odx = dx;
    dx = -dx;
  }

  // Check if ball hits ceiling or floor
  //if (y <= bricksTopY  || y >= HEIGHT - ball-10)
  if (y - ball <= topPlayField  || y >= HEIGHT - ball-10)
  {
    ody =dy;
    dy = -dy;
  }

  // Check if ball hits paddle
  if (y >= playerTopY - ball && y <=  playerTopY + ball + paddleHeight)
  {
    if ((x >= p1 - paddleWidth/2) && (x <= p1  + paddleWidth/2))
    {
      ody =dy;
      dy = -dy;
      //fix for sticky paddle bug
      if (dy > 0) {
        y = playerTopY + ball + paddleHeight;
      }
      else {
        y = playerTopY - ball;
      }

      //check paddle sides
      if ((x == p1 - paddleWidth/2) || (x == p1  + paddleWidth/2))
      {
        dx = -dx;
      }
    }
  }

  //set speed
  delay(speedDelay);

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
            drawBrick(i,j);
        }
  }
}

void drawBrick (int col, int row)
{
  if ((col >= 0 && col <= 13) && (row >= 0 && row <= 7))
  {
    uint16_t color = ILI9340_BLACK;
    if (bricks [col][row])
    {
      if (row == 0 || row == 1) color = ILI9340_RED;
      else if (row == 2 || row == 3) color = ILI9340_ORANGE;
      else if (row == 4 || row == 5) color = ILI9340_GREEN;
      else color = ILI9340_YELLOW;
    } 
    tft.fillRect(boarder + (col * (brickWidth + pad)), bricksTopY + (row * (brickHeight + pad)), brickWidth, brickHeight, color); 
  }
}

byte getBrickRow(int y)
{
  return  map(y, bricksTopY-pad, bricksTopY + (8 * (brickHeight + pad)),0,7);
}

byte getBrickCol(int x)
{
  //return map(x, boarder, WIDTH-boarder, 0,13);
   //return map(x, 0, 239, 0,13);  
int col = -1;
if (x > 0 && x <= brickWidth + pad) col = 0;
else if (x > brickWidth + pad && x <= 2 * (brickWidth + pad)) col = 1;
else if (x > 2 * (brickWidth + pad)  && x <= 3 * (brickWidth + pad)) col = 2;
else if (x > 3 * (brickWidth + pad)  && x <= 4 * (brickWidth + pad)) col = 3;
else if (x > 4 * (brickWidth + pad)  && x <= 5 * (brickWidth + pad)) col = 4;
else if (x > 5 * (brickWidth + pad)  && x <= 6 * (brickWidth + pad)) col = 5;
else if (x > 6 * (brickWidth + pad)  && x <= 7 * (brickWidth + pad)) col = 6;
else if (x > 7 * (brickWidth + pad)  && x <= 8 * (brickWidth + pad)) col = 7;
else if (x > 8 * (brickWidth + pad)  && x <= 9 * (brickWidth + pad)) col = 8;
else if (x > 9 * (brickWidth + pad)  && x <= 10 * (brickWidth + pad)) col = 9;
else if (x > 10 * (brickWidth + pad)  && x <= 11 * (brickWidth + pad)) col = 10;
else if (x > 11 * (brickWidth + pad)  && x <= 12 * (brickWidth + pad)) col = 11;
else if (x > 12 * (brickWidth + pad)  && x <= 13 * (brickWidth + pad)) col = 12;
else if (x > 13 * (brickWidth + pad)  && x <= 14 * (brickWidth + pad)) col = 13;

//int r = map(x, 0, 239, 0,13);
//
//if (col != r) 
//{
//  Serial.print("!!!! map:"); Serial.print(r); Serial.print(" col:"); Serial.print(col);  Serial.println("!!!!");  
//}
return col;
}

bool isInBrickArea(int y)
{
  return (y > bricksTopY && y <= bricksTopY + (8 * (brickHeight + pad)));
}

