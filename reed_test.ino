int LED = 7;
int reed_switch = 4;
int reed_status;
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(reed_switch, INPUT);
  }
  void loop(){
    reed_status = digitalRead(reed_switch);
    if (reed_status == 1){
    digitalWrite(LED, LOW);
    }else{
    digitalWrite(LED, HIGH);
    delay(1000);}
    }
