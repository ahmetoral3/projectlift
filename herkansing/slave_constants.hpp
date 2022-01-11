#pragma once

int8_t const GOING_DOWN = -1;
int8_t const STATIONARY = 0;
int8_t const GOING_UP = 1;

uint8_t const LATCH_PIN = 6;
uint8_t const CLOCK_PIN = 4;
uint8_t const DATA_PIN = 8;

uint8_t const SENSOR_DATA = 5;

uint8_t const BUTTON_DOWN = 2;
uint8_t const BUTTON_DOWN_LED = 7;
uint8_t const BUTTON_UP = 3;
uint8_t const BUTTON_UP_LED = 9;

uint8_t const LED_UP = 10;
uint8_t const LED_DOWN = 11;

uint8_t const I2C_ADDRESS = 20;

uint8_t const DATA_ARRAY[4] = {B11111101, B01100001, B11011011, B11110011};
uint8_t const THIS_FLOOR = I2C_ADDRESS % 20;
