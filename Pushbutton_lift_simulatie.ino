#define BUTTON 2
#define LED 3
#define LIFT 4

int BUTTONstate = 0;
int LIFTstate = 0;

void setup()
{
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(LIFT, INPUT);
}

void loop()
{
  BUTTONstate = digitalRead(BUTTON);
  if (BUTTONstate == HIGH)
  {
    digitalWrite(LED, HIGH);
  }
  LIFTstate = digitalRead(LIFT);
  if (LIFTstate == HIGH)
  {
    digitalWrite(LED, LOW);
  }
}
