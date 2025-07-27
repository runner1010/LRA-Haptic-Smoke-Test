/*
	LRA Haptic Smoke Test: Method to test the feel of a variety of haptic motors with the alarm sequence

	Change code to use either ERM or LRA motors by commenting out the appropriate lines in setup().
	
	Components:
		- DRV2605 haptic driver
		- ESP32-C3-DevKitM-1
		- ERM or LRA haptic motor
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv;

#define LED_PIN 8
#define SDA_PIN 6
#define SCL_PIN 7

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

// Setup function
void setup() {
  Serial.begin(115200);
  delay(5000);	 // Wait for serial monitor to open

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);

  while (!drv.begin(&Wire)) {
    Serial.println("DRV2605 init failed — retrying...");
    digitalWrite(LED_PIN, LOW);
    delay(1000);
  }

  Serial.println("DRV2605 initialized");


  // Configure DRV2605 for ERM
  drv.useERM();	 // Use ERM for this example
  drv.selectLibrary(1); // Use library 1-5 for ERM

/*   // Configure DRV2605 for LRA
  drv.useLRA(); // Use LRA for this example
  drv.selectLibrary(6); // Select library 6 for LRA */

  drv.setMode(DRV2605_MODE_INTTRIG);	// Set to internal trigger mode

  // Alarm waveform setup
  drv.setWaveform(0, 47);   // Buzz
  drv.setWaveform(1, 24);   // Tick
  drv.setWaveform(2, 0);    // End

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
      drv.go();
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
