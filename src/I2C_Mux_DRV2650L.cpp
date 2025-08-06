/*
  I2C_Mux_DRV2650L.cpp

	Components:
		- DRV2605 haptic driver
    - PCA9548 I2C MUX
		- ESP32-C3-DevKitM-1
		- LRA haptic motor

  This code:
    - Only uses a single DRV2605 (no MUX)
    - Runs 30s active and 30s inactive periods
    - Waveform sequence every TBD ms
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv[2];

#define LED_PIN 8
#define SDA_PIN 6
#define SCL_PIN 7

#define MUX_ADDRESS 0x70

const unsigned long ACTIVE_TIME = 30000;	// Active phase duration in milliseconds
const unsigned long INACTIVE_TIME = 30000; // Inactive phase duration in milliseconds
const unsigned long GO_INTERVAL = 1000;    // Interval between GO commands in milliseconds

const unsigned long ACTIVE_LED_ON_TIME = 250;	// LED ON time during active phase
const unsigned long ACTIVE_LED_OFF_TIME = 250;	// LED OFF time during active phase
const unsigned long INACTIVE_LED_ON_TIME = 2000;	// LED ON time during inactive phase
const unsigned long INACTIVE_LED_OFF_TIME = 2000;	// LED OFF time during inactive phase

unsigned long phaseStartTime = 0;	// Start time of the current phase
unsigned long activeStartTime = 0;	// Start time of the active phase
unsigned long lastGoTime = 0;	// Last time the GO command was sent
unsigned long lastLedToggle = 0;	// Last time the LED state was toggled

bool activePhase = true;	// Flag to indicate if we are in the active phase
bool ledOnState = false;	// Current state of the LED

// Function to select MUX channel
void selectMuxChannel(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(1 << channel); // Select channel by writing bitmask
  Wire.endTransmission();
}

// Setup function
void setup() {
  Serial.begin(115200);
  delay(5000);	 // Wait for serial monitor to open

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

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

  // Configure DRV2605 for LRA
  drv[i].useLRA(); // Use LRA for this example
  drv[i].selectLibrary(6); // Select library 6 for LRA

  drv[i].setMode(DRV2605_MODE_INTTRIG);	// Set to internal trigger mode

  // Alarm waveform setup
  drv[i].setWaveform(0, 47);   // Buzz
  drv[i].setWaveform(1, 24);   // Tick
  drv[i].setWaveform(2, 0);    // End
  }

  // Start the timers
  phaseStartTime = millis();
  activeStartTime = millis();
  lastGoTime = 0;
  lastLedToggle = millis();
  digitalWrite(LED_PIN, LOW);
}

// Main Loop
void loop() {
  // Get the current time
  unsigned long now = millis();

  // Active phase logic 
  if (activePhase) {
    // Trigger vibration every second
    if (now - lastGoTime >= GO_INTERVAL) {
      lastGoTime = now;

      // Trigger vibration on both motors
      for (uint8_t i = 0; i < 2; i++) {
        selectMuxChannel(i);
        drv[i].go();
        Serial.print("Triggering motor on MUX channel: ");
        Serial.println(i);
      }
    }

    // LED blink: 250ms ON / 250ms OFF
    unsigned long ledInterval = ledOnState ? ACTIVE_LED_ON_TIME : ACTIVE_LED_OFF_TIME;
    if (now - lastLedToggle >= ledInterval) {
      ledOnState = !ledOnState;
      digitalWrite(LED_PIN, ledOnState ? HIGH : LOW);
      lastLedToggle = now;
    }

    // End active phase
    if (now - phaseStartTime >= ACTIVE_TIME) {
      unsigned long actualDuration = now - activeStartTime;
      Serial.print("Switching to INACTIVE phase — Active phase lasted ");
      Serial.print(actualDuration);
      Serial.println(" ms");

      activePhase = false;
      phaseStartTime = now;
      lastLedToggle = now;
      ledOnState = false;
      digitalWrite(LED_PIN, LOW);
    }

  }
  // Inactive phase logic
  else {
    // LED blink: 2000ms ON / 2000ms OFF
    unsigned long ledInterval = ledOnState ? INACTIVE_LED_ON_TIME : INACTIVE_LED_OFF_TIME;
    if (now - lastLedToggle >= ledInterval) {
      ledOnState = !ledOnState;
      digitalWrite(LED_PIN, ledOnState ? HIGH : LOW);
      lastLedToggle = now;
    }

    // End inactive phase
    if (now - phaseStartTime >= INACTIVE_TIME) {
      Serial.println("Switching to ACTIVE phase");
      activePhase = true;
      phaseStartTime = now;
      activeStartTime = now;
      lastGoTime = 0;
      lastLedToggle = now;
      ledOnState = false;
      digitalWrite(LED_PIN, LOW);
    }
  }
}
