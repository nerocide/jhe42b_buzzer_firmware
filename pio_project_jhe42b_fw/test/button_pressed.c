unsigned long keyPrevMillis = 0;
const unsigned long keySampleIntervalMs = 25;
byte longKeyPressCountMax = 80;    // 80 * 25 = 2000 ms
byte longKeyPressCount = 0;
byte prevKeyState = HIGH;         // button is active low
const byte keyPin = 2;            // button is connected to pin 2 and GND
// called when button is kept pressed for less than 2 seconds
void shortKeyPress() {
    // Serial.println("short");
}
// called when button is kept pressed for more than 2 seconds
void longKeyPress() {
    // Serial.println("long");
}
// called when key goes from not pressed to pressed
void keyPress() {
    // Serial.println("key press");
    longKeyPressCount = 0;
}
// called when key goes from pressed to not pressed
void keyRelease() {
    // Serial.println("key release");
   
    if (longKeyPressCount >= longKeyPressCountMax) {
        longKeyPress();
    }
    else {
        shortKeyPress();
    }
}
void setup() {
    Serial.begin(115200);
    pinMode(keyPin, INPUT_PULLUP);
}
void loop() {
    // key management section
    if (millis() - keyPrevMillis >= keySampleIntervalMs) {
        keyPrevMillis = millis();
       
        byte currKeyState = digitalRead(keyPin);
       
        if ((prevKeyState == HIGH) && (currKeyState == LOW)) {
            keyPress();
        }
        else if ((prevKeyState == LOW) && (currKeyState == HIGH)) {
            keyRelease();
        }
        else if (currKeyState == LOW) {
            longKeyPressCount++;
        }
       
        prevKeyState = currKeyState;
    }

    // other code goes here
}