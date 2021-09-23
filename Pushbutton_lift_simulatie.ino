#define knopBoven 2
#define knopBovenLED 3
#define knopOnder 12
#define knopOnderLED 13
#define lift 4

int knopStateB = 0;
int knopStateO = 0;
int liftState = 0;

void setup()
{
  pinMode(knopBoven, INPUT);
  pinMode(knopBovenLED, OUTPUT);
  pinMode(knopOnder, INPUT);
  pinMode(knopOnderLED, INPUT);
  pinMode(lift, INPUT);
}

void loop()
{
  knopStateB = digitalRead(knopBoven);
  if (knopStateB == HIGH)
  {
    digitalWrite(knopBovenLED, HIGH);
  }
  liftState = digitalRead(lift);
  if (liftState == HIGH)
  {
    digitalWrite(LED, LOW);
  }

  knopStateO = digitalRead(knopBoven);
  if (knopStateO == HIGH)
  {
    digitalWrite(knopOnderLED, HIGH);
  }
  liftState = digitalRead(lift);
  if (liftState == HIGH)
  {
    digitalWrite(knopOnderLED, LOW);
  }
}
