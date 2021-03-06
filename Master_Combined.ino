#include <Wire.h>
#include <Keypad.h>

#define MOTOR_EN_1_2  12
#define MOTOR_IN1     11
#define MOTOR_IN2     10

#define delayFac  200

// The I2C-address of the control-room, which is known by the floor-controllers.
#define I2C_ADDRESS 40

#define STATIONARY 0
#define GOING_UP 1
#define GOING_DOWN -1
//#define 

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

// 
uint8_t Speed;
long startTime; 
uint32_t timeToGo;

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
uint8_t destinations[6];
uint8_t destinationsLeft = 0;

uint8_t currentFloor = 0;
uint8_t destinationFloor = 0;
int8_t currentDirection = STATIONARY;

// Initieer de keypad.
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  
  pinMode(MOTOR_EN_1_2, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  
//  setupTimerInterrupt(15);

  timeToGo = millis() + 5000; // 

  insertInDestinationArray(0, 0); // Set current-floor to 0
}

void loop() {
  // Hier moet de motor-code. De rest van de logica wordt uitgevoerd naar aanleiding van
  // signalen vanaf de etages.
  // Ook de keypad hier uitlezen.
  if (currentDirection == STATIONARY && (millis() > timeToGo) && destinationsLeft > 0) {
    destinationFloor = destinations[0];
    if (destinationFloor > currentFloor) {
      setCurrentDirection(GOING_UP);
      forwardRampUp();
    } else {
      setCurrentDirection(GOING_DOWN);
      backwardRampUp();
    }
  }
  
}

void setCurrentDirection(byte cDirection) {
  currentDirection = cDirection;
  byte data = 0;
  data += destinationFloor;
  data = data << 1;
  sendI2CData(data);
}

void sendI2CData(int data) {
  Wire.beginTransmission(0);
  Wire.write(data);
  Wire.endTransmission();
}

/**
 * Leest data via I2C, en roept de juiste handle-functies aan.
 */
void receiveEvent(int numOfBytes) {
  // TODO handle data...
//  cout << "Data received..." << endl;
Serial.println("Data received...");
  while (Wire.available()) {
    byte data = Wire.read();
//    cout << data << endl;
    Serial.println(data);
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
//    cout << "Index is greater than amount of destinations in array." << endl;
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
  if (data == destinationFloor) {
    switch(currentDirection) {
      case 0:
        break;
      case 1:
        forwardRampDown();
        break;
      case -1:
        backwardRampDown();
        break;
    }
    timeToGo = millis() + 5000;
    removeAtIndex(0);
    currentDirection = STATIONARY;
    sendI2CData((data << 1) + 1);
  }

  
  
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

void processKey(char key) {
  
  byte keypad_data = currentFloor;
  
  switch (key) {
    case '0':
      keypad_data = 0;
      break;
    case '1':
      keypad_data = 1;
      break;
    case '2':
      keypad_data = 2;
      break;
    case '3':
      keypad_data = 3;
      break;
    default:
      break;
  }

  if (keypad_data == currentFloor) {
    return;
  }

  keypad_data = keypad_data << 1;
  keypad_data += (keypad_data > currentFloor) ? 1 : 0;
  
  handleButtonPress(keypad_data);
  
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

void setupTimerInterrupt(int freq) {

  TCCR3A = 0;
  TCCR3B = 0;
  TCNT3 = 0;

  TCCR3B |= (1 << WGM12);

  TCCR3B |= (1 << CS12) | (1 << CS10);

  OCR3A = 15625 / freq - 1;

  TIMSK3 |= (1 << OCIE3A);
  
}

ISR(TIMER3_COMPA_vect) {
  processKey(keypad.getKey());
}
