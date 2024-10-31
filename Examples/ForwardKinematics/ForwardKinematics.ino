

#include "../../include/motorcontrol.h"
#include "../../include/wscommunicator.h"
#include "../../include/display.h"
#include "../../include/forwardkinematics.h"

//
// Global state
//

// Network configuration
const char* SSID = "Pomona";
const uint16_t PORT = 8181;
const unsigned long HEARTBEAT_INTERVAL = 1000;
WsCommunicator wsCommunicator(SSID, PORT, HEARTBEAT_INTERVAL);



// ForwardKinematics config
const float trackWidth = 0.5; 
const unsigned long interval = 100; 
ForwardKinematics forwardKinematics(trackWidth, interval);


//Display
Display display;

// Motor configuration
const float WHEEL_DIAMETER = 0.07;
const float WHEEL_CIRCUMFERENCE = WHEEL_DIAMETER * PI;


const float LEFT_GAIN = 0.5;
const float RIGHT_GAIN = 0.5;
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
    display.setup();
    motorControl.setup();
    motorControl.setTargetVelocity(.2,.25);
    display.setup();
    display.drawString(0,0, wsCommunicator.getIpAddress());
    display.drawString(1, 4, String(wsCommunicator.getPort()));
    forwardKinematics.setup();
    forwardKinematics.setPose();
}

//
// Loop
//

void loop() {
    wsCommunicator.loopStep();
    motorControl.loopStep(wsCommunicator.isEnabled());
    forwardKinematics.loopStep(motorControl.getLeftVelocity(), motorControl.getRightVelocity());
  Pose currentPose = forwardKinematics.getPose();
  if (wsCommunicator.isEnabled()) {
    Serial.println("Pose: x=" + String(currentPose.x) + ", y=" + String(currentPose.y) + ", theta=" + String(currentPose.theta));
  }
  delay(250);


}
