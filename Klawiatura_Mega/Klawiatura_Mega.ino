/*

1.09.2015
*/

#include <Keypad.h>
#include <LiquidCrystal.h>




////Led:
int greenLed = 2;
int redLed = 3;

  //LCD:
  LiquidCrystal lcd(30, 31, 32, 33, 34, 35);


/////////Keypad:
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
boolean customCheck =0;
char kodPin[4] = {'0','0','0','0'};

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

void setup(){
  Serial.begin(9600);
  //LCD:
    lcd.begin(20, 4);
  //
  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  
  digitalWrite(redLed,HIGH);
  digitalWrite(greenLed,HIGH);
  lcd.setCursor(0,0);
  lcd.print("Wpisz kod PIN:");
  lcd.setCursor(14,0);
  
  //LCD pin name 	RS 	EN 	DB4 	DB5 	DB6 	DB7
  //Arduino pin  	30 	31 	32 	33 	34 	35
  ///
}
  
void loop(){
  
  
  digitalWrite(redLed,HIGH);
  digitalWrite(greenLed,HIGH);
  char customKey = customKeypad.getKey();
  
  if (customKey){
  //Serial.println(customKey);
  digitalWrite(redLed,LOW);
  digitalWrite(greenLed,LOW);
  klawisz=customKey;
  Serial.println(klawisz);
  Serial.println(customCheck);
  
customCheck = customKeypad.keyStateChanged();
  }
   //LCD:
if (customCheck==1)
{
  Serial.println(customKey);
  if (customKey=='*')
  {
    lcd.write("brawo");
  }

 customCheck=0;
}



}
