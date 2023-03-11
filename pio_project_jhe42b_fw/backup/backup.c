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
const int REDLED_PIN=8;
// pin for led buzzer on board
const int LED_BUZZER_PIN=5;
// pin for buzzer on board
const int BUZZER_PIN=13;
// pin for button on board
const int BUTTON_PIN=2;
// pin for power detection on board
const int MAIN_POWER_PIN=1;
// other vars
int MAIN_POWER_STATE=0;
int BUTTON_STATE=255;

// button wars
bool buttonPressed=FALSE;
bool buttonReleased=FALSE;
unsigned long buttonTimerPressed=0;
unsigned long buttonTimerReleased=0;
// power vars
bool powerPlugged=FALSE;
bool powerUnPluggled=FALSE;
unsigned long powerTimerPlugged=0;
unsigned long powerTimerUnPlugged=0;
// 
bool armed=FALSE;
bool disarmed=TRUE;
// config
int secondSinceUnplugged = 0;
int secondToDisarm = 5;
int secondBeforeAlarm=10;
int alarmInterval=10;

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
  // when the 5V power supply is plugged in - HIGH == pluggled in - LOW == unplugged
  MAIN_POWER_STATE = digitalRead(MAIN_POWER_PIN);
  // When press button is pressed - // HIGH == button released - // LOW == button pressed
  BUTTON_STATE = digitalRead(BUTTON_PIN);
  unsigned long currentMillis=millis();
  
  // when power is plugged
  if (MAIN_POWER_STATE == HIGH) {
    digitalWrite(REDLED_PIN, HIGH);
    // arming circuit
    if ((powerPlugged == FALSE) && (powerUnPluggled == FALSE) && (armed==FALSE) && (disarmed==TRUE)){
      powerPlugged=TRUE;
      armed = TRUE;
      disarmed=FALSE;
      arming(BUZZER_PIN);
    }
    // store time when plug while armed
    if((powerPlugged == FALSE) && (powerUnPluggled == TRUE) && (armed==TRUE) && (disarmed==FALSE)){
        powerTimerPlugged=millis();
    }
  }
  else {
    // turn onboard red led on
    digitalWrite(REDLED_PIN, LOW);
    // store time when circuit is unplugged
    if (( powerPlugged == TRUE ) && (armed==TRUE) && (disarmed==FALSE)){
      powerUnPluggled = TRUE;
      powerPlugged = FALSE;
      powerTimerUnPlugged = millis();
    }
    // calculate elapsed time since unplug
    if ((powerPlugged == FALSE) && (powerUnPluggled == TRUE) && (armed==TRUE) && (disarmed==FALSE)){
      // powerUnPluggled = FALSE;
      int secondSinceUnplugged = round((millis() - powerTimerUnPlugged)/1000);
      // play alarm
      if ( secondSinceUnplugged >= secondBeforeAlarm ){
        alarm(BUZZER_PIN);
      }
    }
    // calculate elapsed time since unplug
    if((powerPlugged == FALSE) && (powerUnPluggled == TRUE) && (armed==TRUE) && (disarmed==FALSE)){
       int secondSinceUnplugged = round((millis() - powerTimerPlugged)/1000);
      // disarm
      if ((secondSinceUnplugged > secondToDisarm-1) && (secondSinceUnplugged < secondToDisarm+1)){
        disarmed=TRUE;
        armed=FALSE;
        powerUnPluggled = FALSE;
        disarming(BUZZER_PIN);
      }
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


