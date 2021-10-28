#include <arduinio.h>
#include <Wire.h>
#include <Keypad.h>
#include "DestinationManager.h"

#define MOTOR_EN_1_2  12
#define MOTOR_IN1     11
#define MOTOR_IN2     10
 
#define slow 64
#define normal 128
#define fast 255

#define delayFac  200
 
int Speed;
int startTime; 

// The I2C-address of the control-room, which is known by the floor-controllers.
#define I2C_ADDRESS 40

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

// TODO set correct pin-layout
//// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to row pinouts

// TODO set correct pin-layout
// Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins.
byte colPins[COLS] = {5, 4, 3, 2}; //connect to column pinouts

//Define the keymap
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

/*
 * Destinations kunnen ook opgeslagen worden door gebruik te maken van bit-shifting.
 * Ex. Als we willen aangeven dat we vanaf boven, naar etage 1 willen, kunnen we dat zo 
 * doen:
 * 
 * Eerst zetten we de etage:
 * byte etage = 1;
 * 
 * Dan shiften we één bit naar links:
 * etage = etage << 1;
 * 
 * Nu kunnen we de meest rechter bit (LSB) 0 laten voor naar beneden, of 1 maken voor 
 * naar boven.
 * 
 */
byte destinations[6];
byte destinationsLeft = 0;

byte currentFloor = 0;
byte destinationFloor = 0;
byte currentDirection = STATIONARY;

// Initieer de keypad.
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(MOTOR_EN_1_2, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
}

void loop() {
  // Hier moet de motor-code. De rest van de logica wordt uitgevoerd naar aanleiding van
  // signalen vanaf de etages.
  // Ook de keypad hier uitlezen.

}

/**
 * Leest data via I2C, en roept de juiste handle-functies aan.
 */
void receiveEvent(int numOfBytes) {
  // TODO handle data...
  while (Wire.available()) {
    byte data = Wire.read();
    bool functionIdentifier = data & 1; // Check of LSB 1 of 0 is.
    if (functionIdentifier) { // Als LSB 1 is, dan is de data IR-data.
      handleIRSignal(data >> 1); // Shift naar rechts om de functionIdentifier te verwijderen, en roep de handle-functie aan.
    } else { // Als LSB 0 is, dan is de data een button-press. 
      handleButtonPress(data >> 1); // Shift naar rechts om de functionIdentifier te verwijderen, en roep de handle-functie aan.
    }
  }
}

/**
 * Inserts a destination at the given index of the destinations-array.
 */
void insertInDestinationArray(int index, byte destination) {
  // Als de index groter is dan destinationsLeft, dan valt de index buiten de functionele array.
  if (index > destinationsLeft) { 
    cout << "Index is greater than amount of destinations in array." << endl;
    return;
  }

  // We maken een nieuwe array, en kopiëren daar alle waarden voor de gegeven index naartoe.
  byte newDestinations[6];
  for (auto i = 0; i < index; i++) {
    newDestinations[i] = destinations[i];
  }

  // De gegeven index is bereikt, dus zetten we de waarde erin.
  newDestinations[index] = destination;

  // We kopiëren de rest van de destinations in de destinations-array naar de kopie-array.
  for (auto i = 0; i < destinationsLeft - index; i++) {
    newDestinations[i + index] = destinations[i + index];
  }

  // We kopiëren de waardes in de kopie-array naar de destinations-array.
  for (auto i = 0; i < 6; i++) {
    destinations[i] = newDestinations[i];
  }

  // Incrementeer de hoeveelheid destinations in de array.
  destinationsLeft += 1;
}

/**
 * Removes the entry at the given index of the destinations-array.
 */
void removeAtIndex(int index) {
  // Als de index kleiner is dan destinationsLeft, dan is het een valide index.
  if (index < destinationsLeft) {
    // Vanaf de index die verwijderd moet worden, schuiven we alle waardes in de destinationsArray een plaats op naar links.
    for (auto i = index; index < 5; index++) {
      destinations[i] = destinations[i + 1];
    }
    destinations[5] = 0;
  }
  // Decrementeer de hoeveelheid
  destinationsLeft -= 1;
}

void handleIRSignal(byte data) {
  // TODO implement
  // if data == destinationFloor then
  //     Stop elevator
  //     Remove destination from array (removeAtIndex(0))
  // endif
  //
  // Send floor number to 7-segs
}

void handleButtonPress(byte data) {
  // Als de data al in de destinationArray staat, dan doen we er niks mee.
  for (auto destination : destinations) {
    if (data == destination) return;
  }
  // Data bevat al alle informatie die nodig is, in de juiste volgorde, om het in de 
  // destinations-array te zetten.
  // TODO implement algoritme om slim de volgorde van destinations te bepalen.
  insertInDestinationArray(destinationsLeft, data);
}

void readKeypad() {
  char key = keypad.getKey();
  if (key != NO_KEY) { // Dit voeren we alleen uit als er WEL een knop ingedrukt is.
    /*
     * Wanneer een case waar is, dan zal de switch alles daaronder uitvoeren, tot het een break-statement tegenkomt. Dit betekent dat we voor de waardes 
     * 0 t/m 3 maar 1 keer processKey hoeven neer te zetten, gevolgd door een break. Alle waardes daarboven zijn niet relevant voor onze lift, dus 
     * die negeren we.
     */
    switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
        processKey(key);
        break;
      default:
        break;
    }
  }
}

void processKey(char key) {
  
  byte data;
  
  switch (key) {
    case '0':
      data = 0;
      break;
    case '1':
      data = 1;
      break;
    case '2':
      data = 2;
      break;
    case '3':
      data = 3;
      break;
    default:
      break;
  }

  if (data == currentFloor) {
    return;
  }

  data = data << 1;
  data += (data > currentFloor) ? 1 : 0;
  
  handleButtonPress(data);
  
}

// Motor
void forwardRampUp(){
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  int i = 134;
  startTime = millis();
  while (i < 255) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i += 1;
    }
  }
} 

void forwardRampDown(){ 
  digitalWrite(MOTOR_IN1, HIGH); 
  digitalWrite(MOTOR_IN2, LOW);
  int i = 255;
  startTime = millis();
  while (i >= 0) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i -= 1;;
    }
  }
}
 
void backwardRampUp(){
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  int i = 134;
  startTime = millis();
  while (i < 255) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i += 1;
    }
  }
} 

void backwardRampDown(){ 
  digitalWrite(MOTOR_IN1, LOW); 
  digitalWrite(MOTOR_IN2, HIGH); 
  int i = 255;
  startTime = millis();
  while (i >= 0) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i -= 1;;
    }
  }
}
 
void brake(){
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, HIGH);
}
