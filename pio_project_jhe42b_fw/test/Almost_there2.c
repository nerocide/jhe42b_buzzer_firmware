#include <Arduino.h>
#include <notes.h>
// const
#define powerPin 1  // PA2
#define buttonPin 2 // PA3
// #define ledBuzzerPin 5 // not used?
#define redLedPin 8 // PC6
#define buzzerPin 13  // PD4
#define signalPin 7 // PC5
// #define testPin 7

bool armed = FALSE;
bool powerOutage = FALSE;
unsigned long powerCutMillis=0;
int alarmInterval = 10; // number of seconds between each beeps
unsigned long pluggedMillis=0; // store plugged timestamp
unsigned long unpluggedMillis=0; // store unplugged timestamp
int disarmDelay = 5;

void setup()  { // setup pins I/O
  pinMode(buttonPin,INPUT);
  pinMode(powerPin,INPUT);
  pinMode(signalPin,INPUT);
  pinMode(redLedPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
}

void loop()  {
  int mainPowerState = digitalRead(powerPin); // HIGH when plugged
  int buttonState = digitalRead(buttonPin); //LOW when pressed
  int signalState = digitalRead(signalPin); //LOW when pressed
  unsigned long currentMillis=millis(); // current time marker

  if (mainPowerState == HIGH) { // when main power connected
    if(pluggedMillis == 0){ // and not plugged timestamp
      pluggedMillis=millis(); // store plugged timestamp
      powerCutMillis=0; // reset power loss timestamp
    }
    if ( armed == FALSE ){ // if not armed, arm buzzer
      armed = TRUE;
      digitalWrite(redLedPin, HIGH); // turn on red led onboard      
      arming(buzzerPin); // play arming sound
    }
    if (powerOutage == TRUE){
      if ((round((currentMillis - pluggedMillis)/1000) > disarmDelay-1) && (round((currentMillis - pluggedMillis)/1000) < disarmDelay+1)) {
        armed = FALSE;
        powerCutMillis = 0; // reset power cut timestamp so the buzzer can be armed again
        digitalWrite(redLedPin, LOW); // turn off red led onboard
        disarming(buzzerPin); // play disarm sound
      }
    }
  }else { // when main power is disconnected
    if (armed == TRUE) { // if buzzer is armed
      // if (powerCutMillis == 0){
      powerCutMillis=millis(); // store power cut timestamp
      // }

      powerOutage = TRUE;
      if ( round((currentMillis - powerCutMillis)/1000) >= alarmInterval ) { // when power cut time is equal or more than alarmInterval
        powerCutMillis=millis(); // set time power cut timestamp so it can ring next interval
        pluggedMillis=0; // reset plugged timestamp
        alarm(buzzerPin); // play alarm
      }
      // else{
        // }
    }
  }
}

  // if (buttonState == LOW) {
  //   digitalWrite(redLedPin, HIGH);
  // }
  // else{
  //   digitalWrite(redLedPin, LOW);
  // }

  // if (signalState == HIGH) { // when signal is enable play sound
  //   digitalWrite(redLedPin, HIGH);
  //   alarm(buzzerPin);
  // }
  // else{
  //   digitalWrite(redLedPin, LOW);
  // }

// }