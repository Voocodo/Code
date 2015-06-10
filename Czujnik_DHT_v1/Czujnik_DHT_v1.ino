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
vw_set_ptt_inverted(true); //
vw_set_tx_pin(transmit_pin);
vw_set_ptt_pin(transmit_en_pin);
vw_setup(2000);// speed of data transfer Kbps

}

void loop() {
  
char msg[8]={'h','e','l','l','o','w','#',};

 //DHT:
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading temperature or 
  //humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
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
digitalWrite(13,1);
vw_send((uint8_t *)msg,7);
vw_wait_tx(); // Wait until the whole message is gone
digitalWrite(13,0);
count=count+1;

}
