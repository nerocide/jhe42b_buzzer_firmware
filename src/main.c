#include <Arduino.h>
#include <notes.h>
// const
#define powerPin 1  // PA2
#define buttonPin 2 // PA3
// #define ledBuzzerPin 5 // not used?
#define redLedPin 8 // PC6
#define buzzerPin 13  // PD4
#define signalPin 7 // PC5

const int disarmDelay=10; // in seconds, how long should power be applied to disarm
const int shortPressDelay=100; // in miliseconds, how long to press the button to register a press
const int longPressDelay=500; // in miliseconds, how long to press the button to register a press
const int graceDelay=50; // in miliseconds, how long to press the button to register a press
bool armed = FALSE;
bool powerOutage = FALSE;
int alarmInterval = 30; // number of seconds between each beeps
int shortPressCount=0; // register number of time the bouton is short pressed
int longPressCount=0; // register number of time the bouton is long pressed
unsigned long powerCutMillis=0;
unsigned long pluggedMillis=0; // store plugged timestamp
unsigned long buttonPressMillis=0; // store button press timestamp

void setup()  { // setup pins I/O
  pinMode(buttonPin,INPUT);
  pinMode(powerPin,INPUT);
  pinMode(signalPin,INPUT);
  pinMode(redLedPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
}

void disarm(){
  armed = FALSE; // disarm the system
  powerCutMillis = 0; // reset power cut timestamp so we can detect outage
  digitalWrite(redLedPin, LOW); // turn off red led onboard
  play_disarming(buzzerPin); // play disarm sound
}

void loop() {
  int mainPowerState = digitalRead(powerPin); // HIGH when plugged
  int buttonState = digitalRead(buttonPin); //LOW when pressed
  int signalState = digitalRead(signalPin); //LOW when activated
  unsigned long currentMillis=millis(); // current time marker
  

  if (mainPowerState == HIGH){ // when main power connected
    if(pluggedMillis == 0){ // and no plugged timestamp
      pluggedMillis=millis(); // store plugged timestamp
      powerCutMillis=0; // reset power loss timestamp
    }
    if (armed == FALSE){ // if not armed, arm buzzer
      armed = TRUE; // arm the system
      digitalWrite(redLedPin, HIGH); // turn on red led onboard      
      play_arming(buzzerPin); // play arming sound
    }
    if (powerOutage == TRUE){ //if there is an outage
      pluggedMillis=millis(); // store plugged timestamp
      powerCutMillis = 0; // reset power cut timestamp so we can detect outage
    }
  }else{ // when main power is disconnected
    if (armed == TRUE){ // if buzzer is armed
      if (powerOutage == FALSE){ // if no outage is present
        powerCutMillis=millis(); // store power cut timestamp
        powerOutage = TRUE; // register power outage
      }
      if (round((currentMillis - powerCutMillis)/1000) >= alarmInterval){ // when power outage duration is equal or more than alarmInterval
        pluggedMillis=0; // reset plugged timestamp
        play_alarm(buzzerPin); // play alarm
      }
      if (powerOutage == TRUE){ // if there is an outage
        if ((round((currentMillis - pluggedMillis)/1000) > (disarmDelay-1)) && (round((currentMillis - pluggedMillis)/1000) < (disarmDelay+1))){ // and power was applied for disarmDelay ~ +/- 1s
          disarm();
        }
        if (shortPressCount > 1){ // if the button is pressed disarm the system
          disarm();
          shortPressCount=0;
        }
      }
    }
  }
  // SIGNALBUZZER
  if (signalState == HIGH){ // when signal is enable play sound
    digitalWrite(redLedPin, HIGH);
    play_alarm(buzzerPin);
  }else{
    digitalWrite(redLedPin, LOW);
  }
  // BUTTON MANAGEMENT
  if (buttonState == LOW) {
    digitalWrite(redLedPin, HIGH);
    if(buttonPressMillis == 0){
      buttonPressMillis=millis();
    }
  }else{
    digitalWrite(redLedPin, LOW);
    // detect short press
    if ((round(currentMillis - buttonPressMillis) > shortPressDelay-graceDelay) && (round(currentMillis - buttonPressMillis) < shortPressDelay+graceDelay)){
      shortPressCount=shortPressCount+1;
      buttonPressMillis=0;
    }
    // detect short press
    if ((round(currentMillis - buttonPressMillis) > longPressDelay-graceDelay) && (round(currentMillis - buttonPressMillis) < longPressDelay+graceDelay)){
      longPressCount=longPressCount+1;
      buttonPressMillis=0;
    }
  }
}