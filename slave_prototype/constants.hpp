#pragma once
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
const byte dataArray[4] = {B11111101, B01100001, B11011011, B11110011};
const byte thisFloor = I2C_ADDRESS % 20;
