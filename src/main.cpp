#include <Arduino.h>

// Simple Drawing Script for AZ-Delivery TFT Touch 240x320 Shield
// Tony Goodhew 30 Sept 2020
// Modified from Adafruit examples for a similar shield
// Tutorial45.com
// Adafruit GFX Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

#include "joystick.cpp"
#include "beeping.cpp"
//#include "sequencer.cpp"


#define LCD_CS A3     // Chip Select goes to Analog 3
#define LCD_CD A2     // Command/Data goes to Analog 2
#define LCD_WR A1     // LCD Write goes to Analog 1
#define LCD_RD A0     // LCD Read goes to Analog 0
#define LCD_RESET A4  // Can alternately just connect to Arduino's reset pin

// 16-bit colour values:
#define BLACK 0xFFFF
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xF87F
#define WHITE 0x0000
#define GRAY2 0xF7EF
unsigned int c[] = {BLACK,RED,GREEN,BLUE,CYAN,MAGENTA,YELLOW,WHITE};
int cc = c[1];

// Create the TFT (Graphics) object - named pins
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);



#define LCD_WIDTH             319
#define LCD_HEIGHT            239

#define MIN(X, Y)             (((X) < (Y)) ? (X) : (Y))

#define BOARD_WIDTH           11
#define BOARD_HEIGHT          20

#define BLOCK_SIZE            MIN( (LCD_WIDTH-1) / BOARD_WIDTH, (LCD_HEIGHT-1) / BOARD_HEIGHT )

#define BOARD_LEFT            118//(LCD_WIDTH - BOARD_WIDTH * BLOCK_SIZE)/3*4
#define BOARD_RIGHT           (BOARD_LEFT + BLOCK_SIZE * BOARD_WIDTH)
#define BOARD_TOP             80//(LCD_HEIGHT - BOARD_HEIGHT * BLOCK_SIZE) / 2
#define BOARD_BOTTOM          (BOARD_TOP + BOARD_HEIGHT * BLOCK_SIZE)

#define PIT_COLOR             GREEN
#define BG_COLOR              BLACK

#define DROP_WAIT_INIT        1100

#define INPUT_WAIT_ROT        300
#define INPUT_WAIT_MOVE       150

#define INPUT_WAIT_NEW_SHAPE  400

// used to clear the position from the screen
typedef struct Backup {
  byte x, y, rot;
};


class Tetris
{
  // shapes definitions
  
  byte l_shape[4][4][2] {
    {{0, 0}, {0, 1}, {0, 2}, {1, 2}},
    {{0, 1}, {1, 1}, {2, 0}, {2, 1}},
    {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
    {{0, 0}, {0, 1}, {1, 0}, {2, 0}},
  };

  byte j_shape[4][4][2] {
    {{1, 0}, {1, 1}, {0, 2}, {1, 2}},
    {{0, 0}, {1, 0}, {2, 0}, {2, 1}},
    {{0, 0}, {1, 0}, {0, 1}, {0, 2}},
    {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
  };

  byte o_shape[1][4][2] {
    { {0, 0}, {0, 1}, {1, 0}, {1, 1}}
  };

  byte s_shape[2][4][2] {
    {{0, 1}, {1, 0}, {1, 1}, {2, 0}},
    {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
  };

  byte z_shape[2][4][2] {
    {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
    {{1, 0}, {0, 1}, {1, 1}, {0, 2}}
  };

  byte t_shape[4][4][2] {
    {{0, 0}, {1, 0}, {2, 0}, {1, 1}},
    {{0, 0}, {0, 1}, {1, 1}, {0, 2}},
    {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
    {{1, 0}, {0, 1}, {1, 1}, {1, 2}}
  };

  byte i_shape[2][4][2] {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}},
    {{0, 0}, {0, 1}, {0, 2}, {0, 3}}
  };

  // All game shapes and their colors

  byte *all_shapes[7] = {l_shape[0][0], j_shape[0][0], o_shape[0][0], s_shape[0][0], z_shape[0][0], t_shape[0][0], i_shape[0][0]};

  unsigned int colors[7] = {ORANGE, BLUE, YELLOW, GREEN, RED, MAGENTA, CYAN};


  // how many rotated variations each shape has
  
  byte shapes[7] = {4, 4, 1, 2, 2, 4, 2};


  // game progress
  
  int lines, level;

  // current shapes
  
  byte current;
  
  // tetris guidelines have all 7 shapes
  // selected in sequence to avoid
  // long runs without a shape
  
  byte next[7];
  byte next_c;

  unsigned long lastInput, lastDrop;

  byte board[BOARD_HEIGHT][BOARD_WIDTH];

  byte x, y, rot;
  Backup old;

  boolean newShape;

  int dropWait;

  public:
  Tetris() : newShape(true), lines(0)
  {
  }

  void run()
  {
    randomSeed(analogRead(2));

    for ( int i = 0; i < BOARD_WIDTH; i++ )
    {
      for ( int j = 0; j < BOARD_HEIGHT; j++)
      {
        board[j][i] = 0;
      }
    }

    //next shape
    
    randomizer();

    // initialize game logic
    
    lastInput = 0;
    lastDrop = 0;
    dropWait = DROP_WAIT_INIT;
    level = 1;

    // draw background
    int c = LCD_WIDTH / 28;
    for (int i = 0; i<=28;i++)
    {
      tft.fillRect(0, i*c, LCD_HEIGHT, c, tft.color565(min((i+5)*12,255),min((i+15)*8,255),i*7));
    }

    tft.fillRect(BOARD_LEFT, BOARD_TOP, BOARD_RIGHT-BOARD_LEFT-1, BOARD_BOTTOM-BOARD_TOP, BG_COLOR);

    // draw board left limit
    
    tft.drawLine (
      BOARD_LEFT - 1,
      BOARD_TOP-1,
      BOARD_RIGHT,
      BOARD_TOP-1,
      PIT_COLOR);

    // draw board left limit
    
    tft.drawLine (
      BOARD_LEFT - 1,
      BOARD_TOP,
      BOARD_LEFT - 1,
      BOARD_BOTTOM,
      PIT_COLOR);

    // draw board right limit
    
    tft.drawLine (
      BOARD_RIGHT,
      BOARD_TOP,
      BOARD_RIGHT,
      BOARD_BOTTOM,
      PIT_COLOR);

    // draw board bottom limit
    
    tft.drawLine (
      BOARD_LEFT - 1,
      BOARD_BOTTOM,
      BOARD_RIGHT + 1,
      BOARD_BOTTOM,
      PIT_COLOR);

    for ( int i = BOARD_LEFT + BLOCK_SIZE - 1; i < BOARD_RIGHT; i += BLOCK_SIZE)
    {
      tft.drawLine (
        i,
        BOARD_TOP,
        i,
        BOARD_BOTTOM - 1,
        GRAY2);
    }

    for ( int i = BOARD_TOP + BLOCK_SIZE - 1; i < BOARD_BOTTOM; i += BLOCK_SIZE)
    {
      tft.drawLine (
        BOARD_LEFT,
        i,
        BOARD_RIGHT - 1,
        i,
        GRAY2);
    }

    scoreBoard();

    do {

      // get clock
      const unsigned long now = millis();

      // display new shape
      if ( newShape )
      {
        Joystick::waitForRelease(INPUT_WAIT_NEW_SHAPE);
        newShape = false;

        // a new shape enters the game
        chooseNewShape();

        // draw next box
        tft.fillRect(30, 100, BLOCK_SIZE * 6, BLOCK_SIZE * 5, BLACK);
        tft.drawRect(29, 99, BLOCK_SIZE * 6 + 1, BLOCK_SIZE * 5 + 1, WHITE);

        byte *shape = all_shapes[next[next_c]];
        for ( int i = 0; i < 4; i++ )
        {
          byte *block = shape + i * 2;
          tft.fillRoundRect(
            30 + BLOCK_SIZE + block[0]*BLOCK_SIZE,
            100 + BLOCK_SIZE + block[1]*BLOCK_SIZE,
            BLOCK_SIZE - 2,
            BLOCK_SIZE - 2 ,
            1,
            colors[next[next_c]]);
        }

        // check if new shape is placed over other shape(s)
        // on the board
        if ( touches(0, 0, 0 ))
        {
          // draw shape to screen
          draw();
          return;
        }

        // draw shape to screen
        draw();
      }
      else
      {
        // check if enough time has passed since last time the shape
        // was moved down the board
        if ( now - lastDrop > dropWait || Joystick::getY() > Joystick::NEUTRAL)
        {
          // update clock
          lastDrop = now;
          moveDown();
          delay(10);
        }
      }

      if (!newShape)
      {
        userInput(now);
      }

    } while (true);
  }

  private:

  void chooseNewShape()
  {
    current = next[next_c];

    next_c++;
    
    if ( next_c == 7 )
    {
      randomizer();
    }

    // new shape must be positioned at the middle of
    // the top of the board
    // with zero rotation
    
    rot = 0;
    y = 0;
    x = BOARD_WIDTH / 2;

    old.rot = rot;
    old.y = y;
    old.x = x;
  }

  void userInput(unsigned long now)
  {
    unsigned long waited = now - lastInput;

    int jx = Joystick::getX();    

    int move = INPUT_WAIT_MOVE / jx;

    if ( jx < Joystick::NEUTRAL && waited > -move)
    {
      if  (x > 0 && !touches(-1, 0, 0))
      {
        x--;
        lastInput = now;
        draw();
      }
    }
    else if ( jx > Joystick::NEUTRAL && waited > move )
    {
      if ( x < BOARD_WIDTH && !touches(1, 0, 0))
      {
        x++;
        lastInput = now;
        draw();
      }
    }
    
    if ( Joystick::fire() )
    {
      while ( !touches(0, 1, 0 ))
      {
        y++;
      }
      lastInput = now;
      draw();
    }

    int my = Joystick::getY();
    if (( my == -Joystick::HARD && waited > INPUT_WAIT_ROT ) || ( my == -Joystick::HARDER && waited > INPUT_WAIT_ROT / 2 ))
    {
      if (Joystick::getY() < 0 && !touches(0, 0, 1))
      {
        rot++;
        rot %= shapes[current];
        lastInput = now;
        draw();
      }
    }
  }

  void moveDown()
  {
    // prepare to move down
    // check if board is clear bellow
    
    if ( touches(0, 1, 0 ))
    {
      // moving down touches another shape
      newShape = true;

      // this shape wont move again
      // add it to the board
      
      byte *shape = all_shapes[current];
      for ( int i = 0; i < 4; i++ )
      {
        byte *block = (shape + (rot * 4 + i) * 2);
        board[block[1] + y][block[0] + x] = current + 1;
      }

      // check if lines were made
      
      score();
      Beeping::beep(1500, 25);
    }
    else
    {
      // move shape down
      y += 1;
      draw();
    }
  }

  void draw()
  {
    byte *shape = all_shapes[current];
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (rot * 4 + i) * 2);

      tft.fillRoundRect(
        BOARD_LEFT + block[0]*BLOCK_SIZE + BLOCK_SIZE * x,
        BOARD_TOP + block[1]*BLOCK_SIZE + BLOCK_SIZE * y,
        BLOCK_SIZE - 2,
        BLOCK_SIZE - 2 ,
        1,
        colors[current]);

      board[block[1] + y][block[0] + x] = 255;
    }

    // erase old
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (old.rot * 4 + i) * 2);

      if ( board[block[1] + old.y][block[0] + old.x] == 255 )
      continue;

      tft.fillRect(
        BOARD_LEFT + block[0]*BLOCK_SIZE + BLOCK_SIZE * old.x,
        BOARD_TOP + block[1]*BLOCK_SIZE + BLOCK_SIZE * old.y,
        BLOCK_SIZE - 2,
        BLOCK_SIZE - 2 ,
        BG_COLOR);
    }

    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (rot * 4 + i) * 2);
      board[block[1] + y][block[0] + x] = 0;
    }

    old.x = x;
    old.y = y;
    old.rot = rot;
  }

  boolean touches(int xi, int yi, int roti)
  {
    byte *shape = all_shapes[current];
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (((rot + roti) % shapes[current]) * 4 + i) * 2);

      int x2 = x + block[0] + xi;
      int y2 = y + block[1] + yi;

      if ( y2 == BOARD_HEIGHT || x2 == BOARD_WIDTH || board[y2][x2] != 0 )
      {
        return true;
      }
    }
    return false;
  }

  void score()
  {
    // we scan a max of 4 lines
    int ll;
    if ( y + 3 >= BOARD_HEIGHT )
    {
      ll = BOARD_HEIGHT - 1;
    }

    // scan board from current position
    for (int l = y; l <= ll; l++)
    {
      // check if there's a complete line on the board
      boolean line = true;

      for ( int c = 0; c < BOARD_WIDTH; c++ )
      {
        if (board[l][c] == 0)
        {
          line = false;
          break;
        }
      }

      if ( !line )
      {
        // move to next line
        continue;
      }

      Beeping::beep(3000, 50);

      lines++;

      if ( lines % 10 == 0 )
      {
        level++;
        dropWait /= 2;
      }

      scoreBoard();

      // move board down
      for ( int row = l; row > 0; row -- )
      {
        for ( int c = 0; c < BOARD_WIDTH; c++ )
        {
          byte v = board[row - 1][c];

          board[row][c] = v;
          tft.fillRoundRect(
            BOARD_LEFT + BLOCK_SIZE * c,
            BOARD_TOP + BLOCK_SIZE * row,
            BLOCK_SIZE - 2,
            BLOCK_SIZE - 2 ,
            1,
            v == 0 ? BLACK : colors[v - 1] ) ;
        }
      }

      // clear top line
      for ( int c = 0; c < BOARD_WIDTH; c++ )
      {
        board[0][c] = 0;
      }

/*      tft.fillRect(
        BOARD_LEFT,
        BOARD_TOP,
        BOARD_RIGHT - BOARD_LEFT,
        BLOCK_SIZE,
        BLACK ) ;*/
    }

    delay(350);
  }

  void scoreBoard()
  {
    tft.fillRect(6, 3, 128, 50, BLACK);
    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(YELLOW);
    tft.setCursor(20, 20);
    tft.print("Level");
    tft.setCursor(20, 40);
    tft.print("Lines");
    tft.setCursor(100, 20);
    tft.print(level);
    tft.setCursor(100, 40);
    tft.print(lines);
    tft.drawRect(5, 2, 130, 52, GREEN);
  }

  // create a sequence of 7 random shapes
  
  void randomizer()
  {
    // randomize 7 shapes
    
    for ( byte i = 0; i < 7; i ++)
    {
      boolean retry;
      byte shape;
      do
      {
        shape = random(7);
        
        // check if already in sequence
        
        retry= false;
        for ( int j = 0; j < i; j++)
        {
          if ( shape == next[j] )
          {
            retry = true;
            break;
          }
        }

      } while (retry);
      next[i] = shape;
    }
    next_c = 0;
  }  
};

void drawPreGameScreen()
{
  tft.fillScreen(BLACK);
  tft.setTextColor(BLUE);
  tft.setCursor(70,140);
  tft.println("Tetris");
  tft.setCursor(30,210);
  tft.println("Click to play");
  //tft.setCursor(50,250);
  //tft.println("Charlie.R");
}

void gameOver()
{
  tft.fillRect(16, 84, 200, 52, BLACK);
  tft.setFont(&FreeSans18pt7b);
  tft.setCursor(27,120);
  tft.setTextColor(WHITE);
  tft.print("Game Over");
  tft.drawRect(16, 84, 200, 52, CYAN);

delay(300);
/*  Beeping::beep(600, 200);
  delay(300);
  Beeping::beep(600, 200);
  delay(300);
  Beeping::beep(200, 600);
  delay(1500);*/
  Joystick::waitForClick();
}


//ISR(TIMER1_COMPA_vect) {

  // sequencer plays tetris theme
  //Sequencer::play();
//}

void setup() {

  //Sequencer::init();

  //TFT_BL_ON;      // turn on the background light

//  tft.init();  // init TFT library
  tft.reset();
  tft.begin(0x9341);  // SDFP5408
  tft.setRotation(2);
  tft.setFont(&FreeSans18pt7b);
  // play turnon sound	
  Beeping::turnOn();

  // initialize joystick
  Joystick::init();
}

void loop() {

  // title screen
  drawPreGameScreen();

  // wait a bit before playing sounds
  //delay(700);

  // start playing tetris theme
  //Sequencer::start();

  // wait for click
  while (!Joystick::fire());

  // stop playing tetris theme
  //Sequencer::stop();

  // load game
  Tetris t;
  t.run();

  // game ended
  gameOver();
}
