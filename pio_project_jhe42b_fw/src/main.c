#include <Arduino.h>
#include <notes.h>
// PA1, /* 0 */
// PA2,
// PA3,
// PB5, /* 3 */
// PB4,
// PC3, /* 5 */
// PC4,
// PC5,
// PC6,
// PC7,
// PD1, /* 10 */
// PD2,
// PD3,
// PD4,
// PD5,
// PD6  /* 15 */

// pin for red led on board
int REDLED_PIN=8;
// pin for led buzzer on board
int LED_BUZZER_PIN=5;
// pin for buzzer on board
int BUZZER_PIN=13;
// pin for button on board
int BUTTON_PIN=2;
// pin for power detection on board
int MAIN_POWER_PIN=1;
// other vars
int MAIN_POWER_STATE=0;
int BUTTON_STATE=255;

bool buttonPressed=FALSE;
bool buttonReleased=FALSE;
long buttonTimerPressed=0;
long buttonTimerReleased=0;

bool powerPlugged=FALSE;
bool powerUnPluggled=FALSE;
long powerTimerPlugged=0;
long powerTimerUnPlugged=0;


// to be dev
// int TESTPIN=1;

void setup()  {
  // initialize INPUTS pins
  pinMode(BUTTON_PIN,INPUT);
  pinMode(MAIN_POWER_PIN,INPUT);
  // initialize OUTPUT pins
  pinMode(REDLED_PIN,OUTPUT);
  // pinMode(LED_BUZZER_PIN,OUTPUT); // not used at the end...
  pinMode(BUZZER_PIN,OUTPUT);
  // test pin
  // pinMode(TESTPIN,INPUT);
  // pinMode(TESTPIN,OUTPUT);
}

// the loop routine runs over and over again forever:
void loop()  {
  // when the 5V power supply is plugged in
  // HIGH == pluggled in
  // LOW == unplugged
  MAIN_POWER_STATE = digitalRead(MAIN_POWER_PIN);
  // When press button is pressed
  // HIGH == button released
  // LOW == button pressed
  BUTTON_STATE = digitalRead(BUTTON_PIN);

  if (MAIN_POWER_STATE == HIGH) {
    digitalWrite(REDLED_PIN, HIGH);
    if (( powerPlugged == FALSE) && (powerUnPluggled == FALSE ))
    {
      powerPlugged=TRUE;
      powerTimerPlugged=millis();
    }
  } 
  else {
    digitalWrite(REDLED_PIN, LOW);
    if ( powerPlugged == TRUE )
    {
      powerUnPluggled = TRUE;
      powerPlugged = FALSE;
      powerTimerUnPlugged = millis();
    }
    if ((powerUnPluggled == TRUE ) && ( powerPlugged == FALSE))
    {
      powerUnPluggled = FALSE;
      long duration = powerTimerUnPlugged - powerTimerPlugged;
      int blink_count = round(duration / 1000);
      blink_led(REDLED_PIN,blink_count);
    }
  }

  if (BUTTON_STATE == LOW) {
    //  blink_led(REDLED_PIN,2);
    //  arming(BUZZER_PIN);

    if (( buttonPressed == FALSE) && (buttonReleased == FALSE ))
    {
      buttonPressed=TRUE;
      buttonTimerPressed=millis();
    }
  }
  else{
    if ( buttonPressed == TRUE )
    {
      buttonReleased = TRUE;
      buttonPressed = FALSE;
      buttonTimerReleased = millis();
    }
    if ((buttonReleased == TRUE ) && ( buttonPressed == FALSE))
    {
      buttonReleased = FALSE;
      long duration = buttonTimerReleased - buttonTimerPressed;
      int blink_count = round(duration / 1000);
      blink_led(REDLED_PIN,blink_count);
    }    
  }
}


