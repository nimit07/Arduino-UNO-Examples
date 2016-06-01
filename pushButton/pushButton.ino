/*
 * By-Nimit Khandelwal
 * ARDUINO EXAMPLE -  Add PUSH Button with Arduino 
 */
const int buttonPin = 11;   // the number of the pushbutton pin
const int ledPin =  8;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int state_sos=0;
void setup() {
  Serial.begin(115200);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState ==HIGH ) {
    Serial.print("HIGH");
    // turn LED on:
  } 
  else{
    Serial.print("LOW");
    }
    delay(1000);
  }

