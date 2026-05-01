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
mod modalita = menu;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 9, 8, U8X8_PIN_NONE);   //inizializazione pin I2C
int cord[3][2]={{5, 10}, {5, 30}, {5, 40}};  //matrice cordinate


void drawline(int a)
{
   switch(a)
   {
    case 1: u8g2.drawStr(cord[1][0]+32, cord[1][1], "<---"); u8g2.setDrawColor(0); u8g2.drawStr(cord[2][0]+40, cord[2][1], "<---"); break;
    case 2: u8g2.drawStr(cord[2][0]+40, cord[2][1], "<---"); u8g2.setDrawColor(0); u8g2.drawStr(cord[1][0]+32, cord[1][1], "<---"); break;
    default: break;
   }
   u8g2.setDrawColor(1);
}

void displaymenu()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(cord[0][0], cord[0][1], "- menu giochi -");
  u8g2.drawStr(cord[1][0], cord[1][1], "1) tris");
  u8g2.drawStr(cord[2][0], cord[2][1], "2) dino");
  u8g2.drawStr(cord[1][0]+32, cord[1][1], "<---");
  u8g2.sendBuffer();
}

void displaytris()
{
  u8g2.clearBuffer();

  // linee verticali (3 colonne)
  u8g2.drawLine(42, 0, 42, 63);
  u8g2.drawLine(84, 0, 84, 63);

  // linee orizzontali (3 righe)
  u8g2.drawLine(0, 21, 127, 21);
  u8g2.drawLine(0, 42, 127, 42);

  u8g2.sendBuffer();
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

      displaymenu();
      drawline(i);

      if (digitalRead(su) == LOW)
      {
        i--;
        if (i < 0) i = 1;
        delay(150);
      }

      if (digitalRead(giu) == LOW)
      {
        i++;
        if (i > 1) i = 0;
        delay(150);
      }

      if (digitalRead(ent) == LOW)
      {
        if (i == 0) modalita = tris;
        if (i == 1) modalita = dino;
        delay(150);
      }

      break;

    case tris:
      displaytris();

      break;

    case dino:
      // qui chiamerai gioco dino
      break;
  }
}

