#include <Stepper.h>
// source : https://elektronicavoorjou.nl/arduino-project-stepper-motor-speed-control/
// Verander dit om het aantal stappen per omwenteling te passen
const int StappenPerOmwenteling = 100;
Stepper MijnStepper(StappenPerOmwenteling, 8, 9, 10, 11);
int StappenTeller = 0;
// aantal stappen dat de motor heeft genomen
void setup()
{
} void loop() {
// Lees sensorwaarde:
int sensorWaarde = analogRead(A0);
// Bereik 0 tot 100:
int motorSnelheid = map(sensorWaarde, 0, 1023, 0, 100);
// Zet de motor snelheid:
if (motorSnelheid > 0) { MijnStepper.setSpeed(motorSnelheid);
// stap 1/100 van een omwententeling:
MijnStepper.step(StappenPerOmwenteling / 100); }
}
