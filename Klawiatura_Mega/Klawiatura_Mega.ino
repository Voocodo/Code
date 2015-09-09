/*

1.09.2015
*/

#include <Keypad.h>
#include <LiquidCrystal.h>




////Led:
int greenLed = 2;
int redLed = 3;
int buzzer = 4;
  //LCD:
  LiquidCrystal lcd(30, 31, 32, 33, 34, 35);


/////////Keypad:
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
boolean customCheck =0;
char kodPin[4] = {'1','4','7','8'};
int pozycja =0;
int proba=0;

char klawisz =0;

//Kod pin
byte enterPinModeActive=0;



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

//Buzzer

unsigned long buzzer_time = 0;
unsigned long aktualnyCzas =0;
/////////////////////////////////////////////


void enterPinMode()
{
    digitalWrite(greenLed,LOW);
    digitalWrite(redLed,HIGH);
    proba=proba+1;
    lcd.print("*");
    
    
    Serial.println("Enter Pin Mode");
    if (klawisz == kodPin[pozycja])
    {
      Serial.print("Klawisz:");
      Serial.println(klawisz);
      Serial.print("Kod Pin:");
      Serial.println(kodPin[pozycja]);
      Serial.print("Pozycja");
      Serial.println(pozycja);
      
      pozycja = pozycja +1;
      
    }
    
    else pozycja =0;
    
    if (pozycja == 3 && proba == 4)
    {
      Serial.println("Otwieram");
     lcd.clear();
    lcd.write("Otwarte!"); 
    }
   
   if (proba==4 && proba<4)
   {
     enterPinModeActive=0;
   }
    
}









////////////////////////////////////////////
void setup(){
  Serial.begin(9600);
  //LCD:
    lcd.begin(20, 4);
  //
  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer,OUTPUT);
  
  digitalWrite(redLed,HIGH);
  digitalWrite(greenLed,HIGH);
  digitalWrite(buzzer,LOW);
  lcd.setCursor(0,0);
  lcd.print("Wpisz kod PIN:");
  lcd.setCursor(14,0);
  
  //LCD pin name 	RS 	EN 	DB4 	DB5 	DB6 	DB7
  //Arduino pin  	30 	31 	32 	33 	34 	35
  ///
}
  
void loop(){

  
  char customKey = customKeypad.getKey();
  
  if (customKey){
    digitalWrite(buzzer,HIGH);
    buzzer_time=millis()+200;
  //Serial.println(customKey);
  klawisz=customKey;
  Serial.println(klawisz);
  Serial.println(customCheck);
  lcd.write(klawisz);
  
customCheck = customKeypad.keyStateChanged();
  }
  
  
 aktualnyCzas = millis();
  
  if (aktualnyCzas>=buzzer_time)
  {
  digitalWrite(buzzer,LOW);  
  }
  
  
   //LCD:
if (customCheck==1)
{
  Serial.println(customKey);
  if (customKey=='*' || enterPinModeActive==1)
  {
    lcd.clear();
    lcd.write("Wpisz kod:");

    enterPinModeActive=1;
    enterPinMode();
  }
  
  if (customKey=='#')
  {
   enterPinModeActive=0;
     digitalWrite(greenLed,HIGH);
    digitalWrite(redLed,LOW); 
  }

 customCheck=0;
}

}


///

