#include <Arduino.h>
#include <notes.h>
// const
#define powerPin 1  // PA2
#define buttonPin 2 // PA3
// #define ledBuzzerPin 5 // not used?
#define redLedPin 8 // PC6
#define buzzerPin 13  // PD4
#define signalPin 7 // PC5
#define testPin 0

unsigned long powerCutMillis = 0; // store unplugged timestamp
unsigned long powerDisarmMillis = 0; // store power disarm timestamp
unsigned long pluggedTimeStamp = 0; // store plugged timestamp
unsigned long unpluggedTimeStamp = 0; // store unplugged timestamp
unsigned long repluggedTimeStamp = 0; // store replugged timestamp
bool armed = FALSE; // binary value to toggle arming of system
bool plugged = FALSE; // binary value to toggle arming of system
int alarmInterval = 10; // number of seconds between each beeps
int disarmDelay = 5;  // number of seconds after which system is disarmed
int maxPlugDelay = 20;  // max number of second to wait for unactivity

void setup()  { // setup pins I/O
  pinMode(buttonPin,INPUT);
  pinMode(powerPin,INPUT);
  pinMode(signalPin,INPUT);
  pinMode(testPin,INPUT);
  pinMode(redLedPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
}

void loop()  {
  int mainPowerState = digitalRead(powerPin); // HIGH when plugged
  int buttonState = digitalRead(buttonPin); //LOW when pressed
  int signalState = digitalRead(signalPin); //LOW when pressed
  int testState = digitalRead(testPin); //LOW when pressed
  unsigned long currentMillis=millis(); // current time marker

  if (mainPowerState == HIGH) { // when power is plugged
    // digitalWrite(redLedPin, HIGH); // turn on onboard red LED
    if (signalState == LOW){ // when signal is received from FC
      // alarm(buzzerPin); // play alarm
    }
    if (pluggedTimeStamp == 0){ // when plugged timestamp is still not set
      pluggedTimeStamp = millis(); // store plugged timestamp
      // powerCutMillis=0; // reset power loss timestamp
    }
    if (armed == FALSE ){ // if not armed, arm buzzer
      armed = TRUE; // toggle armed var
      plugged = TRUE; // toggle plugged var
      // arming(buzzerPin); // play arming sound
    }
    if ((armed == TRUE) && (plugged == FALSE)){
      repluggedTimeStamp = millis();
      plugged = TRUE; // toggle plugged var
      // blink_led(redLedPin,2);
    }else{
      powerCutMillis = millis(); // set time power cut timestamp so it can ring next interval
    }
    // int pluggedElapsedSecs = round((currentMillis - pluggedTimeStamp)/1000) ;
    // if ( pluggedElapsedSecs >= maxPlugDelay ){ // when power cut time is equal or more than alarmInterval
    //   pluggedTimeStamp=millis(); // set plug timestamp so it can ring next interval
    //   alarm(buzzerPin); // play alarm
    // }
  }else { // when main power is disconnected
    // digitalWrite(redLedPin, LOW); // turn off red led onboard
    if (armed == TRUE){ // if buzzer is armed
      if(plugged == TRUE){
        plugged = FALSE; // toggle plugged var
      }
      if (powerCutMillis == 0){ // if not timestamp is stored
        powerCutMillis=millis(); // store power cut timestamp
      }
      if (unpluggedTimeStamp == 0){ // when plugged timestamp is still not set
        unpluggedTimeStamp = millis(); // store plugged timestamp
        // powerCutMillis=0; // reset power loss timestamp
      }

      if ((powerCutMillis != 0) && ( powerDisarmMillis == 0)){ // if not timestamp is stored
        powerDisarmMillis=millis(); // store power cut timestamp
      }
      int unpluggedElapsedSecs = round((millis() - powerCutMillis)/1000) ;
      if ( unpluggedElapsedSecs >= alarmInterval ){ // when power cut time is equal or more than alarmInterval
        powerCutMillis = millis(); // set time power cut timestamp so it can ring next interval
        // pluggedTimeStamp=0; // reset plugged timestamp
        // alarm(buzzerPin); // play alarm
      }
      if ((repluggedTimeStamp != 0) && (plugged == FALSE) && (powerDisarmMillis != 0)){
        // blink_led(redLedPin,2);
        int repluggedElapsedSecs = round((powerDisarmMillis - repluggedTimeStamp)/1000); //calculates replugged time
        if ((repluggedElapsedSecs > disarmDelay-1) && (repluggedElapsedSecs < disarmDelay+1)) {
          armed = FALSE;
          powerCutMillis = 0; // reset power cut timestamp so the buzzer can be armed again
          // disarming(buzzerPin); // play disarm sound
        }
      }
    }else{
      powerCutMillis = 0; // store unplugged timestamp
      pluggedTimeStamp = 0; // store plugged timestamp
      unpluggedTimeStamp = 0; // store unplugged timestamp
    }
  }
  if (buttonState == LOW) {
    digitalWrite(redLedPin, HIGH);
  }
  else{
    digitalWrite(redLedPin, LOW);
  }
}