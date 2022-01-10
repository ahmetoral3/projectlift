#define knopBoven 3
#define knopBovenLED 9
#define knopOnder 2
#define knopOnderLED 7
#define lift 5
#define ledUp 10
#define ledDown 11

const int dataPin = 8;  // blue wire to 74HC595 pin 14
const int latchPin = 6; // green to 74HC595 pin 12
const int clockPin = 4; // yellow to 74HC595 pin 11

const byte dataArray[4] = {B11111101, B01100001, B11011011, B11110011};

int knopStateB = 0;
int knopStateO = 0;
int liftState = 0;

const char common = 'c';    // common cathode

bool decPt = true;  // decimal point display flag

void setup()
{
  pinMode(knopBoven, INPUT);
  pinMode(knopBovenLED, OUTPUT);
  pinMode(knopOnder, INPUT);
  pinMode(knopOnderLED, OUTPUT);
  pinMode(lift, INPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(ledUp, OUTPUT);
  pinMode(ledDown, OUTPUT);
}
void loop()
{
  digitalWrite(latchPin, LOW);//Enable the shift register.
  shiftOut(dataPin, clockPin, LSBFIRST, dataArray[B11110011]);//to show that elevator is on builder's floor.
  digitalWrite(latchPin, HIGH);//Upload the data from shift register to storage register.
  knopStateB = digitalRead(knopBoven);
  if (knopStateB == LOW)
  {
    digitalWrite(knopBovenLED, HIGH);
  }
  knopStateO = digitalRead(knopOnder);
  if (knopStateO == LOW)
  {
    digitalWrite(knopOnderLED, HIGH);
  }
  liftState = digitalRead(lift);
  if (liftState == HIGH)
  {
    digitalWrite(knopOnderLED, LOW);
    digitalWrite(knopBovenLED, LOW);
    digitalWrite(ledDown, HIGH);
    digitalWrite(ledUp, LOW);
  }
  else
  {
    digitalWrite(ledDown, LOW);
    digitalWrite(ledUp, HIGH);
  }

}
