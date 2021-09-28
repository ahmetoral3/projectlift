#include <Stepper.h>
const int StappenPerOmwenteling = 100;
Stepper MijnStepper(StappenPerOmwenteling, 8, 9, 10, 11);


void setup(){
  pinMode(3,INPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);
} 
void loop() {
  motorTurn();
}
// FSM gebruiken
void motorTurn() {
  int motorSnelheid;
  if (liftDetect() == LOW){
    digitalWrite(13,LOW);
    motorSnelheid = 180;
  } else {
    digitalWrite(13,HIGH);
    motorSnelheid = speedDecay(motorSnelheid);
    Serial.println(motorSnelheid);
  }
  if (motorSnelheid > 0) { 
    MijnStepper.setSpeed(motorSnelheid);
    // stap 1/100 van een omwententeling:
    MijnStepper.step(StappenPerOmwenteling / 100); 
  }
}

int speedDecay(int A) {
  int decay = 10;
  A = A - decay;
  return A;
}

bool liftDetect() {
  if (digitalRead(3) == HIGH) {
    return HIGH;
  } else {
    return LOW;
  }
}
