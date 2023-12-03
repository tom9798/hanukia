#include "LowPower.h"
const int ButtonPin = 13;
const int wake = 2; //Interrupt pin for waking up from stdby mode (MUST BE THIS ONE OR PIN 3!! current physical configuration - pin 2)
const int LedPin[] = {3, 4, 5, 6, 7, 8, 9, 10, 11};  // Array of LED pins
int CurrentDay = 0;                                 // Counter to keep track of the current day
int debounceDuration = 120;                         // Time duration for button debounce
bool isON = false;
unsigned long timeON = 0;  // Record the time when the button is pressed

unsigned long previousMillis = 0;
const long flameInterval = 120;  // Adjust this value for the flame flicker effect

bool ledStates[] = {false, false, false, false, false, false, false, false, false, false}; // Keeps track of LED states
bool allOff = true;

void setup() {
  for (int i = 0; i < 10; i++) {
    pinMode(LedPin[i], OUTPUT);
  }
  pinMode(ButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(wake), wakeUp, CHANGE); // this pin is connected to the button pin, which will wake up the arduino from power saving
  Serial.begin(9600);
}

void wakeUp(){
  // just a function to go to when waking up, no functionality needed
  // technically possible to add something nice when starting, such as lighting the "shamash", up to you
}

void loop() {
  // Serial.print("working\n"); //just for debugging, can check if the for loop stopped = stdby mode
  if (digitalRead(ButtonPin)) {
    CurrentDay++;
    if (CurrentDay == 10) {
      CurrentDay = 0;
      isON = false;
      for (int i = 0; i < 10; i++) {
        ledStates[i] = false; // Reset LED states when turning off
      }
      allOff = true;
    } else {
      allOff = false;
      isON = true;
      timeON = millis();
      ledStates[CurrentDay - 1] = true; // Mark the LED that should be on
    }

    turnLED(CurrentDay);

    unsigned long lastTimeStateChanged = millis();

    while (millis() - lastTimeStateChanged < debounceDuration) {
      if (digitalRead(ButtonPin)) {
        lastTimeStateChanged = millis();
      }
    }
  }

    if (allOff && (millis() - timeON > 5000)) {
    isON = false;
    CurrentDay = 0;
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); // set arduino to power saving mode
  }

  if (isON && (millis() - timeON > 2100000)) {
    isON = false;
    CurrentDay = 0;
    for (int i = 0; i < 10; i++) {
      ledStates[i] = false; // Reset LED states when turning off (if we comment out this for loop, we can set it to remember how many candles were lit instead of lighting tem all each time)
    } 
    turnLED(CurrentDay);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); // set arduino to power saving mode
  }

  // Add the flame flicker effect
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= flameInterval) {
    previousMillis = currentMillis;
    flickerFlame();
  }
}

void turnLED(int day) {
  for (int i = 0; i < 10; i++) {
    if (i < day && ledStates[i]) {
      digitalWrite(LedPin[i], HIGH);
      // Serial.print(i);
    } else {
      digitalWrite(LedPin[i], LOW);
    }
  }
}

void flickerFlame() {
  for (int i = 0; i < 10; i++) {
    if (ledStates[i]) {
      int randomBrightness = random(150, 256);  // Random brightness level
      analogWrite(LedPin[i], randomBrightness);
    }
  }
}