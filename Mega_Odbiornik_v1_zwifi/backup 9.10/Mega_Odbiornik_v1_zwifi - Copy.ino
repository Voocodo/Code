//30.08 testowanie
//9.10.2015 proba zmieszania z sd

#include <VirtualWire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

const int led_pin = 6;
//const int transmit_pin = 12;
const int receive_pin = 22;
const int transmit_en_pin = 3;

int zmienna[3];
float temp =0;
float wilg =0;
char c;
//Wifi:////////////////////

byte mac[] = {
  0xDE, 0xAD, 0xCE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 88, 88);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);



////////////////////////////
//SD:
File webFile;

///


void setup()
{
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");
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
  
  
  
  
  
  //////////////////////////
}

void loop()
{
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

 for (int i = 0; i < 3; i++)
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
    temp=temp/100;
    wilg=wilg/100;
    Serial.print("Temp:");
    Serial.print(temp);
    Serial.print("Wilg:");
    Serial.println(wilg);
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
           
            client.print("Temperatura: ");
            client.println(temp);
            client.println("<br />");
            client.print("Wilgotność: ");
            client.println(wilg);
            client.println("<br />");
          
          client.println("</html>");
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


