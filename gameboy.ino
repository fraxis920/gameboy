#include <U8g2lib.h>
int i = 0;
enum tasti       //enumerazione dei pulsanti displonibili
{
  su = 3,
  giu = 4,
  sx = 5,
  dx = 6,
  ent = 7,
};

enum mod       //enumerazioni stati
{
  menu, 
  tris,
  dino,
};

enum Cell {
  EMPTY,
  X,
  O
};

struct pintcursor {
  int x;
  int y;
};
pintcursor coordinates = {0, 0};

mod modalita = menu;

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

void displaymenu()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(cord[0][0], cord[0][1], "- menu giochi -");
  u8g2.drawStr(cord[1][0], cord[1][1], "1) tris");
  u8g2.drawStr(cord[2][0], cord[2][1], "2) dino");
  u8g2.sendBuffer();
}

void displaytris(Cell gamespace[3][3], int center[3][3][2])
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

void drawcircle()
{
  //u8g2.drawCircle(x, y, r);
}

void moveplayertris(Cell gamespace[3][3], int center[3][3][2])
{
  if(gamespace[coordinates.y][coordinates.x] == EMPTY)                                                            //check if the cell in the current cursor position is already displayng something
  {
      u8g2.drawStr(center[coordinates.y][coordinates.x][0], center[coordinates.y][coordinates.x][1], "x");        //drawing the char x in the cursor position
      u8g2.sendBuffer();                                                                                          // send buffer to display (render cursor)
  }                                                                                                               // redraw the gamespace (clears and refreshes screen)
      displaytris(gamespace, center);

  if(digitalRead(su) == LOW && coordinates.y != 0)                                                                // Move cursor UP if button is pressed and not at top boundary 
  {   
      coordinates.y--;
  }
  if(digitalRead(giu) == LOW && coordinates.y != 2)                                                               // Move cursor DOWN if button is pressed and not at bottom boundary
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
    drawx(gamespace, center);                                                                                     // place the char x and return the updated cell value
  }                                                                 
}

Cell drawx(Cell gamespace[3][3], int center[3][3][2])
{
  // Draws an "X" on the OLED display at the current cursor position.
  // center[y][x][0] = X pixel coordinate
  // center[y][x][1] = Y pixel coordinate

  u8g2.drawStr(center[coordinates.y][coordinates.x][0], center[coordinates.y][coordinates.x][1], "x");            //drawing the char x in the cursor position                                     
 
  u8g2.sendBuffer();                                                                                              // update the display (render to screen)

  gamespace[coordinates.y][coordinates.x] = X;                                                                    // Update the logical game grid and mark the current cell in gamespace as occupied by X

  return gamespace[coordinates.y][coordinates.x];                                                                 // Return the value of the updated cell
}


void setup() 
{
  u8g2.begin();
  u8g2.setDrawColor(1);
  pinMode(su, INPUT_PULLUP);
  pinMode(giu, INPUT_PULLUP);
  pinMode(sx, INPUT_PULLUP);
  pinMode(dx, INPUT_PULLUP);
  pinMode(ent, INPUT_PULLUP);

}

void loop()
{
  switch(modalita)
  {
    case menu:
    {
      drawline(i);

      if (digitalRead(su) == LOW)
      {
        i--;
        if (i < 0) i = 0;
        delay(150);
      }

      if (digitalRead(giu) == LOW)
      {
        i++;
        if (i > 1) i = 1;
        delay(150);
      }

      if (digitalRead(ent) == LOW)
      {
        if (i == 0) modalita = tris;
        if (i == 1) modalita = dino;
        delay(150);
      }

      break;
    }

    case tris:
    {
      
      bool gamestatus=true;
      Cell gamespace[3][3] = 
      {
      {EMPTY, EMPTY, EMPTY},
      {EMPTY, X, EMPTY},
      {EMPTY, O, EMPTY}
      };                
      int center[3][3][2] = 
      {
      { {21,10}, {63,10}, {105,10} },
      { {21,32}, {63,32}, {105,32} },
      { {21,53}, {63,53}, {105,53} }
      };
      displaytris(gamespace, center);
      while(gamestatus)
      {
        moveplayertris(gamespace, center);
      }

      break;
    }

    case dino:
    {
      // qui chiamerai gioco dino
      break;
    }
  }
}

                                                                      /*(0,0) ---------------------> X (127,0)
                                                                        |
                                                                        |
                                                                        |
                                                                        |
                                                                        v
                                                                      Y (0,63)*/



