//Datalog:
// 25.04.2015 
// 6.06.2015 Bezprzewodowy czujnik 433 MhZ
// 7.06 uruchamianie
// 10.06 edycja GitHub
// 30.08 testowanie
// 20.08 dodanie barometru (dziala)
// 15.10 edycja kodu

//Biblioteki:
#include <DHT.h>
#include <VirtualWire.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

//Piny:
#define DHTPIN 2     
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define transmit_pin  12
#define transmit_en_pin 3
#define ledPin 13

//Inicjalizacja DHT:
DHT dht(DHTPIN, DHTTYPE);

//Inicjalizacja BMP:
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

//Zmienne:
byte count = 1;
int zmienna[4]; //Ilosc zmiennych do przeslania
float cisn=0;

//
void displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}




void setup() {
  digitalWrite(13,0);
  Serial.begin(9600); 
  // DHT:
  dht.begin();
  
  //433:
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(transmit_pin);
  vw_set_ptt_pin(transmit_en_pin);
  vw_setup(2000);// speed of data transfer Kbps

  //BMP:
   Serial.println("Pressure Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    //while(1);
  }
  else
  /* Display some basic information on this sensor */
  displaySensorDetails();
}
 

void loop() {
  
  //char msg[3]={'','',''};

 //DHT:
  delay(200);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C ");
  
  //BMP:
  sensors_event_t event;
  bmp.getEvent(&event);
   if (event.pressure)
  {
    /* Display atmospheric pressue in hPa */
    Serial.print("Pressure:    ");
    cisn=event.pressure;
    Serial.print(cisn);
    Serial.println(" hPa");
    float temperature;
    bmp.getTemperature(&temperature);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    Serial.print("Altitude:    "); 
    Serial.print(bmp.pressureToAltitude(seaLevelPressure,
                                        event.pressure)); 
    Serial.println(" m");
    Serial.println("");
  }
  else
  {
    Serial.println("Sensor error");
  }
  

  
    //433 mhz:
//msg[5] = 5;  
//msg[6] = count;
zmienna[0]=t*100;
zmienna[1]=h*100;
zmienna[2]=(int)cisn;
zmienna[3]=count;
//Serial.print(msg[0]);
//Serial.print(msg[1]); 
//Serial.print(msg[2]); 
//Serial.print(msg[3]); 
//Serial.print(msg[4]); 
//Serial.print(msg[5]); 
//Serial.println(msg[6]); 

//Wysylanie wiadomosci:
digitalWrite(13,1); 
vw_send((uint8_t *)zmienna, sizeof(zmienna));
vw_wait_tx(); // Czekaj az calosc zostanie przeslana:
digitalWrite(13,0);
count=count+1;
delay(1000);
}








