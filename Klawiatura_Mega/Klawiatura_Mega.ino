/*
1.09.2015
10.09.2015 leniwy start
11.09.2015 kod pin dziala
*/

//------------------Libraries------------------//
#include <Keypad.h>
#include <LiquidCrystal.h>
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
byte statusUzbrojenia=1; // 1 - uzbrojony, 0-rozbrojony

//Buzzer
unsigned long buzzer_time = 0;
unsigned long aktualnyCzas =0;
/////////////////////////////////////////////

//Relay
int relay1 = 52;
int relay2 = 53;
unsigned long relay1_time=0;
unsigned long relay2_time=0;


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
       }
       
         if (statusUzbrojenia==1)
       {
         lcd.clear();
         lcd.write("Alarm aktywny!"); 
         digitalWrite(buzzer,HIGH);
         
         buzzer_time=millis()+1000; // Włacza buzzer na sekundę oraz zostawia ekran na 3 sekundy
         lcdFadeTime=millis()+3000;
         lcdFade=1;
       }
  }
}

////////////////////////////////////////////

void setup(){
  Serial.begin(9600);
  lcd.begin(20, 4);
  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2,OUTPUT);
  
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(redLed,HIGH);
  digitalWrite(greenLed,HIGH);
  digitalWrite(buzzer,LOW);
}
  
void loop(){
  
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

}

