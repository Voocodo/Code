//Datalog:
// 25.04.2015 
// 6.06.2015 Bezprzewodowy czujnik 433 MhZ
// 7.06 uruchamianie
// 10.06 edycja GitHub



//Biblioteki:
#include <DHT.h>
#include <VirtualWire.h>

//Piny:
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define transmit_pin  12
#define transmit_en_pin 3
#define ledPin 13

//Inicjalizacja DHT:
DHT dht(DHTPIN, DHTTYPE);

//Zmienne:
byte count = 1;

void setup() {
  Serial.begin(9600); 
  // Dht:
  dht.begin();
  
  //Rfid:
vw_set_ptt_inverted(true);
vw_set_tx_pin(transmit_pin);
vw_set_ptt_pin(transmit_en_pin);
vw_setup(2000);// speed of data transfer Kbps
}


void loop() {
  
char msg[8]={'s','i','e','m','a','0','0',};

 //DHT:
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  
    //Rfid:
msg[5]=t;  
msg[6] = count;
 Serial.print(msg[0]);
Serial.print(msg[1]); 
Serial.print(msg[2]); 
Serial.print(msg[3]); 
Serial.print(msg[4]); 
Serial.print(msg[5]); 
Serial.print(msg[6]); 

digitalWrite(13,1); //Wysylanie wiadomosci
vw_send((uint8_t *)msg,7);
vw_wait_tx(); // Wait until the whole message is gone
digitalWrite(13,0);
count=count+1;
}
