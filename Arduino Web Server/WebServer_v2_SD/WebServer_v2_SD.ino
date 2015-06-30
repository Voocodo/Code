/*
26.06.2015 Serbia
30.06.2015 Adding SD file loading
 */

#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>



// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,88,225);
// Initialize the Ethernet server library
EthernetServer server(80);

// ???????????:
File webFile;
 
byte temp;
char ledPin=13;
const byte chipSelect = 4;
String dataString = "420";
boolean ledStatus=0;
String httpRequest;
 
void setup() {
 //Pins:
 pinMode(ledPin,OUTPUT);
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.begin(9600);
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
  Serial.print("Initializing SD card...");
    
    //Sd card:
    pinMode(4, OUTPUT);
    //Write pin 4 high?
    
  if (!SD.begin(chipSelect)) {
    Serial.println("ERROR: Card failed, or not present");
    return;
  }
  //SD card OK:
    Serial.println("card initialized.");
      Serial.println(FreeRam());

}


void loop() {
  
  //Writing data to SD:
  File dataFile = SD.open("temp_log.txt", FILE_WRITE);
  // if the file is available, write to it:
  temp=420;
  if (dataFile) {
    dataFile.println(temp);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) { // client data available to read
        char c = client.read();
        //LED:
        httpRequest += c;  //save the HTTP request 1 char at a time
        
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          ///////////////////
          webFile = SD.open("index.htm");
          if (webFile) {
             while(webFile.available()) {
             client.write(webFile.read()); // send web page to client
             ProcessCheckbox(client);
             Serial.print(httpRequest);
                        }
                        webFile.close();
                    }
                    break;
                }
                
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
          //////////////////
          
}
        

//////////////////

void ProcessCheckbox(EthernetClient cl)
{
    if (httpRequest.indexOf("LED=1") > -1) {  // see if checkbox was clicked
        // the checkbox was clicked, toggle the LED
        if (ledStatus) {
            ledStatus = 0;
        }
        else {
            ledStatus = 1;
        }
    }
    
    if (ledStatus) {    // switch LED on
        digitalWrite(ledPin, HIGH);
        // checkbox is checked
        cl.println("<input type=\"checkbox\" name=\"LED\" value=\"1\" \
        onclick=\"submit();\" checked>LED2");
    }
    else {              // switch LED off
        digitalWrite(2, LOW);
        // checkbox is unchecked
        cl.println("<input type=\"checkbox\" name=\"LED\" value=\"1\" \
        onclick=\"submit();\">LED");
    }
}


