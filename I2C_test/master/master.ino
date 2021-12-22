#include <Wire.h>
int LED=13;
int x = 0;

void setup() 
{
  Wire.begin(15); 
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  Wire.onReceive(receive);
}

void loop() 
{
  Wire.beginTransmission(9);
  Wire.write(x);              
  Wire.endTransmission();
  Serial.println(x);
 
  x++;
  if (x > 6) 
  {
    x = 0;
  }
  delay(200);
}

void receive(int numberOfBytes) {
 while (Wire.available()) {
  if (Wire.read()) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
 }
}
