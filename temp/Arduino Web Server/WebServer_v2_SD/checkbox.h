
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
        cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\" checked>LED2");
    }
    else {              // switch LED off
        digitalWrite(2, LOW);
        // checkbox is unchecked
        cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\">LED2");
    }
}

