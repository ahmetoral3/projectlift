#include <Wire.h> 

byte kooiAanwezig; // geeft aan of de liftkooi aanwezig is op deze etage 
byte knopGedrukt;   // geeft aan of de oproepknop is ingedrukt op deze locatie 
byte kooiLocatie;   // geeft de actuele liftkooi locatie aan 
byte oproepLampje;  // Lampje aan als oproepLampje = 1, lampje uit als oproepLampje = 0 

void setup() { 
Wire.begin(1);                // sluit aan op i2c bus als slave #1 (=etage 1) 

      // Note: vul hier het etage nummer in van je eigen etage 

Wire.onRequest(requestEvent); // functieaanroep wanneer er een request (leesverzoek) is ontvangen van de master 
Wire.onReceive(receiveEvent); // functieaanroep wanneer er een schrijfverzoek is ontvangen van master 

} 

void loop() { 

// plaats hier je eigen code voor de etage 

// de variabelen kooiAanwezig en knopGedrukt moeten hierin bepaald worden 

delay(500); 

} 
// onderstaande functie wordt uitgevoerd wanneer er een leesverzoek is ontvangen van de master 

//  
void requestEvent() { 

Wire.write(kooiAanwezig); // geeft aan of liftkooi op deze etage aanwezig is 
Wire.write(knopGedrukt); // geeft aan of er op de oproepknop is gedrukt 

} 
// deze functie wordt uitgevoerd wanneer de master informatie naar deze slave schrijft 

//  
void receiveEvent(int howMany) { 
kooiLocatie = Wire.read(); // liftkooi locatie om op 7-segment display te laten zien 
oproepLampje = Wire.read(); // zet het oproeplampje aan (oproepLampje=1) of uit (oproepLampje=0)op deze etage 

} 

// 
