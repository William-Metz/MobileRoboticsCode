
#include "../../include/robot.h"

Robot_Class robot;  

void setup() {
    Serial.begin(11520);

}

void loop() {
    // Time step (dt) and wheel angular velocities (phi_dot_L and phi_dot_R)
    float dt = 0.1f;            // Time step in seconds
    float phi_dot_L = 1.0f;     // Left wheel angular velocity
    float phi_dot_R = 1.0f;     // Right wheel angular velocity

    // Update the robot's state
    robot.update_state(dt, phi_dot_L, phi_dot_R);

    // Delay to control the update rate
    delay(100);  // Delay for 100 milliseconds
}

