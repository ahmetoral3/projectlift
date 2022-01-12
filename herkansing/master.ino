#include <Wire.h>
#include <Keypad.h>
#include "constants.hpp"

static uint8_t velocity;
static uint32_t start_time;
static uint32_t time_to_go;

static uint8_t destinations[6];
static uint8_t destinations_left;

static uint8_t current_floor;
static uint8_t destination_floor;

static int8_t current_direction;
static boolean notify = true;

static long time = millis();

Keypad keypad = Keypad(makeKeymap(KEYS), ROW_PINS, COL_PINS, ROWS, COLS);

void set_current_direction(int8_t c_direction) {
  current_direction = c_direction;
  byte data = 0;
  data += destination_floor;
  data = data << 1;
  send_i2c_data(data);
}

void send_i2c_data(int data) {
  for (uint8_t index  = 20; index < 24; index++) {
    Serial.print("Sending data: ");
    Serial.println(data);
    Wire.beginTransmission(index);
    Wire.write(data);
    Wire.endTransmission();
  }
}

/**
   Inserts a destination at the given index of the destinations-array.
*/
void insert_in_destination_array(uint8_t index, uint8_t destination) {
  // Als de index groter is dan destinationsLeft, dan valt de index buiten de functionele array.
  if (index > destinations_left) {
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
  for (auto i = 0; i < destinations_left - index; i++) {
    newDestinations[i + index] = destinations[i + index];
  }

  // We kopiëren de waardes in de kopie-array naar de destinations-array.
  for (auto i = 0; i < 6; i++) {
    destinations[i] = newDestinations[i];
  }

  // Incrementeer de hoeveelheid destinations in de array.
  destinations_left += 1;
}

/**
   Leest data via I2C, en roept de juiste handle-functies aan.
*/
void handle_client_data(uint8_t data) {
  // TODO handle data...
  //  cout << "Data received..." << endl;
  Serial.println("Data received...");
  Serial.println(data, BIN);
  for (size_t index = 0; index < 3; index++) {
    if (data & 1) {
      switch (index) {
        case 0: // Button down pressed
          data = data >> 1;
        case 1: // Button up pressed
          for (auto destination : destinations) {
            if (data == destination)
              return;
          }
          Serial.print("Button press detected at floor ");
          Serial.println(index);
          data = data >> 1;
          insert_in_destination_array(destinations_left, data >> 1);
          break;
        case 2: // Arrived at floor.
          notify = true;
          current_floor = data >> 1;
          break;
      }
    }
    data = data >> 1;
  }
}

/**
   Removes the entry at the given index of the destinations-array.
*/
void remove_at_index(int index) {
  // Als de index kleiner is dan destinationsLeft, dan is het een valide index.
  if (index < destinations_left) {
    // Vanaf de index die verwijderd moet worden, schuiven we alle waardes in de destinationsArray een plaats op naar links.
    for (auto i = index; index < 5; index++) {
      destinations[i] = destinations[i + 1];
    }
    destinations[5] = 0;
  }
  // Decrementeer de hoeveelheid
  destinations_left -= 1;
  
}

void handle_ir_signal(byte data) {
  // TODO implement
  // if data == destinationFloor then
  //     Stop elevator
  //     Remove destination from array (removeAtIndex(0))
  // endif
  //
  // Send floor number to 7-segs
  if (data == destination_floor) {
    switch (current_direction) {
      case 0:
        break;
      case 1:
        forward_ramp_down();
        break;
      case -1:
        backward_ramp_down();
        break;
    }
//    time_to_go = millis() + 5000;
    remove_at_index(0);
    destination_floor = destinations[0];
    current_direction = STATIONARY;
  }
  data = data << 1;
  data = data + 1;
  send_i2c_data(data);
}

void handle_button_press(byte data) {
  // Als de data al in de destinationArray staat, dan doen we er niks mee.
  for (auto destination : destinations) {
    if (data == destination) return;
  }
  // Data bevat al alle informatie die nodig is, in de juiste volgorde, om het in de
  // destinations-array te zetten.
  // TODO implement algoritme om slim de volgorde van destinations te bepalen.
  insert_in_destination_array(destinations_left, data);
}

void process_key(char key) {

  byte keypad_data = current_floor;

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

  if (keypad_data == current_floor) {
    return;
  }

  keypad_data = keypad_data << 1;
  keypad_data += (keypad_data > current_floor) ? 1 : 0;

  handle_button_press(keypad_data);

}

// Motor
void forward_ramp_up() {
  digitalWrite(MOTOR_IN_1, HIGH);
  digitalWrite(MOTOR_IN_2, LOW);
  int i = 134;
  start_time = millis();
  while (i < 255) {
    analogWrite(MOTOR_ENABLE, i);
    if ((millis() - start_time) % DELAY_FACTOR == 0) {
      i += 1;
    }
  }
}

void forward_ramp_down() {
  digitalWrite(MOTOR_IN_1, HIGH);
  digitalWrite(MOTOR_IN_2, LOW);
  int i = 255;
  start_time = millis();
  while (i >= 0) {
    analogWrite(MOTOR_ENABLE, i);
    if ((millis() - start_time) % DELAY_FACTOR == 0) {
      i -= 1;;
    }
  }
}

void backward_ramp_up() {
  digitalWrite(MOTOR_IN_1, LOW);
  digitalWrite(MOTOR_IN_2, HIGH);
  int i = 134;
  start_time = millis();
  while (i < 255) {
    analogWrite(MOTOR_ENABLE, i);
    if ((millis() - start_time) % DELAY_FACTOR == 0) {
      i += 1;
    }
  }
}

void backward_ramp_down() {
  digitalWrite(MOTOR_IN_1, LOW);
  digitalWrite(MOTOR_IN_2, HIGH);
  int i = 255;
  start_time = millis();
  while (i >= 0) {
    analogWrite(MOTOR_ENABLE, i);
    if ((millis() - start_time) % DELAY_FACTOR == 0) {
      i -= 1;;
    }
  }
}

void brake() {
  digitalWrite(MOTOR_IN_1, HIGH);
  digitalWrite(MOTOR_IN_2, HIGH);
}

void setup_timer_interrupt(int freq) {

  TCCR3A = 0;
  TCCR3B = 0;
  TCNT3 = 0;

  TCCR3B |= (1 << WGM12);

  TCCR3B |= (1 << CS12) | (1 << CS10);

  OCR3A = 15625 / freq - 1;

  TIMSK3 |= (1 << OCIE3A);

}

//ISR(TIMER3_COMPA_vect) {
//  process_key(keypad.getKey());
//}

void setup() {
  Serial.begin(9600);

  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(MOTOR_IN_1, OUTPUT);
  pinMode(MOTOR_IN_2, OUTPUT);

  Wire.begin();

  time_to_go = millis();

  current_floor = 3;
  destinations_left = 0;
  insert_in_destination_array(0, 0);

}

void loop() {
  if (notify) {
    handle_ir_signal(current_floor);
    notify = false;
  }
  for (uint8_t index = 20; index < 24; index++) {
    byte data = Wire.requestFrom(index, 1);
    while (Wire.available()) {
      data = Wire.read();
    }
    handle_client_data(data);
  }
  if (current_direction == STATIONARY && (millis() > time_to_go) && destinations_left > 0) {
    destination_floor = destinations[0];
    if (destination_floor > current_floor) {
      set_current_direction(GOING_UP);
      forward_ramp_up();
    } else {
      set_current_direction(GOING_DOWN);
      backward_ramp_up();
    }
  }
}
