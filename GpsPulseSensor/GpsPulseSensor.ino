
/*
 By- Nimit Khandelwal
* program for Interfacing pulse sensor and ublox NEO-6m-001 to arduino uno.
* interface pulse sensor   
* >>> Pulse Sensor purple wire goes to Analog Pin 0 <<<
* Pulse Sensor sample aquisition and processing happens in the background via Timer 2 interrupt. 2mS sample rate.
* PWM on pins 3 and 11 will not work when using this code, because we are using Timer 2!
* The following variables are automatically updated:
* Signal :    int that holds the analog signal data straight from the sensor. updated every 2mS.
* IBI  :      int that holds the time interval between beats. 2mS resolution.
* BPM  :      int that holds the heart rate value, derived every beat, from averaging previous 10 IBI values.
* QS  :       boolean that is made true whenever Pulse is found and BPM is updated. User must reset.
* Pulse :     boolean that is true when a heartbeat is sensed then false in time with pin13 LED going out.

* Interfacing gps 
*  This code also demonstrates the normal use of a TinyGPS object.
*  It requires the use of SoftwareSerial, and assumes that you have a
   9600-baud serial GPS device hooked up on pins 5(rx) and 4(tx).
*/

#include <SoftwareSerial.h>

#include <TinyGPS.h>

TinyGPS gps;
//setup software Serial for GPS at pin 5(RX),4(TX)
SoftwareSerial ss(5, 4);
// Sensor and pins variables
int pulsePin = 0;
int blinkPin = 13;

// Pulse rate variable
volatile int BPM;  
int counts=0;  

// Raw signal
volatile int Signal;

// Interval between beats
volatile int IBI = 600;

// Becomes true when the pulse is high
volatile boolean Pulse = false;

// Becomes true when Arduino finds a pulse
volatile boolean QS = false;


void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  interruptSetup();
}

void loop()
{
 //disable timer 2 to encode,decode gps data and send data to base station through put request from esp8266 module
    TCCR2A = 0x00;// ENABLE PWM ON DIGITAL PINS 3 AND 11
    TIMSK2 = 0x00;// DISABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
   for (unsigned long duration = millis(); millis() - duration < 2000;)
  {
    gps_data();
    }
     // set timer two to two milliseconds for pulse readings
    TCCR2A = 0x02;// DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
    TIMSK2 = 0x02;// ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
 for (unsigned long duration = millis(); millis() - duration < 4000;)
  {
    pulse_data();
    }
  delay(500);
 
}
void gps_data(){
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      
      char c = ss.read();
//       Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
 
  }
  void pulse_data(){
    if (QS == true) {
        
    // Print heart rate      
    Serial.print("Heart rate: ");            
    Serial.println(BPM);
        
    // Reset the Quantified Self flag for next time      
    QS = false;                       
   }
  
  // Wait 20 ms
  delay(20);
    }
