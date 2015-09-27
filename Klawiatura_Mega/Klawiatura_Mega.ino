/*
1.09.2015
10.09.2015 leniwy start
11.09.2015 kod pin dziala
25.09 
27.09 dodanie alarmu
*/

//------------------Libraries------------------//
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Wire.h>
//---------------------------------------------//


//-------------------Led Pins-------------------//
int greenLed = 2;
int redLed = 3;
int buzzer = 4;
//---------------------------------------------//


//---------------------LCD----------------------//
LiquidCrystal lcd(30, 31, 32, 33, 34, 35); 
boolean lcdFade=1;
unsigned long lcdFadeTime =0;
  //LCD pin name 	RS 	EN 	DB4 	DB5 	DB6 	DB7
  //Arduino pin  	30 	31 	32 	33 	34 	35
//---------------------------------------------//

/////////Keypad:
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
boolean customCheck =0;
char klawisz =0;

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


//Kod pin:
byte enterPinModeActive=0;
char kodPin[4] = {'1','4','7','8'};
int pozycja =0;
int proba=0;
byte statusUzbrojenia=0; // 1 - uzbrojony, 0-rozbrojony

//Buzzer
unsigned long buzzer_time = 0;
unsigned long aktualnyCzas =0;
/////////////////////////////////////////////

//Relay
int relay1 = 52;
int relay2 = 53;
unsigned long relay1_time=0;
unsigned long relay2_time=0;

//Kontrakton
int magnetPin = 18; //for interrupt
boolean firstTime=0;

//I2C
int daneI2C [3];
int licznikI2C = 0;
int ileBitowI2C =0;
byte lowBit = 0;
byte highBit = 0;
int iI2C =0;
unsigned long i2c_time=0;
#define SLAVE_ADDRESS 0x60
float temp =0;
float wilg =0;
int cisn =0;

void czytajI2C()
{
    Wire.requestFrom(SLAVE_ADDRESS, 6);
  ileBitowI2C=Wire.available();
  Serial.print("Ilosc bitow I2C:");
  Serial.println(ileBitowI2C);
  for (iI2C =0;iI2C<ileBitowI2C; iI2C++)
  {
    
    
  if (iI2C%2==0 || iI2C==0) //For pair bytes and 0:
    {
      lowBit=Wire.read(); 
    //  Serial.print("lowBit:");
  //    Serial.println(lowBit);
    }
    
    else if (iI2C%2==1) //For unpair bytes
    {
    highBit=Wire.read();
    //Serial.print("highBit:");
    //Serial.println(highBit);
    daneI2C[licznikI2C]=lowBit+highBit*256;
    //Serial.print("Dane[");
    //Serial.print(licznikI2C);
    //Serial.print("]:");
    //Serial.println(daneI2C[licznikI2C]);
    licznikI2C=licznikI2C+1;
    }
  }
  licznikI2C=0;
//  temp=daneI2C[1]/100;
//  wilg=daneI2C[2]/100;
  temp=daneI2C[1];
  wilg=daneI2C[2];
  temp=temp/100;
  wilg=wilg/100;
  cisn=daneI2C[0];
  Serial.println("---------------------------------------------------");
  Serial.print("Temperatura:");
  Serial.println(temp);
  Serial.print("Wilgotnosc:");
  Serial.println(wilg);
  Serial.print("Cisnienie:");
  Serial.println(cisn);
  Serial.println("---------------------------------------------------");
  
}


void enterPinMode() //Sprawdza poprawność kodu PIN
{
    proba=proba+1; //Zlicza ilość prób wpisania kodu
    lcd.print("*"); //Wyświetla gwiazdkę zamiast znaku
    
    if (klawisz == kodPin[pozycja]) //Zmienna pozycja wskazuje poprawność wpisywanego kodu
    {
      pozycja = pozycja +1;   
    }
    else pozycja =0;
    
    
       if (proba==4 && pozycja<4) //Jeśli kod PIN jest zły
   {
     lcd.clear();
     lcd.write ("Niepoprawny kod PIN!");
     enterPinModeActive=0;
     lcdFadeTime=millis()+3000;
     lcdFade=1;
     
  Wire.beginTransmission(0x60);   
  Wire.write(0);                
  Wire.endTransmission();
   }
    
    
    if (pozycja == 4 && proba == 4) //Pin OK
    {
      enterPinModeActive=0;
      statusUzbrojenia=!statusUzbrojenia;
    
           if (statusUzbrojenia==0)
        {
         lcd.clear();
         lcd.write("Alarm nieaktywny!");
         digitalWrite(buzzer,HIGH);
         digitalWrite(relay1, LOW);
         relay1_time=millis()+5000;
         buzzer_time=millis()+1000; // Włacza buzzer na sekundę oraz zostawia ekran na 3 sekundy
         lcdFadeTime=millis()+3000;
         lcdFade=1;
         
         Wire.beginTransmission(0x60);   
  Wire.write(1);                
  Wire.endTransmission();
       }
       
         if (statusUzbrojenia==1)
       {
         lcd.clear();
         lcd.write("Alarm aktywny!"); 
         digitalWrite(buzzer,HIGH);
         
         buzzer_time=millis()+1000; // Włacza buzzer na sekundę oraz zostawia ekran na 3 sekundy
         lcdFadeTime=millis()+3000;
         lcdFade=1;
         
         Wire.beginTransmission(0x60);   
  Wire.write(2);                
  Wire.endTransmission();
         
       }
  }
}

////////////////////////////////////////////

void setup(){
  Serial.begin(9600);
  Wire.begin(); //Inicjalizacja I2C
  lcd.begin(20, 4);
  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2,OUTPUT);
  
  pinMode(magnetPin,INPUT);
  
  
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(redLed,HIGH);
  digitalWrite(greenLed,HIGH);
  digitalWrite(buzzer,LOW);
}
  
void loop(){
  
  
//Sprawdza stan kontraktonu:
if (digitalRead(magnetPin)==0 && firstTime==1 ) //Jesli drzwi zostaly otwarte 
{
 Serial.println("Drzwi otwarte!");
  if (statusUzbrojenia==1) //W stanie uzbrojenia, wlaczAlarm:
  {
 digitalWrite(buzzer,HIGH);
 Serial.println("Alarm activated!"); 
 buzzer_time=millis()+1000;
  }
                         // Niezaleznie od stanu, wyslij do plytki 2 i zapisz na karcie SD:
 //+ logowanie do karty SD
   Serial.println("Information sent to module 2!"); 
  Wire.beginTransmission(0x60);   
  Wire.write(magnetPin);                
  Wire.endTransmission();
  firstTime=0;  
}

if (digitalRead(magnetPin)==1)
{
 firstTime=1; 
 //Serial.println("Magnet dezactivated!"); 
}
  
  
// Sprawdza status alarmu i sygnalizuje go diodami:
  if (statusUzbrojenia==0)
  {
   digitalWrite(redLed,HIGH); 
   digitalWrite(greenLed,LOW); 
  }
  
  if (statusUzbrojenia==1)
  {
   digitalWrite(redLed,LOW); 
   digitalWrite(greenLed,HIGH); 
  }
  
  
 //Sprawdź czy nacisnieto nowy klawisz:  
char customKey = customKeypad.getKey();
   
//Jesli tak, zapikaj i wypisz go na LCD:   
  if (customKey)
  {
    Serial.println("FunkcjaCustomKey");
    digitalWrite(buzzer,HIGH);
    buzzer_time=millis()+200; // Czas do wylaczenia

  klawisz=customKey;
  Serial.println(klawisz);
  //customCheck = customKeypad.keyStateChanged(); // Czy status klawisza sie zmienil?
  
    if (enterPinModeActive==1 && klawisz!='#') //Jeśli wpisywany jest kod pin, następuje przekierowanie
    {
    //Serial.println("Funckja enterpin");
    enterPinMode();  
    }
  
  //Tu zapada decyzja co zrobić dalej:
  switch (klawisz) {
   case '*': 
   enterPinModeActive=1;
   proba=0;
   lcd.clear();
   lcd.write("Wpisz kod:");
   break;
   
   case '#':
   lcd.clear();
  lcd.print("Ekran startowy.");
  lcd.setCursor(5,1);
  lcd.print("Witaj!");
  enterPinModeActive=0;
   break;
   
   case 'A':
   if (enterPinModeActive==0 && statusUzbrojenia==0)
   {
     digitalWrite(relay1, LOW);
     relay1_time=millis()+5000;
   lcd.clear();
   lcd.print("Drzwi otwarte!");
   lcdFadeTime=millis()+3000;
   lcdFade=1;
   }   
   break;
   
   case 'B':
   if (enterPinModeActive==0 && statusUzbrojenia==0)
   {
     
     digitalWrite(relay2, LOW);
     relay2_time=millis()+5000;
     lcd.clear();
     lcd.print("Relay 2 aktywny!");
     lcdFadeTime=millis()+3000;
     lcdFade=1;
     
   }   
   break;
   
   case 'C':
   if (enterPinModeActive==0 && statusUzbrojenia==0)
   {

     lcd.clear();
     lcd.print("Temp: ");
     lcd.print(temp);
     lcd.setCursor(0,1);
     lcd.print("Wilg: ");
     lcd.print(wilg);
      lcd.setCursor(0,2);
      lcd.print("Cisn: ");
     lcd.print(cisn);
     
     lcdFadeTime=millis()+3000;
     lcdFade=1;
     
   }   
   break;
   
  }

  }
 
 aktualnyCzas = millis(); //Pobierz czas, zeby sprawdzic czy wylaczyc buzzer:
 
 if (aktualnyCzas>=buzzer_time)
  {
  digitalWrite(buzzer,LOW);  
  }
  
   if (aktualnyCzas>=relay1_time)
  {
  digitalWrite(relay1,HIGH);  
  }
  
    if (aktualnyCzas>=relay2_time)
  {
  digitalWrite(relay2,HIGH);  
  }
  
  if (aktualnyCzas>=lcdFadeTime && lcdFade==1 && enterPinModeActive==0)
  {
  lcd.clear();
  lcd.print("Ekran startowy.");
  lcd.setCursor(5,1);
  lcd.print("Witaj!");
 lcd.setCursor(0,2);
 lcd.print("Status alarmu:");
 lcd.setCursor(0,3);
 if (statusUzbrojenia==1){
 lcd.print("Uzbrojony");
 }
 else
 lcd.print("Rozbrojony");
  lcdFade=0;
  }
  
    if (aktualnyCzas>=i2c_time)
  {  
  //Odczytanie informacji z I2C:
   czytajI2C(); 
     i2c_time=millis()+500;
  }
  

  

}

