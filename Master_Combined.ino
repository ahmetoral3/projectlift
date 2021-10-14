#include <arduinio.h>
#include <Wire.h>
#include <Keypad.h>

// Define the directions the elevator can go.
#define GOING_DOWN -1
#define STATIONARY 0
#define GOING_UP 1

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
 * Nu kunnen we de meest linkse bit (LSB) 0 laten voor naar beneden, of 1 maken voor 
 * naar boven.
 * 
 * Hoewel de array gevuld is met 0, kunnen we toch 0 gebruiken voor: Naar beneden, naar 
 * de begane grond.
 * 
 * Dit doen we door bij te houden hoeveel elementen er in de destinations-array zitten.
 */
byte destinations[6];
byte destinationsLeft = 0;

byte currentFloor = 0;
byte destinationFloor = 0;
byte currentDirection = STATIONARY;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
}

void loop() {
  // Hier moet de motor-code. De rest van de logica wordt uitgevoerd naar aanleiding van
  // signalen vanaf de etages.
  // Ook de keypad hier uitlezen.

}

void receiveEvent(int numOfBytes) {
  // TODO handle data...
  while (Wire.available()) {
    byte data = Wire.read();
    bool functionIdentifier = data % 2;
    if (functionIdentifier) {
      // IR-signal
      handleIRSignal(data >> 1);
    } else {
      // Button pressed
      handleButtonPress(data >> 1);
    }
  }
}

/**
 * Inserts a destination at the given index of the destinations-array.
 */
void insertInDestinationArray(int index, byte destination) {
  if (destinationsLeft < index) {
    cout << "Index is greater than amount of destinations in array." << endl;
  }
  
  byte newDestinations[6];
  for (auto i = 0; i < index; i++) {
    newDestinations[i] = destinations[i];
  }

  newDestinations[index] = destination;

  for (auto i = 0; i < destinationsLeft - index; i++) {
    newDestinations[i + index] = destinations[i + index];
  }

  for (auto i = 0; i < 6; i++) {
    destinations[i] = newDestinations[i];
  }

  destinationsLeft += 1;
}

/**
 * Removes the entry at the given index of the destinations-array.
 */
void removeAtIndex(int index) {
  if (index < destinationsLeft) {
    for (auto i = index; index < 5; index++) {
      destinations[i] = destinations[i + 1];
    }
  }
  destinationsLeft -= 1;
}

void handleIRSignal(byte data) {
  // TODO implement
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
  if (key != NO_KEY) {
    delay(60);
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
  // Add a destination to the master's destinations.
}
