#define MOTOR_EN_1_2  10
#define MOTOR_IN1     9
#define MOTOR_IN2     8
 
#define slow 64
#define normal 128
#define fast 255

#define delayFac  200
 
int Speed;
int startTime; 
 
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
  int i = 134;
  startTime = millis();
  while (i < 255) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i += 1;
    }
  }
} 

void Forward_ramp_down(void){ 
  digitalWrite(MOTOR_IN1, HIGH); 
  digitalWrite(MOTOR_IN2, LOW);
  int i = 255;
  startTime = millis();
  while (i >= 0) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i -= 1;;
    }
  }
}
 
void Backward_ramp_up(void){
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  int i = 134;
  startTime = millis();
  while (i < 255) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i += 1;
    }
  }
} 

void Backward_ramp_down(void){ 
  digitalWrite(MOTOR_IN1, LOW); 
  digitalWrite(MOTOR_IN2, HIGH); 
  int i = 255;
  startTime = millis();
  while (i >= 0) {
    analogWrite(MOTOR_EN_1_2, i);
    if ((millis() - startTime)%delayFac==0){
      i -= 1;;
    }
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
 
  Speed=fast; // Normal Speed
 
  Forward_ramp_up();
  Forward_Rev();
  delay(500);
  Forward_ramp_down();
  delay(1000);
  Backward_ramp_up();
  Backward_Rev();
  delay(500); 
  Backward_ramp_down();
  delay(1000);
  
  
}
