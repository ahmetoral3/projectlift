#include <Arduino.h>
#include <Wire.h>

#define SLAVE_0 0
#define MASTER 20

// SLAVE CODE

//input pins op de arduino
int irsensor = 5;
int buttonRed = 2;
int buttonWhite = 3;
int buttonRedLED = 7;
int buttonWhiteLED = 9;
int redLED = 10;
int blueLED = 11;

int verdieping;
int statusSensor;
int statusButtonRed;
int statusButtonWhite;
int statusButtonRedLED;
int statusButtonWhiteLED;

//pins van shift-out register worden gedefinieerd
int dataPin = 6;
int latchPin = 4;
int clockPin = 8;

int digits [4][8] { //Array met de shift out pins voor het laten zien van 7-segment getallen
  {1, 1, 1, 1, 1, 1, 0, 1}, // 0
  {0, 1, 1, 0, 0, 0, 0, 1}, // 1
  {1, 1, 0, 1, 1, 0, 1, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1, 1}, // 3
};

void DisplayDigit(int Digit) //digit weergeven op 7 segment met 74hc595(shift-out)
{
  digitalWrite(latchPin, LOW);
  for (int i = 7; i >= 0; i--)
  {
    digitalWrite(clockPin, LOW);
    if (digits[Digit][i] == 0) digitalWrite(dataPin, LOW);
    if (digits[Digit][i] == 1) digitalWrite(dataPin, HIGH);
    digitalWrite(clockPin, HIGH);
  }
  digitalWrite(latchPin, HIGH);
}

//functie om lampjes van de knoppen aan of uit te zetten
void buttonLEDS () {
  if (statusButtonRedLED == 1) digitalWrite(buttonRedLED, 1);
  if (statusButtonWhiteLED == 1) digitalWrite(buttonWhiteLED, 1);
  if (statusButtonRedLED == 0) digitalWrite(buttonRedLED, 0);
  if (statusButtonWhiteLED == 0) digitalWrite(buttonWhiteLED, 0);
}

//functie die de waarden uitleest die die binnenkrijgt van de master met I2C
void receiveEvent() {
  verdieping = Wire.read();
  statusButtonRedLED = Wire.read();
  statusButtonWhiteLED = Wire.read();

  buttonLEDS();
}

//Wanneer de master om data vraagt wordt deze functie gebruikt om data te schrijven naar de master
void requestEvent() {
  statusSensor = digitalRead (irsensor);
  statusButtonRed = digitalRead (buttonRed);
  statusButtonWhite = digitalRead (buttonWhite);
  Wire.write(statusSensor);
  Wire.write(statusButtonRed);
  Wire.write(statusButtonWhite);
}


void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(irsensor, INPUT);
  pinMode(buttonRed, INPUT);
  pinMode(buttonWhite, INPUT);
  pinMode(buttonRedLED, OUTPUT);
  pinMode(buttonWhiteLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  Wire.begin(1);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  buttonLEDS();
  if (statusSensor == 0){
    digitalWrite(redLED, HIGH);
    digitalWrite(blueLED, LOW);
  } else {
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, HIGH);
  }
  if (verdieping == 0) {
    DisplayDigit(0); //7-segment display laat 0 zien
  } else if (verdieping == 1) {
    DisplayDigit(1);
  } else if (verdieping == 2) {
    DisplayDigit(2);
  } else if (verdieping == 3) {
    DisplayDigit(3);
  }
}
