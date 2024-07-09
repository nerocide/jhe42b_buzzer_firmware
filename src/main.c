#include <Arduino.h>
#include <notes.h>

// Pin definitions
#define powerPin 1      // PA2
#define buttonPin 2     // PA3
#define redLedPin 8     // PC6
#define buzzerPin 13    // PD4
#define signalPin 7     // PC5

// Constants
const int disarmDelay = 10;         // Seconds to apply power to disarm
const int shortPressDelay = 400;    // Milliseconds for short press
const int longPressDelay = 1000;    // Milliseconds for long press
const int debounceDelay = 50;       // Milliseconds for button debounce
const int alarmIntervalDefault = 30; // Default alarm interval in seconds

// State variables
bool armed = false;
bool disarmed = false;
bool powerOutage = false;
bool buttonPressed = false;
bool longPress = false;
int shortPressCount = 0;
int longPressCount = 0;
int lastButtonState = HIGH;
// int alarmInterval = alarmIntervalDefault;

unsigned long powerCutMillis = 0;
unsigned long pluggedMillis = 0;
unsigned long buttonPressMillis = 0;
unsigned long lastAlarmMillis = 0;
unsigned long lastDebounceTime = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(powerPin, INPUT);
  pinMode(signalPin, INPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void disarm() {
  disarmed = true;
  armed = false;
  powerCutMillis = 0;
  digitalWrite(redLedPin, LOW);
  play_disarming(buzzerPin);
}

void arm() {
  armed = true;
  digitalWrite(redLedPin, HIGH);
  play_arming(buzzerPin);
  shortPressCount = 0;
  longPressCount = 0;
}

void handlePowerState(int mainPowerState, unsigned long currentMillis) {
  if (mainPowerState == HIGH) { // When main power connected
    if (!armed && !disarmed) {
      pluggedMillis = millis();
      arm();
    }
    if (powerOutage) {
      pluggedMillis = millis();
      powerCutMillis = 0;
      powerOutage = false;
    }
    if (longPressCount >= 3 && disarmed) {
      longPressCount = 0;
      arm();
    }
  } else if (mainPowerState == LOW) { // When main power disconnected
    if (armed) {
      if (!powerOutage) {
        powerCutMillis = millis();
        powerOutage = true;
        lastAlarmMillis = powerCutMillis;
      }
      if ((currentMillis - lastAlarmMillis) / 1000 >= alarmIntervalDefault) {
        lastAlarmMillis = millis();
        play_alarm(buzzerPin);
      }
      if (powerOutage && (currentMillis - powerCutMillis) / 1000 > disarmDelay - 1 && 
          (currentMillis - powerCutMillis) / 1000 < disarmDelay + 1) {
        disarm();
      }
      if (longPressCount >= 3) {
        longPressCount = 0;
        disarm();
      }
    }
    if (disarmed) {
      armed = false;
      disarmed = false;
    }
  }
}

void handleSignalState(int signalState) {
  if (signalState == LOW) {
    play_alarm(buzzerPin);
  }
}

void handleButtonState(int buttonState, unsigned long currentMillis) {
  if (buttonState != lastButtonState) {
    lastDebounceTime = currentMillis;
  }
  
  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    if (buttonState == LOW) {
      if (!buttonPressed) {
        buttonPressed = true;
        buttonPressMillis = currentMillis;
        longPress = false;
      } else {
        long pressDuration = currentMillis - buttonPressMillis;
        if (pressDuration > shortPressDelay && pressDuration < longPressDelay) {
          shortPressCount++;
        } else if (pressDuration > longPressDelay) {
          longPressCount++;
          longPress = true;
          play_button(buzzerPin);
        }
      }
    } else {
      buttonPressed = false;
    }
  }
  lastButtonState = buttonState;
}

void loop() {
  unsigned long currentMillis = millis();
  int mainPowerState = digitalRead(powerPin);
  int buttonState = digitalRead(buttonPin);
  int signalState = digitalRead(signalPin);

  handlePowerState(mainPowerState, currentMillis);
  handleSignalState(signalState);
  handleButtonState(buttonState, currentMillis);
}