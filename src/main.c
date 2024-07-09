#include <Arduino.h>
#include <notes.h>
// board pinout
#define powerPin 1  // PA2
#define buttonPin 2 // PA3
// #define ledBuzzerPin 5 //not used?
#define redLedPin 8 // PC6
#define buzzerPin 13  // PD4
#define signalPin 7 // PC5
// const
const int disarmDelay=10; // in seconds, how long should power be applied to disarm
const int shortPressDelay=400; // in miliseconds, how long to press the button to register a press
const int longPressDelay=1000; // in miliseconds, how long to press the button to register a press
bool armed = FALSE;
bool disarmed = FALSE;
bool powerOutage = FALSE;
bool buttonPressed = FALSE;
bool longPress = FALSE;
int lastState = LOW;
int alarmInterval = 30; // number of seconds between each beeps
int shortPressCount=0; // register number of time the bouton is short pressed
int longPressCount=0; // register number of time the bouton is long pressed
unsigned long powerCutMillis=0;
unsigned long pluggedMillis=0; // store plugged timestamp
unsigned long buttonPressMillis=0; // store button press timestamp
unsigned long lastAlarmMillis=0; // store previous alarm timestamp

void setup()  { // setup pins I/O
  pinMode(buttonPin,INPUT);
  pinMode(powerPin,INPUT);
  pinMode(signalPin,INPUT);
  pinMode(redLedPin,OUTPUT);
  // pinMode(ledBuzzerPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
}

void disarm(){
  disarmed = TRUE; // disarm the system
  armed = FALSE; // disarm the system
  powerCutMillis = 0; // reset power cut timestamp so we can detect outage
  digitalWrite(redLedPin, LOW); // turn off red led onboard
  play_disarming(buzzerPin); // play disarm sound
}

void arm(){
  armed = TRUE; // arm the system
  digitalWrite(redLedPin, HIGH); // turn on red led onboard      
  play_arming(buzzerPin); // play arming sound
  shortPressCount=0;
  longPressCount=0;
}

void loop() {
  int mainPowerState = digitalRead(powerPin); // HIGH when plugged
  int buttonState = digitalRead(buttonPin); //LOW when pressed
  int signalState = digitalRead(signalPin); //LOW when activated
  unsigned long currentMillis=millis(); // current time marker

  if (mainPowerState == HIGH){ // when main power connected
    if ((armed == FALSE) && (disarmed == FALSE)){ // if not armed, arm buzzer
      pluggedMillis=millis();
      arm();
    }
    if (powerOutage == TRUE){ //if there is an outage
      pluggedMillis=millis(); // store plugged timestamp
      powerCutMillis = 0; // reset power cut timestamp so we can detect outage
      powerOutage = FALSE; // register power outage
    }
  }
  // when main power is disconnected
  if (mainPowerState == LOW){ // when main power connected
    if (armed == TRUE){ // if buzzer is armed
      if (powerOutage == FALSE){ // if no outage is present
        powerCutMillis=millis(); // store power cut timestamp
        powerOutage=TRUE; // register power outage
        lastAlarmMillis=powerCutMillis;
      }
      
      if (round((currentMillis - lastAlarmMillis)/1000) >= alarmInterval){ // when power outage duration is equal or more than alarmInterval
        lastAlarmMillis=millis(); // store current time for next alarm
        play_alarm(buzzerPin); // play alarm
      }
      if (powerOutage == TRUE){ // if there is an outage
        if ((round((powerCutMillis - pluggedMillis)/1000) > (disarmDelay-1)) && (round((powerCutMillis - pluggedMillis)/1000) < (disarmDelay+1))){ // and power was applied for disarmDelay ~ +/- 1s
          disarm();
        }
      }
    }
    if (disarmed == TRUE){
      armed = FALSE;
      disarmed = FALSE;
    }
  }
  // SIGNALBUZZER
  if (signalState == LOW){ // when signal is enable play sound
    play_signal(buzzerPin);
  }
  // Disarm with button
  if ((armed == TRUE) && (longPressCount >= 3 )){ // if the button is pressed disarm the system
    longPressCount=0;
    disarm();
  }
  // BUTTON MANAGEMENT
  if(lastState == HIGH && buttonState == LOW) {
    buttonPressMillis = millis();
    buttonPressed = true;
    longPress = false;
  } else if(lastState == LOW && buttonState == HIGH) {
    buttonPressed = false;
  }
  if(buttonPressed == true && longPress == false) {
    long pressDuration = millis() - buttonPressMillis;

    if( pressDuration > shortPressDelay && pressDuration < longPressDelay ){
      shortPressCount=shortPressCount+1;
    }

    if( pressDuration > longPressDelay ) {
      longPressCount=longPressCount+1;
      longPress = true;
      play_button(buzzerPin);
    }
  }
  // save the the last state
  lastState = buttonState;
}