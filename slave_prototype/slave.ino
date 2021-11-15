//#include <arduinio.h> // For debugging only!
#include <Wire.h>

// Define the directions the elevator can go.
#define GOING_DOWN -1
#define STATIONARY 0
#define GOING_UP 1

// Define the shift-out register pins.
#define LATCH_PIN 2
#define CLOCK_PIN 4
#define DATA_PIN 3

// Define the IR-sensor pin.
#define SENSOR_DATA 5

// Define the pins of the LED-buttons.
#define BUTTON_DOWN 6
#define BUTTON_DOWN_LED 7
#define BUTTON_UP 8
#define BUTTON_UP_LED 9

// Define the pins of the direction indicator LEDs.
#define LED_UP 10
#define LED_DOWN 11

// Define the floor's I2C-address
#define I2C_ADDRESS 20
#define CONTROL_ROOM 40

// Each index contains the data required to show the 
// corresponding number on the 7-segment display.
const byte dataArray[4] = {3, 159, 37, 13};
const byte thisFloor = I2C_ADDRESS % 20;

byte currentFloor = thisFloor; // Used to determine which number to show on the 7-segment display.
byte destinationFloor = thisFloor;
byte currentDirection = STATIONARY; // Used to determine which indicator LED to turn on.

bool liftDetectedBySensor = false;
bool downButtonPressed = false;
bool upButtonPressed = false;

void setup() {

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  
  pinMode(SENSOR_DATA, INPUT);
  
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_UP_LED, OUTPUT);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_LED, OUTPUT);
  
  pinMode(LED_DOWN, OUTPUT);
  pinMode(LED_UP, OUTPUT);

  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);

  TWAR = (I2C_ADDRESS << 1) | 1; //  Enable receiving broadcasts from master.

  // I2C_ADDRESS = 00000001
  // I2C << 1 = 00000010
  // I2C | 1 = 00000010 | 00000001 = 00000011
  
}

void loop() {
  //TODO implement cooldown for IR-read.
  // Als de lift gedetecteerd wordt, door de sensor, dan sturen we die data naar de machine-kamer.
  liftDetectedBySensor = readIRSensor();
  if (liftDetectedBySensor) {
    liftDetectedBySensor = !sendDataToControlRoom((thisFloor << 1) + 1);
  }
  downButtonPressed = isButtonUpPressed(); 
  upButtonPressed = isButtonDownPressed();
  
}

/**
 * Verandert de output op de 7-segment display.
 */
void changeNumberDisplay(const int &newFloor) {

  digitalWrite(LATCH_PIN, LOW);//Enable the shift register.
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, dataArray[newFloor]);//to show that elevator is on builder's floor.
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
