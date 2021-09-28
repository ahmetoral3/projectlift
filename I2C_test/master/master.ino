#include <Wire.h>
int LED=13;
int x = 0;

void setup() 
{
  Wire.begin(); 
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
}

void loop() 
{
  Wire.beginTransmission(9);
  Wire.write(x);              
  Wire.endTransmission();
 
  x++;
  if (x > 6) 
  {
    x = 0;
  }
  delay(200);
}
