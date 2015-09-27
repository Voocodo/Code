//30.08 testowanie

#include <VirtualWire.h>

const int led_pin = 6;
//const int transmit_pin = 12;
const int receive_pin = 22;
const int transmit_en_pin = 3;

int zmienna[3];

float temp =0;
float wilg =0;

void setup()
{
    delay(1000);
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
    

    
    delay(50);
}

