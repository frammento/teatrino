/*
   Gruppo "cena mensile del martedì all'ostello" presenta:
   INTERFACCIA automazione per teatrino marionette/burattini
   scheda: Arduino Uno/Duemilanove/simili (20 in-out)
*/

/*
   CONTROLLER (con attiny84 che ha 10-11 pin disponibili)
   interruttore ON-OFF -> chiude l'alimentazione
   display 4 linee per 16 caratteri -> 6 DO
   potenziometro per modifica valori RGB -> 1 AI
   pulsante per conferma valori immessi -> 1 DI
   i2c -> 2 DO
*/


// • Sipario
#define tendaApriPin 12 // attiva un lato dell'H-Bridge
#define tendaChiudiPin 13 // attiva l'altro lato dell'H-Bridge
#define tendaApriPulsPin A0 // pulsante apertura
#define tendaChiudiPulsPin A1 // pulsante chiusura
#define tendaFinecorsaApriPin A2 // segnala completa apertura
#define tendaFinecorsaChiudiPin A3 // segnala completa chiusura
bool statoFinaleTenda = HIGH; // corrisponde a tenda chiusa

// • Luci fronte
#define luciFrontePin 7 // con circuito transistor per 12V striscia LED bianco
#define luciFrontePulsPin 8 // pulsante luci fronte
bool statoLuciFronte = LOW; // luci spente

// • Luci di scena
#define luciScenaRossoPin 3 // PWM -> collegato a circuito transistor per 12V LED rosso
#define luciScenaVerdePin 5 // PWM -> collegato a circuito transistor per 12V LED verde
#define luciScenaBluPin 6 // PWM -> collegato a circuito transistor per 12V LED blu
byte coloreLuciScena[3] = {0, 100, 0}; // valori RGB per luci spente

// • Fondale
#define luciFondaleRossoPin 9 // PWM -> collegato a circuito transistor per 12V LED rosso
#define luciFondaleVerdePin 10 // PWM -> collegato a circuito transistor per 12V LED verde
#define luciFondaleBluPin 11 // PWM -> collegato a circuito transistor per 12V LED blu
byte coloreLuciFondale[3] = {0, 0, 100}; // valori RGB per luci spente
#define servoFondalePin 4 // pin del servo che fa girare il fondale
bool avanzaServoFondale = LOW;
// • Occhio di bue
#define XservoOcchioPin 1 // pin del servo asse x
#define YservoOcchioPin 2// pin del servo asse y
byte posizioneOcchio[2] = {128, 128}; // valori a mezza scala per posizione centrale (dipende dal servo)

// • Sparabolle
#define bollePin 0  // circuito con transistor che alimenta la sparabolle elettrica
bool statoBolle = LOW;

// • Comunicazione con controller
#define i2cSDA A4 // dati
#define i2cSDC A5 // clock

// LIBRERIE
#include <Wire.h>

#include <Servo.h>
Servo servoFondale;  // crea l'oggetto servoFondale
Servo servoOcchioX;  // crea l'oggetto servoOcchioX
Servo servoOcchioY;  // crea l'oggetto servoOcchioY

// ------------------------------------------------

void setup() {
  // • Sipario
  pinMode(tendaApriPin, OUTPUT);
  pinMode(tendaChiudiPin, OUTPUT);
  pinMode(tendaApriPulsPin, INPUT_PULLUP);
  pinMode(tendaChiudiPulsPin, INPUT_PULLUP);
  pinMode(tendaFinecorsaApriPin, INPUT_PULLUP);
  pinMode(tendaFinecorsaChiudiPin, INPUT_PULLUP);

  // • Luci fronte
  pinMode(luciFrontePin, OUTPUT);
  pinMode(luciFrontePulsPin, INPUT_PULLUP);

  // • Luci di scena
  pinMode(luciScenaRossoPin, OUTPUT); // per completezza, ma inutili usando analogWrite
  pinMode(luciScenaVerdePin, OUTPUT);
  pinMode(luciScenaBluPin, OUTPUT);

  // • Fondale
  pinMode(luciFondaleRossoPin, OUTPUT); // per completezza, ma inutili usando analogWrite
  pinMode(luciFondaleVerdePin, OUTPUT);
  pinMode(luciFondaleBluPin, OUTPUT);

  servoFondale.attach(servoFondalePin);

  // • Occhio di bue
  servoOcchioX.attach(XservoOcchioPin);
  servoOcchioY.attach(YservoOcchioPin);

  // • Sparabolle
  pinMode(bollePin, OUTPUT);

  // • Comunicazione, con controller pensato come master
  Wire.begin(8); // unisciti al bus i2c con indirizzo 8
  Wire.onReceive(riceviControllo); // chiama riceviControllo se ricevi da master i2c
  Wire.onRequest(trasmettiDati);  // eseguita ogni volta che il Controllore/Master richiede dati


  // !! DEBUG !!
  Serial.begin(9600);           // avvia supporto seriale
}


void loop() {
  sipario();
  apriSipario();
  chiudiSipario();
  luciFronte();
  luciScena();
  fondale();
  // comunicazione();
  // occhio();
  sparabolle();
}


// Sipario
void sipario() {
  if (digitalRead(tendaApriPulsPin) == LOW) statoFinaleTenda = LOW;
  if (digitalRead(tendaChiudiPulsPin) == LOW) statoFinaleTenda = HIGH;
}

void apriSipario() {
  if (digitalRead(tendaFinecorsaApriPin) == HIGH && statoFinaleTenda == LOW) {
    digitalWrite (tendaApriPin, HIGH);
  }
  else {
    digitalWrite (tendaApriPin, LOW);
  }
}

void chiudiSipario() {
  if (digitalRead(tendaFinecorsaChiudiPin) == HIGH && statoFinaleTenda == HIGH) {
    digitalWrite (tendaChiudiPin, HIGH);
  }
  else {
    digitalWrite (tendaChiudiPin, LOW);
  }
}


// Luci fronte
void luciFronte() {
  if (digitalRead(luciFrontePulsPin) == LOW) { // il pulsante è stato schiacciato
    statoLuciFronte = !statoLuciFronte; // inverte lo stato della luce
    digitalWrite(luciFrontePin, statoLuciFronte);
    delay(100); // per evitare accensioni-spegnimenti in serie
  }
}


// Luci di Scena
void luciScena() {
  analogWrite (luciScenaRossoPin, coloreLuciScena[0]);
  analogWrite (luciScenaVerdePin, coloreLuciScena[1]);
  analogWrite (luciScenaBluPin, coloreLuciScena[2]);
}

// Fondale
void fondale() {
  analogWrite (luciFondaleRossoPin, coloreLuciFondale[0]);
  analogWrite (luciFondaleVerdePin, coloreLuciFondale[1]);
  analogWrite (luciFondaleBluPin, coloreLuciFondale[2]);

  if (avanzaServoFondale == HIGH) avanzaFondale();
}

void avanzaFondale() {
  servoFondale.write(20); // fa avanzare il servo di un po'
  delay(15); // aspetta un momento per il movimento
  avanzaServoFondale = LOW; // resetta variabile
}

// Comunicazione
void comunicazione() {}

void trasmettiDati() { // invia lo stato delle variabili al master
  Wire.write(statoFinaleTenda);
  Wire.write(digitalRead(tendaFinecorsaApriPin)); // segnala completa apertura
  Wire.write(digitalRead(tendaFinecorsaChiudiPin)); // segnala completa chiusura
  Wire.write(statoLuciFronte);
  Wire.write(coloreLuciScena[0]);
  Wire.write(coloreLuciScena[1]);
  Wire.write(coloreLuciScena[2]);
  Wire.write(coloreLuciFondale[0]);
  Wire.write(coloreLuciFondale[1]);
  Wire.write(coloreLuciFondale[2]);
  Wire.write(avanzaServoFondale);
  Wire.write(posizioneOcchio[0]);
  Wire.write(posizioneOcchio[1]);
  Wire.write(statoBolle);
}

void riceviControllo(int r) {
  while (1 < Wire.available()) { // loop through all but the last
    statoFinaleTenda = Wire.read();
    statoLuciFronte = Wire.read();
    coloreLuciScena[0] = Wire.read();
    coloreLuciScena[1] = Wire.read();
    coloreLuciScena[2] = Wire.read();
    coloreLuciFondale[0] = Wire.read();
    coloreLuciFondale[1] = Wire.read();
    coloreLuciFondale[2] = Wire.read();
    avanzaServoFondale = Wire.read();
    posizioneOcchio[0] = Wire.read();
    posizioneOcchio[1] = Wire.read();
    statoBolle = Wire.read();
  }
}

// Occhio di bue
void occhio() {}

// Sparabolle
void sparabolle() {
  // digitalWrite(bollePin, HIGH);
}

