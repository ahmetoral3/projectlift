#define MOTOR_EN_1_2  10
#define MOTOR_IN1     9
#define MOTOR_IN2     8
 
#define slow 64
#define normal 128
#define fast 255
 
int Speed; 
 
void Forward_Rev(void){
  analogWrite(MOTOR_EN_1_2, Speed);
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
}
 
void Backward_Rev(void){
  analogWrite(MOTOR_EN_1_2, Speed);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
}
 
void Forward_ramp_up(void){
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  for (int i=0; i<255; i++) { 
    analogWrite(MOTOR_EN_1_2, i); 
    delay(10); 
  } 
} 

void Forward_ramp_down(void){ 
  digitalWrite(MOTOR_IN1, HIGH); 
  digitalWrite(MOTOR_IN2, LOW); 
  for (int i=255; i>=0; i--) {
    analogWrite(MOTOR_EN_1_2, i);
    delay(10);
  }
}
 
void Backward_ramp_up(void){
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  for (int i=0; i<255; i++) { 
    analogWrite(MOTOR_EN_1_2, i); 
    delay(10); 
  } 
} 

void Backward_ramp_down(void){ 
  digitalWrite(MOTOR_IN1, LOW); 
  digitalWrite(MOTOR_IN2, HIGH); 
  for (int i=255; i>=0; i--) {
    analogWrite(MOTOR_EN_1_2, i);
    delay(10);
  }
}
 
void Brake(void){
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, HIGH);
}
 
void setup() {
  
  Serial.begin(9600);
  Serial.println("L293D DC motor test");
 
  pinMode(MOTOR_EN_1_2, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
}
 
void loop() {
 
  Speed=normal; // Normal Speed
 
  Forward_Rev(); 
  delay(1000);
  Brake();
  delay(500);
  Backward_Rev(); 
  delay(1000);
  Brake();
  delay(500);
  Forward_ramp_up();
  Forward_ramp_down();
  Backward_ramp_up();
  Backward_ramp_down();
  
}
