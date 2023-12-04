const int ButtonPin = 13;
const int LedPin[] = {2, 3, 4, 5, 6, 7, 8, 9, 10};  // Array of LED pins
int CurrentDay = 0;                                 // Counter to keep track of the current day
int debounceDuration = 120;                         // Time duration for button debounce
bool isON = false;
unsigned long timeON = 0;  // Record the time when the button is pressed

unsigned long previousMillis = 0;
const long flameInterval = 20;  // Adjust this value for the flame flicker effect

bool ledStates[] = {false, false, false, false, false, false, false, false, false, false}; // Keeps track of LED states

void setup() {
  for (int i = 0; i < 10; i++) {
    pinMode(LedPin[i], OUTPUT);
  }
  pinMode(ButtonPin, INPUT);
}

void loop() {
  if (digitalRead(ButtonPin)) {
    CurrentDay++;
    if (CurrentDay == 10) {
      CurrentDay = 0;
      isON = false;
      for (int i = 0; i < 10; i++) {
        ledStates[i] = false; // Reset LED states when turning off
      }
    } else {
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

  if (isON && (millis() - timeON > 2100000)) {
    isON = false;
    CurrentDay = 0;
    for (int i = 0; i < 10; i++) {
      ledStates[i] = false; // Reset LED states when turning off
    }
    turnLED(CurrentDay);
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
    } else {
      digitalWrite(LedPin[i], LOW);
    }
  }
}

void flickerFlame() {
  for (int i = 0; i < 10; i++) {
    if (ledStates[i]) {
      int randomBrightness = random(50, 256);  // Random brightness level
      analogWrite(LedPin[i], randomBrightness);
    }
  }
}