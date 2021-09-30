#include <Keypad.h>

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

Keypad keypad - Keypad( makeKeymap(keys), rowPins, colPins);

void setup() {
  // No real setup for keypad
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();
  if (key != NO_KEY) {
    delay(60);
    switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
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
