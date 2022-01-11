#pragma once

uint8_t const GROUND_FLOOR = 20;
uint8_t const FIRST_FLOOR = 21;
uint8_t const SECOND_FLOOR = 22;
uint8_t const THIRD_FLOOR = 23;

uint8_t const MOTOR_ENABLE = 12;
uint8_t const MOTOR_IN_1 = 11;
uint8_t const MOTOR_IN_2 = 10;

uint8_t const DELAY_FACTOR = 200;

int8_t const STATIONARY = 0;
int8_t const GOING_UP = 1;
int8_t const GOING_DOWN = -1;

byte const ROWS = 4;
byte const COLS = 4;

byte const ROW_PINS[ROWS] = {9, 8, 7, 6};
byte const COL_PINS[COLS] = {5, 4, 3, 2};

char const KEYS[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
