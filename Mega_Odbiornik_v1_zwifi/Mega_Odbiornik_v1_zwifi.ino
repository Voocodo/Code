//30.08 testowanie
//9.09.2015 proba zmieszania z sd
//19.09 logowanie na karte sd
//20.09 dodanie cisnienia
//21.09 Dodanie I2C
//25.09. RTC


#include <VirtualWire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Wire.h>
//RTC:
#include <virtuabotixRTC.h>       
virtuabotixRTC myRTC(5, 6, 7);

const int led_pin = 8;
const int receive_pin = 22;
const int transmit_en_pin = 3;


//I2C:
#define SLAVE_ADDRESS 0x60
byte daneI2C[6];


int zmienna[4];
float temp =0;
float wilg =0;
float cisn =0;
int count=0;
char c;
//Wifi:////////////////////

byte mac[] = {0xDE, 0xAD, 0xCE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 88, 88);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);
////////////////////////////
//SD:
File webFile;
File myFile;
unsigned long datalog_time=0;
int datalog_count =0;
///


void setup()
{
    Serial.begin(9600);	// Debugging only
    //I2C:
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
  
    pinMode(13,OUTPUT);
    pinMode(receive_pin,INPUT);
  
    // Initialise the IO and ISR
    //vw_set_tx_pin(transmit_pin);
    vw_set_rx_pin(receive_pin);
    vw_set_ptt_pin(transmit_en_pin);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec

    vw_rx_start();       // Start the receiver PLL running
    
    ////Wifi://////////////
    
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  ////////////////////////////
  //SD:
   Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");
    
    if (!SD.exists("datalog.txt")) {
        Serial.println("ERROR - Can't find datalog.txt file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found datalog.txt file.");
  
  
  //RTC:
  
//myRTC.setDS1302Time(00, 10, 13, 5, 25, 9, 2015);
  
  //////////////////////////
}

void loop()
{
//int h =hour();
myRTC.updateTime();     
//
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


//Serial.print("*********************");
//erial.println(hour());  
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
	int i;

        digitalWrite(led_pin, HIGH); // Flash a light to show received good message
	// Message with a good checksum received, print it.
	Serial.print("Got: ");
	
	for (i = 0; i < buflen; i++)
	{
     Serial.print("buf[");
     Serial.print(i, DEC);
     Serial.print("]=");
     Serial.print(buf[i], DEC);
     Serial.print("  ");
	}

 for (int i = 0; i < 5; i++)
   {
     zmienna[i] = word(buf[i*2+1], buf[i*2]);
     Serial.print("numbers[");
     Serial.print(i, DEC);
     Serial.print("]=");
     Serial.print(zmienna[i], DEC);
     Serial.print("  ");
   }
	Serial.println();
        digitalWrite(led_pin, LOW);
    temp=zmienna[0];
    wilg=zmienna[1];
    cisn=zmienna[2];
    count=zmienna[3];
    temp=temp/100;
    wilg=wilg/100;
    Serial.print("Temp:");
    Serial.print(temp);
    Serial.print("Wilg:");
    Serial.print(wilg);
     Serial.print("Cisn:");
    Serial.println(cisn);
     Serial.print("Cout:");
    Serial.println(count);
    
    //I2C:
  daneI2C[0]=lowByte(zmienna[2]);
  daneI2C[1]=highByte(zmienna[2]);
  daneI2C[2]=lowByte(zmienna[0]);
  daneI2C[3]=highByte(zmienna[0]);
  daneI2C[4]=lowByte(zmienna[1]);
  daneI2C[5]=highByte(zmienna[1]);
    
    
    //Zapisz temp do karty SD
    
     if (datalog_time < millis())
       {
      Serial.println("Logowanie do karty SD");
      datalog_count = datalog_count+1;
      myFile = SD.open("datalog.txt",FILE_WRITE);
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
      myFile.print(millis()/1000);
      myFile.print(",");
      myFile.print(temp);
      myFile.print(",");
      myFile.print(wilg);
       myFile.print(",");
      myFile.println(cisn);
      myFile.close();
      datalog_time=millis()+10000; //Kolejny wpis za 10 sekund
      }
    
    }
    
    
    ////Wifi:////////
     // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
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
          client.println("<meta charset='ISO-8859-1'>");
          client.println("<html>");

                    webFile = SD.open("index.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    
          
          
          // output the value of each analog input pin
         // for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
           // int sensorReading = analogRead(analogChannel);
           
            client.print("<td><p>Temperatura:</p></td><td>");
            client.println(temp);
            client.println("</td>");
            client.print("</tr><tr><td><p>Wilgotnosc:</p></td><td>");
            client.println(wilg);
            client.println("</td></tr>");
            client.println("<tr><td><p>Cisnienie:</p></td><td>");
            client.println(cisn);
            client.println("</td></tr></table></body></html>");

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
    
  }
  ////////////////////
//I2c:

void requestEvent() 
{
  Serial.println("Request from Master. Sending.");
  Wire.write(daneI2C,6);
}

void receiveEvent(int bytes)
{
  if(Wire.available() != 0)
  {
    for(int i = 0; i< bytes; i++)
    {
      int x = Wire.read();
      Serial.print("Status alarmu: ");
      Serial.println(x, HEX);
    }
  }
}
