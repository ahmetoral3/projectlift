#include <Arduino.h>
#include <Wire.h>
#include <Keypad.h>

#define SLAVE_0 0
#define SLAVE_1 1
#define SLAVE_2 2
#define SLAVE_3 3

#define in1 11
#define in2 10


// MASTER CODE

int huidigeVerdieping;
int statusSensor[4];
int statusButtonRed[4];
int statusButtonWhite[4];

const byte ROWS = 4; //rijen numpad
const byte COLS = 4; //kolommen numpad

char hexaKeys[ROWS][COLS] = { //character array met de waardes van de numpad
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; //rijen numpad digital inputs
byte colPins[COLS] = {5, 4, 3, 2}; //kolommen numpad digital inputs

//functie van keypad library voor lezen van numpad
Keypad numpad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//functie die met I2C van een slave data vraagt zoals de status van de sensoren en de knopjes
void requestData (int etage) {
  Wire.requestFrom(etage, 3); //I2C wordt gebruikt om van een etage data te requesten
  statusSensor[etage] = Wire.read();
  statusButtonRed[etage] = Wire.read();
  statusButtonWhite[etage] = Wire.read();
  if (statusSensor[etage] == 0) //Als beide sensoren van de etage iets detecteren wordt de huidigeVerdieping integer aangepast
  {
    huidigeVerdieping = etage;
  } 
  for (int q = 0; q < 4; q++) //Er wordt van elke slave de status van de sensoren gecheckt
  {
    Wire.requestFrom(q, 1);
    statusSensor[q] = Wire.read();
  }
  //De knopjes van de etage wordt gecheckt om te kijken of de lift naar boven of naar beneden moet
  if (statusButtonRed[etage] == 1 && huidigeVerdieping > etage) {
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping); //
    Wire.write(1); //Rood lampje van de knop wordt aangezet
    Wire.write(0); //Wit lampje van de knop wordt uitgezet
    Wire.endTransmission();
    verdiepingOmlaag(etage);
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping);
    Wire.write(0); //Rood lampje van de knop wordt uitgezet
    Wire.write(0); //Wit lampje van de knop wordt uitgezet
    Wire.endTransmission();
  } else if (statusButtonWhite[etage] == 1 && huidigeVerdieping > etage) {
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping);
    Wire.write(0);
    Wire.write(1);
    Wire.endTransmission();
    verdiepingOmlaag(etage);
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
  }
  if (statusButtonRed[etage] == 1 && huidigeVerdieping < etage) {
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping);
    Wire.write(1);
    Wire.write(0);
    Wire.endTransmission();
    verdiepingOmhoog(etage);
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
  } else if (statusButtonWhite[etage] == 1 && huidigeVerdieping < etage) {
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping);
    Wire.write(0);
    Wire.write(1);
    Wire.endTransmission();
    verdiepingOmhoog(etage);
    Wire.beginTransmission(etage);
    Wire.write(huidigeVerdieping);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
  }
}
//Functie om de lift omhoog naar verdieping 'n' te laten gaan
void verdiepingOmhoog (int n) 
{
  int whileCondition = 0; 
  while (whileCondition == 0)//while loop die alleen stopt wanneer de sensoren van de etage waar de lift naar toe moet geactiveerd zijn
  {
    digitalWrite(in1, HIGH); //dc motor============================
    digitalWrite(in2, LOW);
    
    for (int q = 0; q < 4; q++){ //Van elke slave wordt requestData functie gebruikt zodat de 7segments de verdieping tonen
      requestData(q);
      Wire.beginTransmission(q);
      Wire.write(huidigeVerdieping);
      Wire.endTransmission();
    }
    //Er wordt gecheckt of de verdieping al is aangekomen bij verdieping 'n'
    Wire.requestFrom(n, 1);
    statusSensor[n] = Wire.read();
    if (statusSensor[n] == 0) //Als de sensoren van verdieping 'n' stopt de motor en de while loop
    {
      digitalWrite(in1, LOW); //dc motor============================
      digitalWrite(in2, LOW);
      whileCondition = 1;
    }
  }
}

//Functie om de lift omlaag naar verdieping 'n' te laten gaan
void verdiepingOmlaag (int n) 
{
  int whileCondition = 0; 
  while (whileCondition == 0) //while loop die alleen stopt wanneer de sensoren van de etage waar de lift naar toe moet geactiveerd zijn
  {   
    digitalWrite(in1, LOW); //dc motor============================
    digitalWrite(in2, HIGH);
    for (int q = 0; q < 4; q++){ //Van elke slave wordt requestData functie gebruikt zodat de 7segments de verdieping tonen
      requestData(q);
      Wire.beginTransmission(q);
      Wire.write(huidigeVerdieping);
      Wire.endTransmission();
    }
    //Er wordt gecheckt of de verdieping al is aangekomen bij verdieping 'n'
    Wire.requestFrom(n, 1);
    statusSensor[n] = Wire.read();
    if (statusSensor[n] == 0) //Als de sensoren van verdieping 'n' stopt de motor en de while loop
    {
      digitalWrite(in1, LOW); //dc motor============================
      digitalWrite(in2, LOW);
      whileCondition = 1;
    }
  }
}

//functie om de input van de numpad uit te lezen om te kijken waar de lift naar toe moet
void numpadCheck (char numpadInput){
  //deze if else statements kijken of de lift omhoog of omlaag moet (of niks moet doen)
  if (numpadInput == '0' && huidigeVerdieping != 0){ verdiepingOmlaag(0);
  } else if (numpadInput == '1' && huidigeVerdieping != 1){
    if (huidigeVerdieping < 1) { verdiepingOmhoog(1); }
    if (huidigeVerdieping > 1) { verdiepingOmlaag(1); }
  } else if (numpadInput == '2' && huidigeVerdieping != 2){
    if (huidigeVerdieping < 2) { verdiepingOmhoog(2); }
    if (huidigeVerdieping > 2) { verdiepingOmlaag(2); }
  } else if (numpadInput == '3' && huidigeVerdieping != 3){
    if (huidigeVerdieping < 3) { verdiepingOmhoog(3); }
    if (huidigeVerdieping > 3) { verdiepingOmlaag(3); }
  }
}

void setup() {
  Wire.begin();
  
  pinMode(in1,OUTPUT); //A van de stepper motor
  pinMode(in2,OUTPUT); //B van de stepper motor

  Serial.begin(9600);
}

void loop() {
  char numpadInput =  numpad.getKey(); //character 'numpadInput' wordt aangemaakt, dit is de input van de numpad 
  
  requestData(0); //Van de onderste etage wordt de requestData functie gebruikt om verschillende dingen met I2C te doen
  requestData(1);
  requestData(2);
  requestData(3);
  
  if (numpadInput){ //Als er een input op de numpad wordt gegeven wordt met de functie numpadCheck gekeken wat de lift moet doen
    numpadCheck(numpadInput);
  }

  //Er wordt naar elke etage de huidige verdieping gestuurd
  Wire.beginTransmission(SLAVE_0);
  Wire.write(huidigeVerdieping);
  Wire.endTransmission();
  Wire.beginTransmission(SLAVE_1);
  Wire.write(huidigeVerdieping);
  Wire.endTransmission();
  Wire.beginTransmission(SLAVE_2);
  Wire.write(huidigeVerdieping);
  Wire.endTransmission();
  Wire.beginTransmission(SLAVE_3);
  Wire.write(huidigeVerdieping);
  Wire.endTransmission();
                                              
}
