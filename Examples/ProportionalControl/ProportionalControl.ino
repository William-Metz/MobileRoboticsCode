#include "../../include/motorcontrol.h"
#include "../../include/wscommunicator.h"

//
// Global state
//

// Network configuration
const char* SSID = "Pomona";
const uint16_t PORT = 8181;
const unsigned long HEARTBEAT_INTERVAL = 1000;
WsCommunicator wsCommunicator(SSID, PORT, HEARTBEAT_INTERVAL);

// Robot characteristics
const float WHEEL_DIAMETER = 0.07;
const float WHEEL_CIRCUMFERENCE = WHEEL_DIAMETER * PI;

// Motor configuration
const float LEFT_GAIN = 0.5;
const float RIGHT_GAIN = 0.1;
const float MAX_VELOCITY_STEP = 0.5;
const float MAX_VELOCITY = 0.4;
const int MIN_PWM_PERCENT = 20;
const unsigned long MOTOR_CONTROL_INTERVAL = 100;
MotorControl motorControl(
    WHEEL_CIRCUMFERENCE, LEFT_GAIN, RIGHT_GAIN, MAX_VELOCITY_STEP, MAX_VELOCITY, MIN_PWM_PERCENT, MOTOR_CONTROL_INTERVAL
);

//
// Setup
//

void setup() {
  Serial.begin(115200);
  wsCommunicator.setup();
  motorControl.setup();
  motorControl.setTargetVelocity(0.1);
}

//
// Loop
//

void loop() {
  wsCommunicator.loopStep();
  motorControl.loopStep(wsCommunicator.isEnabled());
}
