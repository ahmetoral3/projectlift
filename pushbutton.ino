#define LED_PIN 2
#define BUTTON_PIN 3
#define LEDB_PIN 12
#define BUTTONB_PIN 13

int lastButtonState = 0;
int ledState = 0;
int lastButtonStateB = 0;
int ledStateB = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LEDB_PIN, OUTPUT);
  pinMode(BUTTONB_PIN, INPUT);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    if (buttonState == LOW) {
      ledState = (ledState == HIGH) ? LOW: HIGH;
      digitalWrite(LED_PIN, ledState);
    }
  }
  if (buttonState != lastButtonStateB) {
    lastButtonStateB = buttonState;
    if (buttonState == LOW) {
      ledStateB = (ledStateB == HIGH) ? LOW: HIGH;
      digitalWrite(LEDb_PIN, ledStateB);
    }
  }
}
