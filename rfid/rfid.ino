/*
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC (that is: a RFID
 * Tag or Card) using a MFRC522 based RFID Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout
 * below), load this sketch into Arduino IDE then verify/compile and upload it.
 * To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M).
 * When you present a PICC (that is: a RFID Tag or Card) at reading distance
 * of the MFRC522 Reader/PCD, the serial output will show the ID/UID, type and
 * any data blocks it can read. Note: you may see "Timeout in communication"
 * messages when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs
 * presented (that is: multiple tag reading). So if you stack two or more
 * PICCs on top of each other and present them to the reader, it will first
 * output all details of the first and then the next PICC. Note that this
 * may take some time as all data blocks are dumped, so keep the PICCs at
 * reading distance until complete.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//

// Rfid:
byte readCard[4];
byte karta1[4] = {96,250,228,69};
byte karta2[4] = {65,95,13,36};
byte checkKarta1=0;
byte checkKarta2=0;
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	ShowReaderDetails();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
}

void loop() {
  lookForRfidCard();
}

void lookForRfidCard ()
{
 	// Szukaj karty RFID:
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
}

	// Wybierz jedna z kart:
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

  //Zczytaj kod UID (4 bajty):
  for (int i = 0; i < 4; i++) 
  {  
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i]);
    Serial.print ("  ");
    }
   Serial.println(""); 
   mfrc522.PICC_HaltA(); // Koniec czytania karty
 
   checkKarta1=0;
   checkKarta2=0;
  for (int i = 0; i < 4; i++) 
  {  
     if (readCard[i] == karta1[i])
     {
     checkKarta1++;  
     }
   
     if (readCard[i] == karta2[i])
     {
     checkKarta2++;  
     }
  }
if (checkKarta1==4)
{
  Serial.println("Otwieram! Karta 1.");
  Serial.println("-------------------------------------"); 
}

else if (checkKarta2==4)
{
  Serial.println("Otwieram! Karta 2.");
  Serial.println("-------------------------------------"); 
}
else 
Serial.println("Niepoprawna karta!");

  
}

//void checkData()
//
//{
//   
//}

void ShowReaderDetails() {
	// Get the MFRC522 software version
	byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
	Serial.print(F("MFRC522 Software Version: 0x"));
	Serial.print(v, HEX);
	if (v == 0x91)
		Serial.print(F(" = v1.0"));
	else if (v == 0x92)
		Serial.print(F(" = v2.0"));
	else
		Serial.print(F(" (unknown)"));
	Serial.println("");
	// When 0x00 or 0xFF is returned, communication probably failed
	if ((v == 0x00) || (v == 0xFF)) {
		Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
	}
}
