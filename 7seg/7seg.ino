const int latchPin = 2;//Pin connected to ST_CP of 74HC595 
const int clockPin = 4;//Pin connected to SH_CP of 74HC595  
const int dataPin = 3; //Pin connected to DS of 74HC595  
/*======================================================================================================= 
//display 0,1,2,3,4,5,6,7,8 
Number 0 :  00000011     3  
Number 1 :  10101111     159  
Number 2 :  00100101     37 
Number 3 :  00001101     13 
Number 4 :  10011001     153 
Number 5 :  01001001     73 
Number 6 :  01000001     65 
Number 7 :  00011111     31 
Number 8 :  00000001     1 
To display the dot, simply subtract 1. 
example : 1 - 1 = 0  or b0000000 0x0 to display 8 
*/ 

/*In order to display zero your array has decimal number 3. However using the shift register 
it is actuall the binary number that is uploaded into the resitsre: zie the table above for zero: 00000011.  
*/ 

int datArray[9] = {3, 159, 37, 13, 153, 73, 65, 31, 1}; // array without the decimal 

void setup () 
{ 
 //set pins to output 
 pinMode(latchPin,OUTPUT); 
 pinMode(clockPin,OUTPUT);
 pinMode(dataPin, OUTPUT);
 pinMode(6, INPUT);
} 

void loop() 
{ 
  int sensor = digitalRead(6);
  
  if (sensor == LOW){
    digitalWrite(latchPin, LOW);//Enable the shift register.
    shiftOut(dataPin,clockPin,MSBFIRST,datArray[3]);//to show that elevator is on builder's floor.
    digitalWrite(latchPin, HIGH);//Upload the data from shift register to storage register. 
  } 
  else {
    digitalWrite(latchPin, LOW);//Enable the shift register.
    shiftOut(dataPin,clockPin,MSBFIRST,datArray[3]);//to show that elevator is on builder's floor.
    digitalWrite(latchPin, HIGH);//Upload the data from shift register to storage register.
  }

} 
