#include <Wire.h>
#include "slave_constants.hpp" 

static uint8_t current_floor = 6; // Used to determine which number to show on the 7-segment display.
static uint8_t destination_floor = 0;
static int8_t current_direction = STATIONARY; // Used to determine which indicator LED to turn on.

static bool lift_detected_by_sensor = false;
static bool down_button_pressed = false;
static bool up_button_pressed = false;

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

  cli();
  attachInterrupt(digitalPinToInterrupt(BUTTON_UP), button_up_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN), button_down_interrupt, FALLING);
  sei();

//  changeNumberDisplay(thisFloor);

//  digitalWrite(LED_UP, HIGH);
//  digitalWrite(LED_DOWN ,HIGH);
//  digitalWrite(BUTTON_UP_LED, HIGH);
//  digitalWrite(BUTTON_DOWN_LED, HIGH);

  // I2C_ADDRESS = 00000001
  // I2C << 1 = 00000010
  // I2C | 1 = 00000010 | 00000001 = 00000011

  change_number_display(3);
  
}

void loop() {
  //TODO implement cooldown for IR-read.
  // Als de lift gedetecteerd wordt, door de sensor, dan sturen we die data naar de machine-kamer.
  //    cout << "Lift detected..." << endl;    
  if (!read_ir_sensor()) {
    lift_detected_by_sensor = true;
  }
}

/**
 * Verandert de output op de 7-segment display.
 */
void change_number_display(const int &newFloor) {
//  cout << "Setting display to: " << newFloor << endl;
  digitalWrite(LATCH_PIN, LOW);//Enable the shift register.
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, DATA_ARRAY[newFloor]);//to show that elevator is on builder's floor.
  digitalWrite(LATCH_PIN, HIGH);//Upload the data from shift register to storage register. 

}

/**
 * Verandert de interne waarde van currentFloor. Als de lift op deze etage stopt EN deze etage is de destination, dan zetten we de LED-knopjes en de richting-indicator uit.
 */
void set_current_floor(const int &new_floor) {
  current_floor = new_floor;
  change_number_display(new_floor);
  if (new_floor == THIS_FLOOR && THIS_FLOOR == destination_floor) {
    set_indicator_led(STATIONARY);
    digitalWrite(BUTTON_DOWN_LED, LOW);
    digitalWrite(BUTTON_UP_LED, LOW);
  }
}

/**
 * Berekent welke richting de liftkooi in beweegt.
 */
int calc_direction (const int &cFloor, const int &nFloor) {

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
void set_destination_floor(const int &new_floor) {
  current_direction = calc_direction(current_floor, new_floor);
  set_indicator_led(current_direction);
  destination_floor = new_floor;
}

bool read_ir_sensor() {
  return digitalRead(SENSOR_DATA);
}

bool is_button_up_pressed() {
  return digitalRead(BUTTON_UP);
}

bool is_button_down_pressed() {
  return digitalRead(BUTTON_DOWN);
}

/**
 * Zet de juiste indicator-LEDs boven de lift-opening aan.
 */
void set_indicator_led(const int &cDirection) {
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
      process_transmission_data(data);
  }
  
}

/**
 * Gebruikt de data om de interne functies aan te sturen.
 */
void process_transmission_data(byte &data) {

  bool functionIdentifier = data & 1;
  data  = data >> 1;
  if (functionIdentifier) {
    set_current_floor(data);
  } else {
    set_destination_floor(data);
  }
  
}

void on_data_request() {
  uint8_t data = THIS_FLOOR;
  data = data << 1;
  data += (lift_detected_by_sensor) ? 1 : 0;
  data = data << 1;
  data += (up_button_pressed) ? 1 : 0;
  data = data << 1;
  data += (down_button_pressed) ? 1 : 0;
  Wire.write(data);
  
  lift_detected_by_sensor = false;
  down_button_pressed = false;
  up_button_pressed = false;
}


void button_up_interrupt() {
  if (current_floor  != THIS_FLOOR) {
    up_button_pressed = true;
    digitalWrite(BUTTON_UP_LED, HIGH);
  }
}

void button_down_interrupt() {
  if (current_floor  != THIS_FLOOR) {
    down_button_pressed = true;
    digitalWrite(BUTTON_DOWN_LED, HIGH);
  }
}
