/* By- Nimit Khandelwal
 *  ARDUINO EXAMPLE -INTERFACE  MULTIPLE RFID-EM-4095 WITH UNO
 *  program to interface multiple rfid_em4095 with arduino uno-r3
 */
#include <SoftwareSerial.h>

SoftwareSerial rfid1(2, 3); // RX, TX
SoftwareSerial rfid2(4, 5); // RX, TX
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
 


  Serial.println("---RFID Reader---");

  // set the data rate for the SoftwareSerial port
  
  rfid2.begin(9600);
  rfid1.begin(9600);

}

void loop() // run over and over
{
  rfid1.listen();
   for (unsigned long duration = millis(); millis() - duration < 1000;)
  {
  if (rfid1.available()){
    
      Serial.write(rfid1.read());
      
    }
  }
   
 rfid2.listen();
   for (unsigned long duration = millis(); millis() - duration < 1000;)
  {
    
    if (rfid2.available())
    { 
      Serial.write(rfid2.read());
      
      }
  }
    

}
