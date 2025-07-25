#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv;

const uint8_t patterns[3] = {1, 47, 87}; // Example waveform IDs
const uint8_t defaultPattern = 47;       // Default waveform ID
const unsigned long timeoutMs = 30000;   // 30 seconds

void prompt() {
  Serial.println("Choose a haptic pattern:");
  Serial.println("1: Pattern 1");
  Serial.println("2: Pattern 2");
  Serial.println("3: Pattern 3");
  Serial.println("Enter 1, 2, or 3:");
}

void playPattern(uint8_t waveform) {
  drv.setWaveform(0, waveform); // Set effect
  drv.setWaveform(1, 0);        // End of sequence
  drv.go();
}

void setup() {
  Serial.begin(115200);
  drv.begin();
  drv.selectLibrary(1); // LRA
  drv.setMode(DRV2605_MODE_INTTRIG);
  prompt();
}

void loop() {
  static unsigned long lastInputTime = millis();
  static bool runningPattern = false;
  static uint8_t currentPattern = 0xFF;

  // Check for user input
  if (Serial.available()) {
    char c = Serial.read();
    if (c >= '1' && c <= '3') {
      currentPattern = patterns[c - '1'];
      runningPattern = true;
      lastInputTime = millis();
      Serial.print("Running pattern ");
      Serial.println(c);
    } else {
      Serial.println("Invalid input. Please enter 1, 2, or 3.");
      prompt();
      lastInputTime = millis();
      runningPattern = false;
    }
    // Clear any extra input
    while (Serial.available()) Serial.read();
  }

  // Timeout: run default pattern
  if (!runningPattern && millis() - lastInputTime > timeoutMs) {
    currentPattern = defaultPattern;
    runningPattern = true;
    Serial.println("No input detected. Running default pattern.");
  }

  // Play the current pattern in a loop
  if (runningPattern) {
    playPattern(currentPattern);
    delay(500); // Adjust for effect duration
    // Check for interruption
    if (Serial.available()) {
      runningPattern = false;
      prompt();
      // Clear input buffer
      while (Serial.available()) Serial.read();
      lastInputTime = millis();
    }
  } else {
    delay(50); // Idle wait
  }
}