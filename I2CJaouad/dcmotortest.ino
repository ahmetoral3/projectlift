#define in1 11
#define in2 10

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

}

void loop() { 
  up();
  //park();
  //down();
}

void up(){
  digitalWrite(in1, HIGH);
  digitalWrite(in1, LOW);
}

void down(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void park(){
  digitalWrite(in1, LOW);
  digitalWrite(in1, LOW);
}
