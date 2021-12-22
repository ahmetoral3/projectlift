#include <Wire.h>
#include "constants.hpp" 

byte currentFloor = 6; // Used to determine which number to show on the 7-segment display.
byte destinationFloor = 0;
byte currentDirection = STATIONARY; // Used to determine which indicator LED to turn on.

bool liftDetectedBySensor = false;
bool downButtonPressed = false;
bool upButtonPressed = false;

void setup() {

//  Serial.begin(9600);

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  
  pinMode(SENSOR_DATA, INPUT);
  
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_UP_LED, OUTPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_DOWN_LED, OUTPUT);
  
  pinMode(LED_DOWN, OUTPUT);
  pinMode(LED_UP, OUTPUT);

  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);

  TWAR = (I2C_ADDRESS << 1) | 1; //  Enable receiving broadcasts from master.

//   cli();
//   attachInterrupt(digitalPinToInterrupt(BUTTON_UP), buttonUpInterrupt, FALLING);
//   attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN), buttonDownInterrupt, FALLING);
//   sei();

//  changeNumberDisplay(thisFloor);

//  digitalWrite(LED_UP, HIGH);
//  digitalWrite(LED_DOWN ,HIGH);
//  digitalWrite(BUTTON_UP_LED, HIGH);
//  digitalWrite(BUTTON_DOWN_LED, HIGH);

  // I2C_ADDRESS = 00000001
  // I2C << 1 = 00000010
  // I2C | 1 = 00000010 | 00000001 = 00000011

  changeNumberDisplay(3);
  
}

void loop() {
  //TODO implement cooldown for IR-read.
  // Als de lift gedetecteerd wordt, door de sensor, dan sturen we die data naar de machine-kamer.
//    cout << "Lift detected..." << endl;
    if (!digitalRead(SENSOR_DATA) == 0) {
      currentFloor = thisFloor;
      sendDataToControlRoom((thisFloor << 1) + 1);
      changeNumberDisplay(thisFloor);
    }
    

  if (downButtonPressed) {
    digitalWrite(BUTTON_DOWN_LED, HIGH);
    sendDataToControlRoom(thisFloor << 2);
    downButtonPressed = false;
  }
  
  if (upButtonPressed) {
    digitalWrite(BUTTON_UP_LED, HIGH);
    byte data = thisFloor << 1;
    data += 1;
    data = data << 1;
    sendDataToControlRoom(data);
    upButtonPressed = false;
  }
}

/**
 * Verandert de output op de 7-segment display.
 */
void changeNumberDisplay(const int &newFloor) {
//  cout << "Setting display to: " << newFloor << endl;
  digitalWrite(LATCH_PIN, LOW);//Enable the shift register.
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, dataArray[newFloor]);//to show that elevator is on builder's floor.
  digitalWrite(LATCH_PIN, HIGH);//Upload the data from shift register to storage register. 

}

/**
 * Verandert de interne waarde van currentFloor. Als de lift op deze etage stopt EN deze etage is de destination, dan zetten we de LED-knopjes en de richting-indicator uit.
 */
void setCurrentFloor(const int &newFloor) {
  currentFloor = newFloor;
  changeNumberDisplay(newFloor);
  if (newFloor == thisFloor && thisFloor == destinationFloor) {
    setIndicatorLED(STATIONARY);
    digitalWrite(BUTTON_DOWN_LED, LOW);
    digitalWrite(BUTTON_UP_LED, LOW);
  }
}

/**
 * Berekent welke richting de liftkooi in beweegt.
 */
int calcDirection (const int &cFloor, const int &nFloor) {

  // Als de volgende etage kleiner is dan de huidige etage, dan beweegt de liftkooi omlaag, en vice versa.
  if (nFloor < cFloor) {
    return GOING_DOWN;
  } else if (nFloor > cFloor) {
    return GOING_UP;
  } else {
    return STATIONARY;
  }
  
}

/**
 * Zorgt ervoor dat de juiste richting in de currentDirection-variabele komt te staan.
 */
void setDestinationFloor(const int &newFloor) {
  currentDirection = calcDirection(currentFloor, newFloor);
  setIndicatorLED(currentDirection);
  destinationFloor = newFloor;
}

bool readIRSensor() {
  return digitalRead(SENSOR_DATA);
}

bool isButtonUpPressed() {
  return digitalRead(BUTTON_UP);
}

bool isButtonDownPressed() {
  return digitalRead(BUTTON_DOWN);
}

/**
 * Zet de juiste indicator-LEDs boven de lift-opening aan.
 */
void setIndicatorLED(const int &cDirection) {
  switch (cDirection) {
    case GOING_UP:
      digitalWrite(LED_UP, HIGH);
      break;
    case GOING_DOWN:
      digitalWrite(LED_DOWN, HIGH);
      break;
    default:
      digitalWrite(LED_UP, LOW);
      digitalWrite(LED_DOWN, LOW);
  }
}

/**
 * Leest de broadcasts van de machine-kamer, en roept de juiste handle-functies aan.
 */
void receiveEvent(int sizeOfTransmission) {
  
  while (Wire.available()) {
      byte data = Wire.read();
      processTransmissionData(data);
  }
  
}

/**
 * Gebruikt de data om de interne functies aan te sturen.
 */
void processTransmissionData(byte &data) {

  bool functionIdentifier = data & 1;
  data  = data >> 1;
  if (functionIdentifier) {
    setCurrentFloor(data);
  } else {
    setDestinationFloor(data);
  }
  
}

/**
 * Verstuurd data naar de machine-kamer.
 */
bool sendDataToControlRoom(byte data) {
  Wire.beginTransmission(CONTROL_ROOM);
  Wire.write(data);
  Wire.endTransmission();  
}

// void buttonUpInterrupt() {
//   if (currentFloor  != thisFloor) {
//     upButtonPressed = true;
//   }
// }

// void buttonDownInterrupt() {
//   if (currentFloor  != thisFloor) {
//     downButtonPressed = true;
//   }
// }
