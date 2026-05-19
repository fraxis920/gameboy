devo risolvere dei problemi con il file proteus a causa del suo vecchio compiler c++ che non prevede funzioni come:
1) enum annidati dentro struct, puntatori e inizializazioni c++ moderne. (problema principale)
2) reference nei parametri es: void Jump(Dinovalue &Dino, unsigned long &lastJumpUpdate, float &Velocity) 
3) variabili PROGMEM troppo grandi. (essenziali per il funzionamento del programma sull'arduino)
4) Funzioni definite dentro header impliciti L 105.

soluzioni probabili:
1) spostamento degli enum annidati fuori dalle struct (piu probabile)
2) variabili PROGEM bitmap globali rispetto allo stato attuale (locale)
3) sotituzione di L 105 con "U8G2_SSD1306_128X64_NONAME_1_HW_I2C"  (meno probabile)

NOTA:
1) è probabile che il proteus fallisca ad una temporizazione corretta del progetto in quanto dovrebbe riuscire a simulare un secondo in tempo reale.
2) il file .ino è perfettamente funzionante qualcosa da aggiungere.

È importante non modificare gli array PROGMEM all’interno del file .ino per il successivo caricamento del programma, in quanto potrebbe causare errori di compilazione, 
problemi di allocazione in memoria flash o comportamenti anomali durante l’esecuzione su microcontrollore.
