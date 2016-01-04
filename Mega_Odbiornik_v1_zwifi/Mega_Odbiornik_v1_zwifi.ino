//30.08 testowanie
//9.09.2015 proba zmieszania z sd
//19.09 logowanie na karte sd
//20.09 dodanie cisnienia
//21.09 Dodanie I2C
//25.09. RTC
//10.10 mieszanie


#include <VirtualWire.h> //Biblioteka do komunikacji z I2C
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Wire.h>

//RTC:
#include <virtuabotixRTC.h>
virtuabotixRTC myRTC(5, 4, 3);
int rtcGnd=6;
int rtcVcc=7;

//433 MHZ:
//const int led_pin = 8;
//const int receive_pin = 8; // zmiana z 22 na 8 w uno
//const int transmit_en_pin = 3;

const int VCC433 = 28;
const int GND433 = 22;
const int receive_pin = 24;


//const int receive_pin = 8; // zmiana z 22 na 8 w uno
const int transmit_en_pin = 3;

//I2C:
#define SLAVE_ADDRESS 0x60
byte daneI2C[6];
int alarmInfo = 0;
byte receiveFlag = 0;

//Inne:
int zmienna[4];
float temp = 0;
float wilg = 0;
float cisn = 0;
int count = 0;
char c;

//Wifi:

byte mac[] = {0xDE, 0xAD, 0xCE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 88, 88);
EthernetServer server(80);

//SD:
File webFile;
File myFile;
File alarmFile;
unsigned long datalog_time = 0;
int datalog_count = 0;




void setup()
{
  Serial.begin(9600);

//RTC:
pinMode(rtcGnd,OUTPUT);
pinMode(rtcVcc,OUTPUT);
digitalWrite(rtcGnd,LOW);
digitalWrite(rtcVcc,HIGH);
  //I2C:
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //433 MHZ:
 pinMode(GND433,OUTPUT);
  pinMode(VCC433,OUTPUT);
  digitalWrite(GND433,LOW);
  digitalWrite(VCC433,HIGH);
  pinMode(13, OUTPUT);
  pinMode(receive_pin, INPUT);
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_pin(transmit_en_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running

  //Wifi:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());


  //SD:
  Serial.println("Inicjalizacja karty SD:");

  if (!SD.begin(4))
  {
    Serial.println("Błąd karty SD");
    return;
  }
  Serial.println("Sukces - karta SD zainicjalizowana");

  //Znajdz plik index.htm:
  if (!SD.exists("index.htm")) {
    Serial.println("Blad - brak pliku index.htm!");
    return;
  }
  Serial.println("SUCCESS - Found index.htm file.");

  if (!SD.exists("datalog.txt")) {
    Serial.println("ERROR - Can't find datalog.txt file!");
    return;  // can't find index file
  }
  Serial.println("SUCCESS - Found datalog.txt file.");

//RTC: Ustawianie aktualmego czasu dla RTC
  //myRTC.setDS1302Time(00, 18, 18, 7, 15, 11, 2015);
 
  Serial.println("Aktualny czas:");
  myRTC.updateTime();
  Serial.print(myRTC.dayofmonth);                                                                        //|
  Serial.print("/");                                                                                     //|
  Serial.print(myRTC.month);                                                                             //|
  Serial.print("/");                                                                                     //|
  Serial.print(myRTC.year);                                                                              //|
  Serial.print(",");
  Serial.print(myRTC.hours);                                                                             //|
  Serial.print(":");                                                                                     //|
  Serial.print(myRTC.minutes);                                                                           //|
  Serial.print(":");                                                                                     //|
  Serial.print(myRTC.seconds);

}

void loop()
{

  //Aktualizacja czasu RTC:
  myRTC.updateTime();
  //  Serial.print("Current Date / Time: ");                                                                 //|
  //  Serial.print(myRTC.dayofmonth);                                                                        //|
  //  Serial.print("/");                                                                                     //|
  //  Serial.print(myRTC.month);                                                                             //|
  //  Serial.print("/");                                                                                     //|
  //  Serial.print(myRTC.year);                                                                              //|
  //  Serial.print("  ");                                                                                    //|
  //  Serial.print(myRTC.hours);                                                                             //|
  //  Serial.print(":");                                                                                     //|
  //  Serial.print(myRTC.minutes);                                                                           //|
  //  Serial.print(":");                                                                                     //|
  //  Serial.println(myRTC.seconds);


  //433 MHZ:
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    //int i;

    //digitalWrite(led_pin, HIGH); // Flash a light to show received good message
    // Message with a good checksum received, print it.
    //Serial.print("Got: ");

    //	for (i = 0; i < buflen; i++)
    //	{
    //     Serial.print("buf[");
    //     Serial.print(i, DEC);
    //     Serial.print("]=");
    //     Serial.print(buf[i], DEC);
    //     Serial.print("  ");
    //	}

    for (int i = 0; i < 5; i++)
    {
      zmienna[i] = word(buf[i * 2 + 1], buf[i * 2]);
    }
    Serial.println();
   // digitalWrite(led_pin, LOW);
    temp = zmienna[0];
    wilg = zmienna[1];
    cisn = zmienna[2];
    count = zmienna[3];
    temp = temp / 100;
    wilg = wilg / 100;
    Serial.println("---------------------------------");
    Serial.print("Temperatura: ");
    Serial.println(temp);
    Serial.print("Wilgotnosc: ");
    Serial.println(wilg);
    Serial.print("Cisnienie: ");
    Serial.println(cisn);
    Serial.print("Cout:");
    Serial.println(count);
    Serial.println("---------------------------------");

    //I2C: Nadawanie danych I2C
    daneI2C[0] = lowByte(zmienna[2]);
    daneI2C[1] = highByte(zmienna[2]);
    daneI2C[2] = lowByte(zmienna[0]);
    daneI2C[3] = highByte(zmienna[0]);
    daneI2C[4] = lowByte(zmienna[1]);
    daneI2C[5] = highByte(zmienna[1]);


    //Zapisz temp do karty SD

    if (datalog_time < millis())
    {
      Serial.println("Logowanie do karty SD");
      datalog_count = datalog_count + 1;
      myFile = SD.open("datalog.txt", FILE_WRITE);
      myFile.print(datalog_count);
      myFile.print(",");
      myFile.print(myRTC.dayofmonth);                                                                        //|
      myFile.print("/");                                                                                     //|
      myFile.print(myRTC.month);                                                                             //|
      myFile.print("/");                                                                                     //|
      myFile.print(myRTC.year);                                                                              //|
      myFile.print(",");
      myFile.print(myRTC.hours);                                                                             //|
      myFile.print(":");                                                                                     //|
      myFile.print(myRTC.minutes);                                                                           //|
      myFile.print(":");                                                                                     //|
      myFile.print(myRTC.seconds);
      myFile.print(",");
      myFile.print(millis() / 1000);
      myFile.print(",");
      myFile.print(temp);
      myFile.print(",");
      myFile.print(wilg);
      myFile.print(",");
      myFile.println(cisn);
      myFile.close();
      datalog_time = millis() + 30000; //Kolejny wpis za 30 sekund
    }

  }


  ////Wifi://
  //Nasluchuj informacji od klienta:
  EthernetClient client = server.available();

  //Jesli pojawia sie klient:
  if (client) {
    Serial.println("Nowy klient");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

          /////////////////////////////////////
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          //          client.println("<meta charset='ISO-8859-1'>");
          //client.println("<meta http-equiv='Content-type' charset=utf-8/>");


          //Wgranie arkusza CSS:

          webFile = SD.open("index.htm");        // open web page file
          if (webFile) {
            while (webFile.available()) {
              client.write(webFile.read()); // send web page to client
            }
            webFile.close();
          }

          client.print("<body><h1>System monitorujacy.<br><br>");
          client.print("Czas:  ");
          client.println(myRTC.hours);
          client.println(":");
          client.println(myRTC.minutes);
          client.println(":");
          client.println(myRTC.seconds);
          client.print("</h1>");
          client.print("<table text-align='left'><tr><td><p>   Temperatura:</p></td><td><h1>");
          client.print(temp);
          client.print("</h1></td></tr>");
          client.print("<tr><td><p> Wilgotnosc:</p></td><td><h1>");
          client.print(wilg);
          client.print("</h1></td></tr>");
          client.print("<tr><td><p>Cisnienie:</p></td><td><h1>");
          client.print(cisn);
          client.print("</h1></td></tr></table>");
          client.print("<br>Last alarm info:");
          client.print(alarmInfo);
          client.print("</body></html>");


          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }



  //I2C receive

  if (receiveFlag == 1) // If there is new event:
  {

    alarmFile = SD.open("alarm.txt", FILE_WRITE); //Save time on SD file...

    alarmFile.println("");
    alarmFile.print(myRTC.dayofmonth);                                                                        //|
    alarmFile.print("/");                                                                                     //|
    alarmFile.print(myRTC.month);                                                                             //|
    alarmFile.print("/");                                                                                     //|
    alarmFile.print(myRTC.year);                                                                              //|
    alarmFile.print(",");
    alarmFile.print(myRTC.hours);                                                                             //|
    alarmFile.print(":");                                                                                     //|
    alarmFile.print(myRTC.minutes);                                                                           //|
    alarmFile.print(":");                                                                                     //|
    alarmFile.print(myRTC.seconds);
    alarmFile.print(",");
    alarmFile.print(alarmInfo);
    alarmFile.print(",     ");


    if (alarmInfo == 0) // Save event type on sd file
    {

      alarmFile.println("Wrong PIN");
      // alarmFile.print(",");
    }

    if (alarmInfo == 1)
    {

      alarmFile.println("Correct PIN. Alarm deactivated.");
      //   alarmFile.print(",");
    }

    if (alarmInfo == 2)
    {
      alarmFile.println("Correct PIN. Alarm activated.");
      // alarmFile.print(",");
    }

    if (alarmInfo == 7)
    {

      alarmFile.println("Motion in room 1 detected!");
      // alarmFile.print(",");
    }

    if (alarmInfo == 12)
    {

      alarmFile.println("RFID card used!");
      // alarmFile.print(",");
    }

    if (alarmInfo == 18)
    {

      alarmFile.println("Door opened!");
      // alarmFile.print(",");
    }

    // alarmFile.print("Event code: ");


    alarmFile.close();


    Serial.print("alarmInfo: ");
    Serial.println(alarmInfo);
    receiveFlag = 0;     // Ready to receive another event.
  }


}

// I2C handling:

void requestEvent()
{
  Serial.println("Request from Master. Sending.");
  Wire.write(daneI2C, 6);
}

void receiveEvent(int bytes)
{
  if (Wire.available() != 0)
  {
    alarmInfo = Wire.read();
    receiveFlag = 1;
  }
}




