#include <U8g2lib.h>
#include <Wire.h>
int i = 0;
enum tasti       //enumerazione dei pulsanti displonibili
{
  up = 3,
  down = 4,
  sx = 5,
  dx = 6,
  ent = 7,
};

enum menustate       //enumerazioni stati
{
  menu, 
  tris,
  Dino,
};

enum Cell 
{
  EMPTY,
  X,
  O
};

enum Tris
{
  Playing,
  Won,
  GameOver,
};



struct pintcursor
{
  int x;
  int y;
};

struct Dinovalue
{
  int x;
  int y;
  enum State
  {
    Air,
    Alive,
    Dead,
    Ground,
  };
  State state;
};

pintcursor coordinates = {0, 0};
menustate Mod = Dino;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 9, 8, U8X8_PIN_NONE);   //inizializazione pin I2C
int cord[3][2]={{5, 10}, {5, 30}, {5, 40}};  //matrice cordinate


void drawline(int a)
{
  u8g2.clearBuffer();
   switch(a)
   {   
    case 0: displaymenu(); u8g2.drawStr(cord[1][0]+35, cord[1][1], "<---");  break;
    case 1: displaymenu(); u8g2.drawStr(cord[2][0]+45, cord[2][1], "<---");  break;
   }
  u8g2.sendBuffer();
}

void displaymenu()      //draw menu
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(cord[0][0], cord[0][1], "- menu giochi -");
  u8g2.drawStr(cord[1][0], cord[1][1], "1) tris");
  u8g2.drawStr(cord[2][0], cord[2][1], "2) Dino");
  u8g2.sendBuffer();
}

void displaytris(Cell gamespace[3][3], const int center[3][3][2])     //draw tris gamespace
{
  u8g2.clearBuffer();

  // linee verticali (3 colonne)
  u8g2.drawLine(42, 0, 42, 63);
  u8g2.drawLine(84, 0, 84, 63);

  // linee orizzontali (3 righe)
  u8g2.drawLine(0, 21, 127, 21);
  u8g2.drawLine(0, 42, 127, 42);  
  for (int r = 0; r < 3; r++) 
  {
    for (int c = 0; c < 3; c++) 
    {
      if (gamespace[r][c] == X) 
      {
        u8g2.drawStr(center[r][c][0], center[r][c][1], "x");
      }
      if (gamespace[r][c] == O) 
      {
        u8g2.drawStr(center[r][c][0], center[r][c][1], "o");
      }
    }
  }
  u8g2.sendBuffer();

}


void movePlayertris(Cell gamespace[3][3], const int center[3][3][2])      //tris move, player side
{

  coordinates.y = 0;
  coordinates.x = 0;
  while(true)
  {
    if(gamespace[coordinates.y][coordinates.x] == EMPTY)                                                            //check if the cell in the current cursor position is already displayng something
    {
        u8g2.drawStr(center[coordinates.y][coordinates.x][0], center[coordinates.y][coordinates.x][1], "x");        //drawing the char x in the cursor position
        u8g2.sendBuffer();                                                                                          // send buffer to display (render cursor)
    }                                                                                                               // redraw the gamespace (clears and refreshes screen)
        displaytris(gamespace, center);

    if(digitalRead(up) == LOW && coordinates.y != 0)                                                                // Move cursor UP if button is pressed and not at top boundary 
    {   
        coordinates.y--;
    }
    if(digitalRead(down) == LOW && coordinates.y != 2)                                                               // Move cursor DOWN if button is pressed and not at bottom boundary
    {
      coordinates.y++;
    }
    if(digitalRead(dx) == LOW && coordinates.x != 2)                                                                // Move cursor RIGHT if button is pressed and not at right boundary
    {
      coordinates.x++;
    }
    if(digitalRead(sx) == LOW && coordinates.x != 0)                                                                //Move cursor LEFT if button is pressed and not at left boundary
    {
      coordinates.x--;
    }
    if (digitalRead(ent) == LOW && gamespace[coordinates.y][coordinates.x] == EMPTY)                                //If ENTER is pressed and the cell is empty:
    {
      drawx(gamespace, center); 
      return;                                                                                                       // place the char x and return the updated cell value
    }  
  }                                                               
}

Cell drawx(Cell gamespace[3][3], const int center[3][3][2])     
{
  // Draws an "X" on the OLED display at the current cursor position.
  // center[y][x][0] = X pixel coordinate
  // center[y][x][1] = Y pixel coordinate

  u8g2.drawStr(center[coordinates.y][coordinates.x][0], center[coordinates.y][coordinates.x][1], "x");            //drawing the char x in the cursor position                                     
 
  u8g2.sendBuffer();                                                                                              // update the display (render to screen)

  gamespace[coordinates.y][coordinates.x] = X;                                                                    // Update the logical game grid and mark the current cell in gamespace as occupied by X

  return gamespace[coordinates.y][coordinates.x];                                                                 // Return the value of the updated cell
}

void moveBot(Cell gamespace[3][3], const int center[3][3][2])     //rule-based AI 
{
  int emptyCoordinates[2] = {-1, -1};                                                                              // Stores the selected empty cell coordinates (y, x)
  const int errore = 5;                                                                                            // Chance of making a mistake in percentage logic
  const int winLines[8][3][2] =                                                                                    // All possible winning combinations (rows, columns, diagonals)
  {
    {{0,0}, {0,1}, {0,2}},  // row 1
    {{1,0}, {1,1}, {1,2}},  // row 2
    {{2,0}, {2,1}, {2,2}},  // row 3

    {{0,0}, {1,0}, {2,0}},  // column 1
    {{0,1}, {1,1}, {2,1}},  // column 2
    {{0,2}, {1,2}, {2,2}},  // column 3

    {{0,0}, {1,1}, {2,2}},  // diagonal 1
    {{0,2}, {1,1}, {2,0}}   // diagonal 2
  };
  for(int i=0; i<8; i++)                                                                                            //Iterate through all possible winning lines  
  {
    // Extract coordinates of the 3 cells in the current line
    int x1 = winLines[i][0][1];                           
    int y1 = winLines[i][0][0];

    int x2 = winLines[i][1][1];
    int y2 = winLines[i][1][0];

    int x3 = winLines[i][2][1];
    int y3 = winLines[i][2][0];

    // Counters for X and O in the current line
    int countX = 0;
    int countO = 0;

    // Read Cell gamespace values
    Cell c1 = gamespace[y1][x1];
    Cell c2 = gamespace[y2][x2];
    Cell c3 = gamespace[y3][x3];

    // Evaluate first cell
    if (c1 == X)
    {
      countX++;
    }
    else if (c1 == EMPTY)
    {
      emptyCoordinates[0] = y1;
      emptyCoordinates[1] = x1;
    }
    else if (c1 == O) 
    {
      countO++;
    }

    // Evaluate second cell
    if (c2 == X)
    {
      countX++;
    }
    else if (c2 == EMPTY)
    {
      emptyCoordinates[0] = y2;
      emptyCoordinates[1] = x2;
    }
    else if (c2 == O) 
    {
      countO++;
    }

    // Evaluate third cell
    if (c3 == X)
    {
      countX++;
    }
    else if (c3 == EMPTY)
    {
      emptyCoordinates[0] = y3;
      emptyCoordinates[1] = x3;
    }
    else if (c3 == O) 
    {
      countO++;
    }
    if (countO == 2 && emptyCoordinates[0] != -1 && random(0, 101) >errore)                                                 //If two O in a line and one empty cell, place O in the EMPTY spot(win move)
    {
      drawO(emptyCoordinates, center);
      gamespace[emptyCoordinates[0]][emptyCoordinates[1]] = O;
      return;
    }
    else if (countX == 2 && emptyCoordinates[0] != -1 && random(0, 101) >errore)                                            //If two X in a line and one empty cell, place O in the EMPTY spot (counter move)
    {
      drawO(emptyCoordinates, center);
      gamespace[emptyCoordinates[0]][emptyCoordinates[1]] = O;
      return;
    }
  }
      while(true)                                                                                                           // If no strategic move is found, choose a random empty cell
      {
        int y = random(0,3);
        int x = random(0,3);
        if(gamespace[y][x] == EMPTY)
        {
          emptyCoordinates[0] = y;
          emptyCoordinates[1] = x;
          drawO(emptyCoordinates, center);
          gamespace[emptyCoordinates[0]][emptyCoordinates[1]] = O;
          return;
        }
      } 
}

void drawO(int emptyCoordinates[2], const int center[3][3][2])                                           
{
  // Draws an "O" on the OLED display at the current cursor position.
  // center[y][x][0] = X pixel coordinate
  // center[y][x][1] = Y pixel coordinate

  u8g2.drawStr(center[emptyCoordinates[0]][emptyCoordinates[1]][0], center[emptyCoordinates[0]][emptyCoordinates[1]][1], "O");                                             //draw o in given cordinates
  u8g2.sendBuffer();                                                                                                                                                       //send buffer to oled display                                                                                                                                                
                                                               
}

Tris ceckwinner(Cell gamespace[3][3])     //ceck if someone won
{
  const int winLines[8][3][2] =                                                                                    // All possible winning combinations (rows, columns, diagonals)
  {
    {{0,0}, {0,1}, {0,2}},  // row 1
    {{1,0}, {1,1}, {1,2}},  // row 2
    {{2,0}, {2,1}, {2,2}},  // row 3

    {{0,0}, {1,0}, {2,0}},  // column 1
    {{0,1}, {1,1}, {2,1}},  // column 2
    {{0,2}, {1,2}, {2,2}},  // column 3

    {{0,0}, {1,1}, {2,2}},  // diagonal 1
    {{0,2}, {1,1}, {2,0}}   // diagonal 2
  };
  for(int i=0; i<8; i++)                                                                                            //Iterate through all possible winning lines  
  {
    // Extract coordinates of the 3 cells in the current line
    int x1 = winLines[i][0][1];                           
    int y1 = winLines[i][0][0];

    int x2 = winLines[i][1][1];
    int y2 = winLines[i][1][0];

    int x3 = winLines[i][2][1];
    int y3 = winLines[i][2][0];

    // Counters for X and O in the current line
    int countX = 0;
    int countO = 0;

    // Read Cell gamespace values
    Cell c1 = gamespace[y1][x1];
    Cell c2 = gamespace[y2][x2];
    Cell c3 = gamespace[y3][x3];

    if (c1 == X) countX++;
    else if (c1 == O) countO++;
    if (c2 == X) countX++;
    else if (c2 == O) countO++;
    if (c3 == X) countX++;
    else if (c3 == O) countO++;
    if(countX == 3) return Won;
    else if(countO == 3) return GameOver;
  }
  return Playing;
}

void drawDino(int pattern, Dinovalue Dino)
{
  const unsigned char DinoBitmap_Left[] PROGMEM = 
  {
	0x00, 0xfc, 0x01, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 
	0x3e, 0x00, 0x00, 0xfe, 0x00, 0x01, 0x1f, 0x00, 0x81, 0x1f, 0x00, 0xc3, 0x7f, 0x00, 0xe7, 0x5f, 
	0x00, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x00, 0xfe, 0x0f, 0x00, 0xfc, 0x0f, 0x00, 0xf0, 0x03, 0x00, 
	0x20, 0x03, 0x00, 0x40, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x06, 0x00
  };
  const unsigned char DinoBitmap_Right [] PROGMEM = 
  {
	0x00, 0xfc, 0x01, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 
	0x3e, 0x00, 0x00, 0xfe, 0x00, 0x01, 0x1f, 0x00, 0x81, 0x1f, 0x00, 0xc3, 0x7f, 0x00, 0xe7, 0x5f, 
	0x00, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x00, 0xfe, 0x0f, 0x00, 0xfc, 0x0f, 0x00, 0xf0, 0x03, 0x00, 
	0x60, 0x02, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00
  };
  switch(pattern)
  {
    case 0: u8g2.drawXBMP(Dino.x, Dino.y, 20, 20, DinoBitmap_Right); break;
    case 1: u8g2.drawXBMP(Dino.x, Dino.y, 20, 20, DinoBitmap_Left); break;
  }
}

void drawStreet()
{
  const unsigned char StreetBitmap[] PROGMEM =
  { 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0c, 0x00, 0x06, 0xc0, 0x09, 0x00, 0x01, 0x80, 0x01, 0x30, 0x00, 0x06, 0xc0, 0x0d, 0x80, 0x00, 
	0x63, 0x9e, 0xb1, 0x37, 0xf7, 0x77, 0xfb, 0x7e, 0xf6, 0x8e, 0xbd, 0xb1, 0x3b, 0xf3, 0xbb, 0x7b, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  u8g2.drawXBMP(0, 23, 127, 50, StreetBitmap);
}

void Jump(Dinovalue Dino)
{
  const unsigned char DinoBitmap_Jump[] PROGMEM =
  { 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 
	0xfc, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x7c, 0x00, 0x10, 0x1e, 0x00, 0x30, 0x3f, 0x00, 0xb0, 0x1f, 
	0x00, 0xe0, 0x0f, 0x00, 0xc0, 0x0f, 0x00, 0x80, 0x07, 0x00, 0x00, 0x05, 0x00, 0x00, 0x05, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  for(Dino.y; Dino.y>20; Dino.y--)
  {
    u8g2.clearBuffer();
    drawStreet();
    u8g2.drawXBMP(Dino.x, Dino.y, 20, 20, DinoBitmap_Jump);
    u8g2.sendBuffer();
    delay(20);
  }
  for(Dino.y; Dino.y<40; Dino.y++)
  {
    u8g2.clearBuffer();
    drawStreet();
    u8g2.drawXBMP(Dino.x, Dino.y, 20, 20, DinoBitmap_Jump);
    u8g2.sendBuffer();
    delay(20);
  }
}

void setup() 
{
  
  u8g2.begin();                       //initialize oled display
  u8g2.setDrawColor(1);               //set the drawcolor to fill
  pinMode(up, INPUT_PULLUP);          //declearing every button 
  pinMode(down, INPUT_PULLUP);
  pinMode(sx, INPUT_PULLUP);
  pinMode(dx, INPUT_PULLUP);
  pinMode(ent, INPUT_PULLUP);

}

void loop()
{
  switch(Mod)
  {
    case menu:
    {
      drawline(i);

      if (digitalRead(up) == LOW)
      {
        i--;
        if (i < 0) i = 0;
        delay(150);
      }

      if (digitalRead(down) == LOW)
      {
        i++;
        if (i > 1) i = 1;
        delay(150);
      }

      if (digitalRead(ent) == LOW)
      {
        if (i == 0) Mod = tris;
        if (i == 1) Mod = Dino;
        delay(150);
      }

      break;
    }

    case tris:
    {
      
      Tris State = Playing;
      Cell gamespace[3][3] = 
      {
      {EMPTY, EMPTY, EMPTY},
      {EMPTY, EMPTY, EMPTY},
      {EMPTY, EMPTY, EMPTY}
      };                
      const int center[3][3][2] = 
      {
      { {21,10}, {63,10}, {105,10} },
      { {21,32}, {63,32}, {105,32} },
      { {21,53}, {63,53}, {105,53} }
      };
      displaytris(gamespace, center);
      while(State == Playing)
      {
        movePlayertris(gamespace, center);
        State = ceckwinner(gamespace);
        moveBot(gamespace, center);
        State = ceckwinner(gamespace);
        delay(100);
      }
      u8g2.clearBuffer();
      if(State == Won) u8g2.drawStr(center[1][0][1],center[1][0][0]+10, "You won");     
      else u8g2.drawStr(center[1][0][1],center[1][0][0]+10, "You lose");
      u8g2.sendBuffer();
      delay(1000);
      break;
    }

    case Dino:
    {
      Dinovalue Dino = {10, 40, Dinovalue::Alive};
      int pattern = 0;
      for(int i=0; i<8; i++)
      {
        if(digitalRead(ent) == LOW) Jump(Dino);
        u8g2.clearBuffer();
        drawStreet();
        drawDino(pattern, Dino);
        u8g2.sendBuffer();
        pattern = 1 - pattern;
        delay(10);
      }
    }
      break;
    }
  }


                                                                      /*(0,0) ---------------------> X (127,0)
                                                                        |
                                                                        |
                                                                        |
                                                                        |
                                                                        v
                                                                      Y (0,63)*/



