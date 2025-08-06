/*
  Continuous_I2C_Mux_DRV2650L.cpp
	
	Components:
		- DRV2605 haptic driver
    - PCA9548 I2C MUX
		- ESP32-C3-DevKitM-1
		- LRA haptic motor

  This code:
    - Implements the MUX
    - Runs continuously
    - Waveform sequence every 750 ms

    
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv[2];

#define LED_PIN 8
#define SDA_PIN 6
#define SCL_PIN 7
#define MUX_ADDRESS 0x70

void selectMuxChannel(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void blinkLED(unsigned long period_ms) {
  static unsigned long lastToggle = 0;
  static bool ledState = false;
  unsigned long now = millis();
  if (now - lastToggle >= period_ms / 2) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    lastToggle = now;
  }
}

void setup() {
  blinkLED(1000); // Slow blink to indicate setup

  Serial.begin(115200);
  delay(5000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED ON to indicate running

  Wire.begin(SDA_PIN, SCL_PIN);

  for (uint8_t i = 0; i < 2; i++) {
    selectMuxChannel(i);
    if (!drv[i].begin(&Wire)) {
      Serial.print("DRV2605 on channel ");
      Serial.print(i);
      Serial.println(" failed to initialize");
      while (1);
    }
    Serial.print("Initialized DRV2605 on channel: ");
    Serial.println(i);

    drv[i].useLRA();
    drv[i].selectLibrary(6);
    drv[i].setMode(DRV2605_MODE_INTTRIG);
    drv[i].setWaveform(0, 47);   // Buzz
    drv[i].setWaveform(1, 24);   // Tick
    drv[i].setWaveform(2, 0);    // End
  }
}

/*
  //Blocking loop
  void loop() {
  blinkLED(400); // Fast blink (400ms period)
  selectMuxChannel(0);
  drv[0].go();
  selectMuxChannel(1);
  drv[1].go();
  delay(750);
} */

// Non-blocking loop
void loop() {
  blinkLED(400); // Fast blink (400ms period)

  static unsigned long lastTrigger = 0;
  unsigned long now = millis();
  if (now - lastTrigger >= 750) {
    lastTrigger = now;
    selectMuxChannel(0);
    drv[0].go();
    selectMuxChannel(1);
    drv[1].go();
  }
}