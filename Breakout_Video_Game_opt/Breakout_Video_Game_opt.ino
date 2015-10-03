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
//ILI9340_CYAN    0x07FF
//ILI9340_MAGENTA 0xF81F
#define ILI9340_ORANGE   0xFAE0

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>
#include <SD.h>

// Pins for the TFT
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _rst 9
#define _dc 8
#define SD_CS 4

// Pins for inputs
#define PaddlePin A0
#define ButtonPin 5

//Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _mosi, _sclk, _rst, _miso);// Using software SPI is really not suggested, its incredibly slow
// TFT object // Use hardware SPI
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

short WIDTH;
short HEIGHT;
byte rotation;
const byte numRow = 8;
const byte numCol = 14;
const byte boarder = 2; //left/right/top boarder width
const byte pad = 2; //space between bricks
short bricksTopY = 66; //y postion of top row of bricks
byte brickWidth = 15;
byte brickHeight = 5;
bool bricks [numCol][numRow]; // 8 rows, 14 columns
bool isOrange = false;
bool isRed = false;
bool isFour  = false;
bool isTwelve = false;
short hits = 0;
byte textSize = 4;
short playFieldTop = (2 * boarder) + (8 * textSize) + 1; //built-in pont is 8 pixel high
short playFieldLeft = boarder + 1;
short scoreTop;
short scoreLeft;
short turnTop;
short turnLeft;
short turn = 0;
short oTurn = 0;

short playerTopY = 280; //y postion of player paddle
byte paddleWidth = 32; // Length of the paddle
const byte paddleHeight = 5; //Height of the paddle

short p1; // Player 1 position
short op1; // Old player 1 position

short x; // x position of the ball
short y; // y position of the ball
short ox; // Old x position of the ball
short oy; // Old y position of the ball
short dx; // Delta x for the ball
short dy; // Delta y for the ball
short odx; // Old Delta x for the ball
short ody; // Old Delta y for the ball
const byte ball = 2;
byte speedDelay = 7;

short score=000;
short oScore=-1;

bool toggle = false;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");


  
  pinMode(PaddlePin, INPUT); //Init paddle
  pinMode(ButtonPin, INPUT_PULLUP);  //Init Button
  
  tft.begin(); // Init display
 
  rotation = 3;
  tft.setRotation(rotation);

   bmpDraw("Brkout.bmp", 0, 0); 
   delay(1000);
  tft.fillScreen(ILI9340_BLACK); // Clear the screen
  WIDTH = tft.width(); //ILI9340_TFTWIDTH  240
  HEIGHT = tft.height(); //ILI9340_TFTHEIGHT 320
  playerTopY = HEIGHT - paddleHeight * 5;

  if (rotation % 2)
  {
    Serial.println("landscape");
    textSize = 3;
    playFieldTop = boarder + 1; 
    playFieldLeft = (2 * boarder) + (8 * textSize) + 1; //built-in font is 8 pixel high
    playFieldLeft = WIDTH -  (numCol * (brickWidth + pad));
    scoreTop = 8 + (4 * boarder);
    scoreLeft = 0;
    turnTop = scoreTop + (10 * textSize) + (4 * boarder);
    turnLeft = 0;
    tft.setTextSize(1);
    tft.setCursor(2 * boarder,turnTop - textSize  - (4 * boarder) );
    tft.print("BALL");
  }
  else
  {
    Serial.println("portrait");    
    playFieldTop = (2 * boarder) + (8 * textSize) + 1; //built-in font is 8 pixel high
    playFieldLeft = boarder + 1;
    scoreTop = boarder+pad;
    scoreLeft = 6*8;   
    turnLeft = scoreLeft + (34 * textSize) + (2 * boarder) + 1;
    turnTop = boarder+pad;
    tft.setTextSize(1);
    tft.setCursor(turnLeft - (8 * textSize), 2 * boarder);
    tft.print("BALL");
  }
  bricksTopY = playFieldTop+30;

  tft.setCursor(2 * boarder,2 * boarder);
  tft.setTextSize(1);
  tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK);
  tft.print("SCORE");


  turn = 1;
  
//  bricksTopY = 66;
//  playFieldTop = (2 * boarder) + (8 * textSize) + 1; //built-in pont is 8 pixel high
//  playFieldLeft = boarder + 1;
  
  Serial.print("w:");Serial.println(WIDTH);
  Serial.print("h:");Serial.println(HEIGHT);
  //Init Bricks array
  initBricks();

  //set initial paddle position
  p1 = WIDTH/2;
  op1 = !p1;

  //set initial ball postion and direcction
  
  x = (WIDTH-playFieldLeft)/2;
  y = HEIGHT - (bricksTopY + (numRow * (brickHeight + pad)))  / 2;
  
  ox = !x;
  oy = !y;
  dx = 1;
  dy = 1;
  odx = dx;
  ody = dy;


  //Draw boarder
//  tft.drawFastHLine(boarder, playFieldTop-1, WIDTH-boarder, ILI9340_WHITE); //top
//  tft.drawFastVLine(boarder-1, playFieldTop, HEIGHT-playFieldTop - boarder, ILI9340_WHITE);//left
//  tft.drawFastVLine(WIDTH-1, playFieldTop, HEIGHT-playFieldTop - boarder, ILI9340_WHITE);//right
  tft.drawFastHLine(playFieldLeft-1, playFieldTop-1, WIDTH-boarder, ILI9340_WHITE); //top
  tft.drawFastVLine(playFieldLeft-1, playFieldTop, HEIGHT-playFieldTop - boarder, ILI9340_WHITE);//left
  tft.drawFastVLine(WIDTH-1, playFieldTop, HEIGHT-playFieldTop - boarder, ILI9340_WHITE);//right


}

void loop() 
{
  // put your main code here, to run repeatedly:

  //tft.drawFastVLine(boarder + (brickWidth + pad), 0, HEIGHT, ILI9340_CYAN);

if (digitalRead(ButtonPin) == LOW)
{
  toggle = !toggle;
  bmpDraw("atari_l.bmp", 0, 0);
  delay(1000);
  tft.fillScreen(ILI9340_BLACK); // Clear the screen
  tft.setCursor(scoreLeft,scoreTop);
  tft.setTextColor(ILI9340_BLACK, ILI9340_BLACK);
  tft.setTextSize(textSize);
  tft.print(score);
  score = 0;
  oScore = -1;
  initBricks();
  turn = 1;
  oTurn = 0;
}



// Draw the score for player 1
  if (score != oScore) 
  { 
    tft.setCursor(scoreLeft,scoreTop);
    tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK);
    tft.setTextSize(textSize);
    tft.print(score);
    oScore = score;
  }

  if (turn != oTurn) 
  { 
    tft.setCursor(turnLeft,turnTop);
    tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK);
    tft.setTextSize(textSize);
    tft.print(turn);
    oTurn = turn;
  }

  // Draw the ball
  if (x != ox || y != oy) // Erase the old ball if the position changed
  { 
    tft.fillCircle(ox, oy, ball, ILI9340_BLACK);
//    if (isInBrickArea(oy))
//    {
//      //drawBrick (getBrickCol(ox), getBrickRow(oy));
//      drawBrick (getBrickCol(ox+(odx*ball)), getBrickRow(oy+(ody*ball)));
//    }
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
  //  p1 = map(analogRead(A0), 0, 1023, (WIDTH - paddleWidth/2)-boarder, paddleWidth/2 + boarder);
  p1 = map(analogRead(A0), 0, 1023, (WIDTH - paddleWidth/2)-boarder, paddleWidth/2 + boarder + playFieldLeft);


  //Check brick collisions
  if (isInBrickArea(y))
  {
    byte col = getBrickCol(x);
    byte row = getBrickRow(y);

    if (col < numCol && row < numRow)
    {
        if (bricks[col][row]) 
        {
          bricks[col][row] = false;
          drawBrick (col, row);
    
          ody = dy;
          dy = -dy;
          
          //score++;
          if (hits == 4) isFour = true;
          if (hits == 12) isTwelve = true;
          hits++;
          if (row >= 0 && row < 2) { score = score + 7; isRed= true;}
          else if (row >= 2 && row < 4) { score = score + 5; isOrange = true;}
          else if (row >= 4 && row < 6) score = score + 3;
          else if (row >= 6 && row < 8) score = score + 1;
          
          if (score == 448)
          {
            initBricks();
          }
        }
    }

  }

  // Check if ball hits walls
  //if (x <= boarder + ball || x >= WIDTH - boarder - ball)
  if (x <= boarder +playFieldLeft + ball || x >= WIDTH - boarder - ball)
  {
    odx = dx;
    dx = -dx;
  }

  // Check if ball hits ceiling or floor
  
  //if (y - ball <= playFieldTop  || y >= HEIGHT - ball - paddleHeight)
  if (y - ball <= playFieldTop)
  {
    ody =dy;
    dy = -dy;
  }

  if (y >= HEIGHT - ball - paddleHeight)
  {
    turn++;
    //center ball
    x = WIDTH/2;
    y = (HEIGHT)/2;
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
  if (isFour) speedDelay = 7;
  if (isTwelve) speedDelay = 5;
  if (isOrange) speedDelay = 3;
  if (isRed) speedDelay = 1;
  delay(speedDelay);

}

void initBricks()
{
  //Init Bricks array
  for (byte i = 0; i < numCol; i++) 
  {
        for (byte j = 0; j < numRow; j++) 
        {
          bricks [i][j] = true;
        }
  }
  
  //center ball
  x = WIDTH/2;
  y = (HEIGHT)/2;
  // Draw the Bricks
  drawBricks();
  hits = 0;
  isFour = false;
  isTwelve = false;
  isOrange = false;
  isRed = false;
}

void drawBricks()
{
  for (byte  i = 0; i < numCol; i++) 
  {
        for (byte  j = 0; j < numRow; j++) 
        {
            drawBrick(i,j);
        }
  }
}

void drawBrick (byte col, byte row)
{
  if ((col >= 0 && col < numCol) && (row >= 0 && row < numRow))
  {
    uint16_t color = ILI9340_BLACK;
    if (bricks [col][row])
    {
      if (row == 0 || row == 1) color = ILI9340_RED;
      else if (row == 2 || row == 3) color = ILI9340_ORANGE;
      else if (row == 4 || row == 5) color = ILI9340_GREEN;
      else color = ILI9340_YELLOW;
    } 
    //tft.fillRect(boarder + (col * (brickWidth + pad)), bricksTopY + (row * (brickHeight + pad)), brickWidth, brickHeight, color); 
    tft.fillRect(playFieldLeft + (col * (brickWidth + pad)), bricksTopY + (row * (brickHeight + pad)), brickWidth, brickHeight, color);
  }
}

byte getBrickRow(short y)
{
  return  map(y, bricksTopY-pad, bricksTopY + (numRow * (brickHeight + pad)),0,numRow-1);
}

byte getBrickCol(short x)
{
  //return map(x, boarder, WIDTH-boarder, 0,13);
   //return map(x, 0, 239, 0,13);  
byte col = 255;
if (x > playFieldLeft && x <= playFieldLeft+ brickWidth + pad) col = 0;
else if (x > playFieldLeft+ brickWidth + pad && x <= playFieldLeft+ 2 * (brickWidth + pad)) col = 1;
else if (x > playFieldLeft+ 2 * (brickWidth + pad)  && x <= playFieldLeft+ 3 * (brickWidth + pad)) col = 2;
else if (x > playFieldLeft+ 3 * (brickWidth + pad)  && x <= playFieldLeft+ 4 * (brickWidth + pad)) col = 3;
else if (x > playFieldLeft+ 4 * (brickWidth + pad)  && x <= playFieldLeft+ 5 * (brickWidth + pad)) col = 4;
else if (x > playFieldLeft+ 5 * (brickWidth + pad)  && x <= playFieldLeft+ 6 * (brickWidth + pad)) col = 5;
else if (x > playFieldLeft+ 6 * (brickWidth + pad)  && x <= playFieldLeft+ 7 * (brickWidth + pad)) col = 6;
else if (x > playFieldLeft+ 7 * (brickWidth + pad)  && x <= playFieldLeft+ 8 * (brickWidth + pad)) col = 7;
else if (x > playFieldLeft+ 8 * (brickWidth + pad)  && x <= playFieldLeft+ 9 * (brickWidth + pad)) col = 8;
else if (x > playFieldLeft+ 9 * (brickWidth + pad)  && x <= playFieldLeft+ 10 * (brickWidth + pad)) col = 9;
else if (x > playFieldLeft+ 10 * (brickWidth + pad)  && x <= playFieldLeft+ 11 * (brickWidth + pad)) col = 10;
else if (x > playFieldLeft+ 11 * (brickWidth + pad)  && x <= playFieldLeft+ 12 * (brickWidth + pad)) col = 11;
else if (x > playFieldLeft+ 12 * (brickWidth + pad)  && x <= playFieldLeft+ 13 * (brickWidth + pad)) col = 12;
else if (x > playFieldLeft+ 13 * (brickWidth + pad)  && x <= playFieldLeft+ 14 * (brickWidth + pad)) col = 13;

//int r = map(x, 0, 239, 0,13);
//
//if (col != r) 
//{
//  Serial.print("!!!! map:"); Serial.print(r); Serial.print(" col:"); Serial.print(col);  Serial.println("!!!!");  
//}
return col;
}

bool isInBrickArea(short y)
{
  return (y > bricksTopY && y <= bricksTopY + (numRow * (brickHeight + pad)));
}


#define BUFFPIXEL 20

void bmpDraw(char *filename, uint16_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File & f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File & f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
