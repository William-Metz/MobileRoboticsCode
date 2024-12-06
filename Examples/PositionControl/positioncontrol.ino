#include <WString.h>
#include "../../include/motorcontrol.h"
#include "../../include/wscommunicator.h"
#include "../../include/display.h"
#include "../../include/positioncontrol.h"
#include "../../include/forwardkinematics.h" //I added to include folder

// Physical Characteristics of the Robot
const float TRACK_WIDTH = 0.5; //ForwardKinematics
const float WHEEL_DIAMETER = 0.07; //MotorControl
const float WHEEL_CIRCUMFERENCE = WHEEL_DIAMETER * PI; //MotorControl

//
// Global state
//

// Network configuration
const char* SSID = "Pomona";
const uint16_t PORT = 8181;
const unsigned long HEARTBEAT_INTERVAL = 1000;
char message[100];
WsCommunicator wsCommunicator(SSID, PORT, HEARTBEAT_INTERVAL);

const unsigned long MESSAGE_INTERVAL = 1000;
IntervalTimer messageTimer(MESSAGE_INTERVAL);



// ForwardKinematics config

const unsigned long FORWARD_KINEMATICS_INTERVAL = 250; 
ForwardKinematics forwardKinematics(TRACK_WIDTH, FORWARD_KINEMATICS_INTERVAL);

//Display
Display display;

// Motor configuration

const float LEFT_GAIN = 0.5;
const float RIGHT_GAIN = 0.5;
const float MAX_VELOCITY_STEP = 0.5;
const float MAX_LINEAR_VELOCITY = 0.4; //updated name
const int MIN_PWM_PERCENT = 20;
const unsigned long MOTOR_CONTROL_INTERVAL = 100;
MotorControl motorControl(
        WHEEL_CIRCUMFERENCE, LEFT_GAIN, RIGHT_GAIN, MAX_VELOCITY_STEP, MAX_LINEAR_VELOCITY, MIN_PWM_PERCENT, MOTOR_CONTROL_INTERVAL
        );


// Position Control Confgiuration

const float GOAL_X = 1.0;
const float GOAL_Y = 1.0;
const float GOAL_THRESHOLD = 0.1;
const float MAX_ANGULAR_VELOCITY = 1.0 //how many radians per second can robot rotate. can tweak
const float K_POSITION = 1.0; //can tweak
const float K_ORIENTATION = 2.0; //can tweak
const unsigned long POSITION_CONTROL_INTERVAL = 250;

PositionControl positionControl(GOAL_X, GOAL_Y, GOAL_THRESHOLD, MAX_LINEAR_VELOCITY, MAX_ANGULAR_VELOCITY, K_POSITION, K_ORIENTATION, TRACK_WIDTH, POSITION_CONTROL_INTERVAL);

//
// Setup
//

void setup() {
    Serial.begin(115200);
    wsCommunicator.setup();
    motorControl.setup();
    display.setup();
    display.drawString(0,0, "Position Control"); // added
    display.drawString(0,0, wsCommunicator.getIpAddress());
    display.drawString(2, 0, String(wsCommunicator.getPort()));
    forwardKinematics.setup();
    forwardKinematics.setPose();

    positionControl.setup();
}

//
// Reset (used for the web interface he created)
//
void reset(){
    forwardKinematics.setPose(0, 0, 0);
}

//
// Loop
//

void loop() {
    // update WS interface
    wsCommunicator.loopStep();

    // allow us to send a reset message to reset the robot back to origin in the web interface
    if (wsCommunicator.resetFlagIsSet()) {
        reset();
        wsCommunicator.clearResetFlag();
    }

    // update display
    display.loopStep(); //Doesn't do anything

    // update motor control (need heartbeat to be enabled)
    motorControl.loopStep(wsCommunicator.isEnabled());

    float leftVelocity = motorControl.getLeftVelocity();
    float rightVelocity = motorControl.getRightVelocity();
    // update forward kinematics
    forwardKinematics.loopStep(leftVelocity, rightVelocity);

    //Update position control 
    Pose currentPose = forwardKinematics.getPose();
    bool shouldUpdateVelocities = positionControl.loopStep(pose, leftVelocity, rightVelocity); //should we update velocity or no

    // if we are at max speed, we don't update the velocities
    if (shouldUpdateVelocities) {
        motorControl.setTargetVelocity(leftVelocity, rightVelocity);
    }

    // allow messages to be sent
    if (messageTimer) {
        //format message. vl and fr are current desired velocities, not the actual ones since we might not change them in positionControl
        snprintf(message, sizeof(message), "Pose: x=%f, y=%f, theta=%f, vl=%f, vr=%f", currentPose.x, currentPose.y, currentPose.theta, motorControl.getLeftVelocity(), motorControl.getRightVelocity());   
        wsCommunicator.sendText(message, strlen(message));
    }
}
