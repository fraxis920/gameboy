#include <U8g2lib.h>
#include <Wire.h>
int i = 0;
enum tasti       //enum of the available buttons --enum dei pulsanti displonibili
{
  up = 3,
  down = 4,
  sx = 5,
  dx = 6,
  ent = 7,
};

enum menustate       //enum game stadium -- enum stadi gioco
{
  menu, 
  tris,
  Dino,
};

enum Cell         //tris gamespace enum -- enum campo di gioco tris
{
  EMPTY,
  X,
  O
};

enum Tris     //enum tris game stadium -- enum degli stadi del gioco tris
{
  Playing,
  Won,
  GameOver,
};



struct pintcursor   //enum cursor coordinates for tris game -- enum coordinate cursore per il gioco tris
{
  int x;
  int y;
};

struct Dinovalue    
{
  int x;                     // Dino horizontal position -- posizione orizzontale del dinosauro

  int y;                     // Dino vertical position -- posizione verticale del dinosauro

  int HitBox[2][2];          // Collision box coordinates -- coordinate della hitbox per le collisioni
                             
                             // [0][0] = top-left X -- X alto sinistra
                             // [0][1] = top-left Y -- Y alto sinistra
                             // [1][0] = bottom-right X -- X basso destra
                             // [1][1] = bottom-right Y -- Y basso destra

  enum State
  {
    Air,                     // Dino is jumping/falling -- il dinosauro è in aria

    Ground,                  // Dino is on the ground -- il dinosauro è a terra
  };

  enum JumpDirection
  {
    Up,                      // Jump movement upward -- salto verso l’alto

    Down,                    // Fall movement downward -- caduta verso il basso
  };

  JumpDirection JumpDirection;   // Current jump direction -- direzione corrente del salto

  State state;                   // Current Dino state -- stato corrente del dinosauro
};

struct Obstacle
{
  int x;                      // Obstacle horizontal position -- posizione orizzontale dell'ostacolo

  int y;                      // Obstacle vertical position -- posizione verticale dell'ostacolo

  int HitBox[2][2];           // Collision box coordinates -- coordinate della hitbox per le collisioni
                              
                              // [0][0] = top-left X -- X alto sinistra
                              // [0][1] = top-left Y -- Y alto sinistra
                              // [1][0] = bottom-right X -- X basso destra
                              // [1][1] = bottom-right Y -- Y basso destra

  enum HeightType  
  {
    Short,                    // Small obstacle height -- ostacolo basso

    Medium,                   // Medium obstacle height -- ostacolo medio

    heigh,                    // Tall obstacle height -- ostacolo alto
  };

  HeightType Height;          // Current obstacle height type -- tipo di altezza corrente dell'ostacolo
};

pintcursor coordinates = {0, 0};      // tris cursor coordinates -- cordinate cursore tris
menustate Mod = menu;                 // game state -- stato gioco

// Creates a U8G2 object for a 128x64 SSD1306 OLED display using the hardware I2C interface.
// Crea un oggetto U8G2 per un display OLED SSD1306 128x64 usando l'interfaccia I2C hardware.

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);   // SSD1306 128x64 OLED via HW I2C -- OLED SSD1306 128x64 tramite I2C hardware

void drawline(int a, int cord[3][2]) // Draws the menu cursor -- Disegna il cursore del menu
{
  u8g2.clearBuffer(); // Clears display buffer -- Pulisce il buffer del display

   switch(a) // Checks selected option -- Controlla l'opzione selezionata
   {   
    case 0: 
      displaymenu(cord); // Draws menu -- Disegna il menu
      u8g2.drawStr(cord[1][0]+35, cord[1][1], "<---"); // Draws cursor -- Disegna il cursore
      break;

    case 1: 
      displaymenu(cord); // Draws menu -- Disegna il menu
      u8g2.drawStr(cord[2][0]+45, cord[2][1], "<---"); // Draws cursor -- Disegna il cursore
      break;
   }

  u8g2.sendBuffer(); // Sends buffer to display -- Invia il buffer al display
}

void displaymenu(int cord[3][2])      //Draw menu at given cordinates -- Disegna il menu a cordinate date
{
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(cord[0][0], cord[0][1], "- menu giochi -");
  u8g2.drawStr(cord[1][0], cord[1][1], "1) tris");
  u8g2.drawStr(cord[2][0], cord[2][1], "2) Dino");
}

void displaytris(Cell gamespace[3][3], const int center[3][3][2]) // Draws tris grid -- Disegna la griglia del tris
{
  u8g2.clearBuffer(); // Clears display buffer -- Pulisce il buffer del display

  // Vertical lines -- Linee verticali
  u8g2.drawLine(42, 0, 42, 63);
  u8g2.drawLine(84, 0, 84, 63);

  // Horizontal lines -- Linee orizzontali
  u8g2.drawLine(0, 21, 127, 21);
  u8g2.drawLine(0, 42, 127, 42);  

  for (int r = 0; r < 3; r++) // Row loop -- Ciclo righe
  {
    for (int c = 0; c < 3; c++) // Column loop -- Ciclo colonne
    {
      if (gamespace[r][c] == X) // Checks X cell -- Controlla cella X
      {
        u8g2.drawStr(center[r][c][0], center[r][c][1], "x"); // Draws X -- Disegna X
      }

      if (gamespace[r][c] == O) // Checks O cell -- Controlla cella O
      {
        u8g2.drawStr(center[r][c][0], center[r][c][1], "o"); // Draws O -- Disegna O
      }
    }
  }

  u8g2.sendBuffer(); // Sends buffer to display -- Invia il buffer al display
}


void movePlayertris(Cell gamespace[3][3], const int center[3][3][2]) // Player move in tris -- Movimento giocatore nel tris
{
  while(true) // Infinite loop until move is made -- Loop infinito finché non si gioca
  {
    if(gamespace[coordinates.y][coordinates.x] == EMPTY) // Checks if current cell is empty -- Controlla se la cella è vuota
    {
        u8g2.drawStr(center[coordinates.y][coordinates.x][0], center[coordinates.y][coordinates.x][1], "x"); // Draw X cursor -- Disegna X nel cursore
        u8g2.sendBuffer(); // Refresh display -- Aggiorna display
    }

    displaytris(gamespace, center); // Redraw grid -- Ridisegna la griglia

    if(digitalRead(up) == LOW && coordinates.y != 0) // Move up if possible -- Muove su se possibile
    {   
        coordinates.y--;
    }

    if(digitalRead(down) == LOW && coordinates.y != 2) // Move down if possible -- Muove giù se possibile
    {
      coordinates.y++;
    }

    if(digitalRead(dx) == LOW && coordinates.x != 2) // Move right if possible -- Muove a destra se possibile
    {
      coordinates.x++;
    }

    if(digitalRead(sx) == LOW && coordinates.x != 0) // Move left if possible -- Muove a sinistra se possibile
    {
      coordinates.x--;
    }

    if (digitalRead(ent) == LOW && gamespace[coordinates.y][coordinates.x] == EMPTY) // Place X if is empty -- Inserisce X se è vuoto
    {
      gamespace[coordinates.y][coordinates.x] = X;    // Sets current cell to X -- Imposta la cella corrente a X
      return; // Exit after move -- Esce dopo la mossa
    }  
  }                                                               
}


// Rule-based bot AI for tic-tac-toe -- IA a regole per tris
void moveBot(Cell gamespace[3][3], const int center[3][3][2])
{
  int emptyCoordinates[2] = {-1, -1}; // Selected empty cell (y, x) -- Cella vuota selezionata (y, x)
  const int errore = 5; // Mistake probability (%) -- Probabilità di errore (%)

  const int winLines[8][3][2] = // All winning lines -- Tutte le linee vincenti
  {
    {{0,0}, {0,1}, {0,2}},
    {{1,0}, {1,1}, {1,2}},
    {{2,0}, {2,1}, {2,2}},

    {{0,0}, {1,0}, {2,0}},
    {{0,1}, {1,1}, {2,1}},
    {{0,2}, {1,2}, {2,2}},

    {{0,0}, {1,1}, {2,2}},
    {{0,2}, {1,1}, {2,0}}
  };

  for(int i = 0; i < 8; i++) // Check all lines -- Controlla tutte le linee
  {
    int x1 = winLines[i][0][1], y1 = winLines[i][0][0];
    int x2 = winLines[i][1][1], y2 = winLines[i][1][0];
    int x3 = winLines[i][2][1], y3 = winLines[i][2][0];

    int countX = 0; // X counter -- Conteggio X
    int countO = 0; // O counter -- Conteggio O

    Cell c1 = gamespace[y1][x1];
    Cell c2 = gamespace[y2][x2];
    Cell c3 = gamespace[y3][x3];

    emptyCoordinates[0] = emptyCoordinates[1] = -1; // Reset empty cell -- Reset cella vuota

    // Cell 1 -- Cella 1
    if (c1 == X) countX++;
    else if (c1 == O) countO++;
    else emptyCoordinates[0] = y1, emptyCoordinates[1] = x1;

    // Cell 2 -- Cella 2
    if (c2 == X) countX++;
    else if (c2 == O) countO++;
    else emptyCoordinates[0] = y2, emptyCoordinates[1] = x2;

    // Cell 3 -- Cella 3
    if (c3 == X) countX++;
    else if (c3 == O) countO++;
    else emptyCoordinates[0] = y3, emptyCoordinates[1] = x3;

    if (countO == 2 && emptyCoordinates[0] != -1 && random(0, 101) > errore) // Winning move -- Mossa vincente
    {
      gamespace[emptyCoordinates[0]][emptyCoordinates[1]] = O;
      return;
    }
    else if (countX == 2 && emptyCoordinates[0] != -1 && random(0, 101) > errore) // Block player -- Blocca giocatore
    {
      gamespace[emptyCoordinates[0]][emptyCoordinates[1]] = O;
      return;
    }
  }

  while(true) // Random move -- Mossa casuale 
  {
    int y = random(0,3);
    int x = random(0,3);

    if(gamespace[y][x] == EMPTY)
    {
      gamespace[y][x] = O;
      return;
    }
  }
}

// Checks if someone has won the game -- Controlla se qualcuno ha vinto
Tris ceckwinner(Cell gamespace[3][3])
{
  const int winLines[8][3][2] = // Winning combinations -- Combinazioni vincenti
  {
    {{0,0}, {0,1}, {0,2}},
    {{1,0}, {1,1}, {1,2}},
    {{2,0}, {2,1}, {2,2}},

    {{0,0}, {1,0}, {2,0}},
    {{0,1}, {1,1}, {2,1}},
    {{0,2}, {1,2}, {2,2}},

    {{0,0}, {1,1}, {2,2}},
    {{0,2}, {1,1}, {2,0}}
  };

  for(int i = 0; i < 8; i++) // Loop through lines -- Scorre le linee
  {
    int x1 = winLines[i][0][1], y1 = winLines[i][0][0];
    int x2 = winLines[i][1][1], y2 = winLines[i][1][0];
    int x3 = winLines[i][2][1], y3 = winLines[i][2][0];

    Cell c1 = gamespace[y1][x1];
    Cell c2 = gamespace[y2][x2];
    Cell c3 = gamespace[y3][x3];

    int countX = 0; // X counter -- Conteggio X
    int countO = 0; // O counter -- Conteggio O

    if (c1 == X) countX++; else if (c1 == O) countO++;
    if (c2 == X) countX++; else if (c2 == O) countO++;
    if (c3 == X) countX++; else if (c3 == O) countO++;

    if (countX == 3) return Won;       // Player wins -- Giocatore vince
    if (countO == 3) return GameOver;  // Bot wins -- Bot vince
  }

  return Playing; // No winner yet -- Nessun vincitore
}


void drawDino(int pattern, Dinovalue Dino)    // Draws the dinosaur sprite based on pattern -- Disegna il dinosauro in base al pattern
{

  //pattern 0
  const unsigned char DinoBitmap_Left[] PROGMEM = 
  {
    0x00, 0xfc, 0x01, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 
    0x3e, 0x00, 0x00, 0xfe, 0x00, 0x01, 0x1f, 0x00, 0x81, 0x1f, 0x00, 0xc3, 0x7f, 0x00, 0xe7, 0x5f, 
    0x00, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x00, 0xfe, 0x0f, 0x00, 0xfc, 0x0f, 0x00, 0xf0, 0x03, 0x00, 
    0x20, 0x03, 0x00, 0x40, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x06, 0x00
  };

  //pattern 1
  const unsigned char DinoBitmap_Right[] PROGMEM = 
  {
    0x00, 0xfc, 0x01, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 
    0x3e, 0x00, 0x00, 0xfe, 0x00, 0x01, 0x1f, 0x00, 0x81, 0x1f, 0x00, 0xc3, 0x7f, 0x00, 0xe7, 0x5f, 
    0x00, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x00, 0xfe, 0x0f, 0x00, 0xfc, 0x0f, 0x00, 0xf0, 0x03, 0x00, 
    0x60, 0x02, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00
  };

  switch(pattern) // Select animation frame -- Seleziona frame animazione
  {
    case 0: 
      u8g2.drawXBMP(Dino.x, Dino.y, 20, 20, DinoBitmap_Right); // Draw right-run dino -- Disegna dinosauro corsa-destra
      break;

    case 1: 
      u8g2.drawXBMP(Dino.x, Dino.y, 20, 20, DinoBitmap_Left); // Draw left-run dino -- Disegna dinosauro corsa-sinistra
      break;
  }
}

void drawStreet() //Draw dino game street -- disegna strada per il gioco dino
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

// Handles jump physics and animation -- Gestisce fisica e animazione del salto
void Jump(Dinovalue &Dino, unsigned long &lastJumpUpdate, float &Velocity)
{
  const float Gravity = 0.2; // Gravity force -- Forza di gravità

  const unsigned char DinoBitmap_Jump[] PROGMEM =
  { 
    0x00, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x00, 0xf6, 0x03, 0x00, 0xfe, 0x03, 0x00, 0xfe, 0x03, 0x00, 
    0xfe, 0x03, 0x00, 0x3e, 0x00, 0x00, 0xfe, 0x00, 0x04, 0x1f, 0x00, 0xc4, 0x7f, 0x00, 0xec, 0x5f, 
    0x00, 0xfc, 0x1f, 0x00, 0xfc, 0x0f, 0x00, 0xf8, 0x0f, 0x00, 0xf0, 0x07, 0x00, 0xe0, 0x06, 0x00, 
    0x60, 0x04, 0x00, 0x20, 0x04, 0x00, 0x60, 0x0c, 0x00, 0x00, 0x00, 0x00
  }; 

  const int jumpSpeed = 130; // Update delay -- Ritardo aggiornamento

  if (millis() - lastJumpUpdate < jumpSpeed) // Timing control (make the jump slower than the game tick rate) -- Controllo temporizzazione (rende il salto più lento del tick del gioco)
  { 
    return;
  }

  lastJumpUpdate = millis(); // Update timer -- Aggiorna timer

  if(Dino.state == Dinovalue::Ground) Dino.state = Dinovalue::Air; // Switch to air state -- Passa allo stato aria

  if(Dino.y >= 40) // Start upward motion -- Inizio salita
  {
    Dino.JumpDirection = Dinovalue::Up;
    Velocity = 2;
  }

  if(Dino.y <= 0) // Start downward motion -- Inizio discesa
  {
    Dino.JumpDirection = Dinovalue::Down;
    Velocity = 0;
  }

  if(Dino.JumpDirection == Dinovalue::Up) // Up movement -- Movimento verso l'alto
  {
    Dino.y -= 1 + Velocity;
    Velocity -= Gravity;
  }
  else // Down movement -- Movimento verso il basso
  {
    Dino.y += 1 + Velocity;
    Velocity += Gravity;
  }

  drawStreet(); // Draw ground -- Disegna terreno

  u8g2.drawXBMP(Dino.x, Dino.y, 20, 20, DinoBitmap_Jump); // Draw jumping sprite -- Disegna sprite salto

  if(Dino.state == Dinovalue::Air && Dino.y >= 40) Dino.state = Dinovalue::Ground; // Land detection -- Rileva atterraggio

  // Update hitbox -- Aggiorna hitbox
  Dino.HitBox[0][0] = Dino.x;
  Dino.HitBox[0][1] = Dino.y;
  Dino.HitBox[1][0] = Dino.x + 20;
  Dino.HitBox[1][1] = Dino.y + 20;
}


void ObstacleGen(Obstacle &Cactus)  // Generates obstacles -- Genera ostacoli
{
  if (Cactus.x == 0) // Respawn obstacle -- Rigenera ostacolo
  {
    Cactus.Height = (Obstacle::HeightType)random(0, 3); // Random height -- Altezza casuale
    Cactus.x = 127; // Reset position to right edge -- Reset posizione al bordo destro
  }

  switch (Cactus.Height) // Set vertical position and HitBox -- Imposta posizione verticale e Hitbox
  {
    case 0: 
      Cactus.y = 51;
      Cactus.HitBox[0][0] = Cactus.x;
      Cactus.HitBox[0][1] = Cactus.y;
      Cactus.HitBox[1][0] = Cactus.x + 10;
      Cactus.HitBox[1][1] = Cactus.y + 20;
      break;

    case 1: 
      Cactus.y = 45;
      Cactus.HitBox[0][0] = Cactus.x;
      Cactus.HitBox[0][1] = Cactus.y;
      Cactus.HitBox[1][0] = Cactus.x + 10;
      Cactus.HitBox[1][1] = Cactus.y + 20;
      break;

    case 2: 
      Cactus.y = 40;
      Cactus.HitBox[0][0] = Cactus.x;
      Cactus.HitBox[0][1] = Cactus.y;
      Cactus.HitBox[1][0] = Cactus.x + 10;
      Cactus.HitBox[1][1] = Cactus.y + 20;
      break;
  }
}


void ObstacleMove(Obstacle &Cactus) // Moves and renders obstacle -- Muove e disegna l'ostacolo
{
  Cactus.x--; // Move left -- Movimento verso sinistra

  switch(Cactus.Height) // Select obstacle type -- Seleziona tipo ostacolo
  {
    case 0:
    {
      const unsigned char Cactus1Bitmap[] PROGMEM =
      {
        0x06, 0x06, 0x0f, 0x0f, 0x0f, 0x06, 0x06, 0x06, 0x00
      };

      u8g2.drawXBMP(Cactus.x, Cactus.y, 5, 9, Cactus1Bitmap); // Draw small cactus -- Disegna cactus piccolo
      break;
    }

    case 1:
    {
      const unsigned char Cactus2Bitmap[] PROGMEM =
      {
        0x1c, 0x1c, 0x1c, 0x5c, 0x5d, 0x5d, 0x5d, 0x5d, 0x3f, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0x00
      };

      u8g2.drawXBMP(Cactus.x, Cactus.y, 8, 16, Cactus2Bitmap); // Draw medium cactus -- Disegna cactus medio
      break;
    }

    case 2:
    {
      const unsigned char Cactus3Bitmap[] PROGMEM =
      {
        0x38, 0x00,0x38, 0x00,0x38, 0x00,0x38, 0x00,0xB8, 0x01,0xBB, 0x01,0xBB, 0x01,0xBB, 0x01,0xBB, 0x01,
        0xBB, 0x01,0xFF, 0x00,0x3E, 0x00,0x38, 0x00,0x38, 0x00,0x38, 0x00, 0x38, 0x00, 0x38, 0x00
      };

      u8g2.drawXBMP(Cactus.x, Cactus.y, 10, 20, Cactus3Bitmap); // Draw large cactus -- Disegna cactus grande
      break;
    }
  }
}


bool CeckCollision(const Obstacle &Cactus, const Dinovalue &Dino) // Checks collision between dino and obstacle -- Controlla collisione tra dinosauro e ostacolo
{
  if (Dino.HitBox[1][0] > Cactus.HitBox[0][0] &&  // Right side overlap -- Sovrapposizione lato destro
      Dino.HitBox[0][0] < Cactus.HitBox[1][0] &&  // Left side overlap -- Sovrapposizione lato sinistro
      Dino.HitBox[1][1] > Cactus.HitBox[0][1] &&  // Bottom overlap -- Sovrapposizione parte bassa
      Dino.HitBox[0][1] < Cactus.HitBox[1][1])    // Top overlap -- Sovrapposizione parte alta
  {
    return true; // Collision detected -- Collisione rilevata
  }

  return false; // No collision -- Nessuna collisione
}
void setup() 
{
  Wire.setClock(400000); // Set I2C speed (max), increasing this value may cause display malfunctions or graphical glitches -- Imposta velocità I2C (massima), aumentarla può causare malfunzionamenti del display o glitch grafici

  u8g2.begin(); // Initialize OLED display -- Inizializza display OLED
  u8g2.setDrawColor(1); // Set drawing color (white) -- Imposta colore disegno (bianco)
  
  pinMode(up, INPUT_PULLUP);   // Button up -- Pulsante su
  pinMode(down, INPUT_PULLUP); // Button down -- Pulsante giù
  pinMode(sx, INPUT_PULLUP);   // Button left -- Pulsante sinistra
  pinMode(dx, INPUT_PULLUP);   // Button right -- Pulsante destra
  pinMode(ent, INPUT_PULLUP);   // Button enter -- Pulsante enter
}

void loop()
{
  
  switch(Mod)
  {
    case menu:
    {
      int cord[3][2]={{5, 10}, {5, 30}, {5, 40}};  //line arrey menu line -- righe matrice menu
      bool firstDraw = true; 

      if(firstDraw)
      {
        drawline(i, cord);
        firstDraw = false;
      }
      if (digitalRead(up) == LOW)
      {
        i--;
        if (i < 0) i = 0;
        drawline(i, cord);
        delay(150);
      }

      if (digitalRead(down) == LOW)
      {
        i++;
        if (i > 1) i = 1;
        drawline(i, cord);
        delay(150);
      }

      if (digitalRead(ent) == LOW)
      {
        if (i == 0) Mod = tris;
        if (i == 1) Mod = Dino;
        drawline(i, cord);
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
      Dinovalue Dino = {10, 40, {{10, 40}, {30, 60}}, Dinovalue::Up, Dinovalue::Ground};
      int stat = false;
      int pattern = 0;
      bool Generated;
      float Velocity = 3;
      Obstacle Cactus = {};
      static unsigned long lastJumpUpdate = 0;
      while(stat == false)
      {
        u8g2.clearBuffer();
        if(digitalRead(ent) == LOW || Dino.state == Dinovalue::Air) Jump(Dino, lastJumpUpdate, Velocity);
        drawStreet();
        drawDino(pattern, Dino);
        pattern = 1 - pattern;
        ObstacleGen(Cactus);
        ObstacleMove(Cactus);
        stat = CeckCollision(Cactus, Dino);
        u8g2.sendBuffer();
        delay(100);
      }
      u8g2.clearBuffer();
      u8g2.drawStr(30,32, "GAME OVER");
      u8g2.drawStr(45,53, "press A");
      u8g2.sendBuffer();
      delay(2000);
      Mod = menu;
      break;
    }
      
  }
}
