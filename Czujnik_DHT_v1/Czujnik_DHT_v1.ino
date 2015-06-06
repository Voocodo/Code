// 25.04.2015 
// 6.06.2015 Bezprzewodowy czujnik 433 MhZ



#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to


#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

#define rfTransmitPin 4
#define ledPin 13   

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
  // Dht:
  dht.begin();
  
  //Rfid:
   pinMode(rfTransmitPin, OUTPUT);     
   pinMode(ledPin, OUTPUT);    
}

void loop() {
  
  //Rfid:
  
  for(int i=4000; i>5; i=i-(i/3)){
     digitalWrite(rfTransmitPin, HIGH);     //Transmit a HIGH signal
     digitalWrite(ledPin, HIGH);            //Turn the LED on
     delay(2000);                           //Wait for 1 second
     
     digitalWrite(rfTransmitPin,LOW);      //Transmit a LOW signal
     digitalWrite(ledPin, LOW);            //Turn the LED off
     delay(i);                            //Variable delay
   }
 
 //DHT:
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
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
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
}
