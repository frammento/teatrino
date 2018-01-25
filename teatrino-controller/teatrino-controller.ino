/*
   Gruppo "cena mensile del martedi' all'ostello" presenta:
   CONTROLLER automazione per teatrino marionette/burattini
   scheda: Arduino Uno/Duemilanove/simili (20 porte in-out)
    oppure con attiny84 (11 porte in-out)
*/

/*  I/O
    interruttore ON-OFF -> chiude l'alimentazione
    display 2-4 linee per 20 caratteri -> 6 DO
    potenziometro per modifica valori RGB -> 1 AI
    pulsanti per selezione/conferma valori -> 2 DI
    i2c -> 2 DO
*/

/* Comunicazione con master via i2c, 1 byte alla volta

  BOOL statoFinaleTenda
  BOOL statoLuciFronte
  BOOL avanza fondale
  BOOL sparabolle

  BYTE coloreLuciScena[3]
  BYTE coloreLuciFondale[3]
  BYTE xocchio
  BYTE yocchio
*/

#define confermaPin 1 // pin del pulsante conferma
#define tornaPin 2 // pin del pulsante torna/annulla

// TESTO
const char testoIntro[] = "TEATRINO";

// -- Sipario
bool statoFinaleTenda = HIGH; // corrisponde a tenda chiusa

// -- Luci fronte
bool statoLuciFronte = LOW; // luci spente

// -- Luci di scena
byte coloreLuciScena[3] = {0, 0, 0}; // valori RGB per luci spente

// -- Fondale
byte coloreLuciFondale[3] = {0, 0, 0}; // valori RGB per luci spente
bool avanzaServoFondale = LOW;

// -- Occhio di bue
byte posizioneOcchio[2] = {128, 128}; // valori a mezza scala per posizione centrale (dipende dal servo)

// -- Sparabolle
bool statoBolle = LOW;

// -- Comunicazione con interfaccia
#define i2cSDA A4 // dati
#define i2cSDC A5 // clock

// LIBRERIE
#include <Wire.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // inizializza oggetto LCD con i pin di controllo

byte on[8] = { // rappresentazione bitmap simbolo "on" (5 colonne, 7 righe)
  0b00000,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000,
  0b00000
};

byte off[8] = { // rappresentazione bitmap simbolo "off"
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};


void setup() {
  pinMode(confermaPin, INPUT_PULLUP); // pulsante per confermare le scelte
  pinMode(tornaPin, INPUT_PULLUP); // pulsante per tornare indietro

  lcd.begin(20, 2); // definisce il numero di colonne e righe dell'LCD
  lcd.createChar(1, on); // aggiunge il simbolo custom "on"
  lcd.createChar(2, off); // aggiunge il simbolo custom "off"

  initLCD();

}

void loop() {
  menu();
  int valorePot = analogRead(A0); // legge il valore del potenziometro (0-1023)

}

void initLCD() {
  lcd.clear(); // cancella lcd
  lcd.setCursor(0, 0); // prima colonna, prima riga
  lcd.print(testoIntro);
  delay(1000); // aspetta per un secondo
  lcd.setCursor(0, 1); // prima colonna, seconda riga
  lcd.print("Off -> ");
  lcd.write(2); // aggiunge simbolo "off"
  delay(300); // aspetta per 0,3s
  lcd.print("   On -> ");
  lcd.write(1); // aggiunge simbolo "on"
  delay(2000); // aspetta 2 secondi
}

void menu() {
  int sceltaMenu = map(analogRead(A0), 0, 1025, 0, 6); // legge il valore del potenziometro (0-1023) e lo trasforma in 0-6
  switch (sceltaMenu) {
    case 0:

      break;

    // ........

    case 6:

      break;
  }
}
