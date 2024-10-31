
// Robot_Class.h

#ifndef ROBOT_CLASS_H
#define ROBOT_CLASS_H

#include <Arduino.h> 
#include <math.h>     

class Robot_Class {
public:
    // Global variables
    float x_G;
    float y_G;
    float theta_G;

    // Robot variables
    float x_dot_R;
    float y_dot_R;
    float theta_dot_R;

    // Constant parameters
    const float r;
    const float d;

    // Constructor
    Robot_Class()
        : x_G(0.0f), y_G(0.0f), theta_G(0.0f),
          x_dot_R(0.0f), y_dot_R(0.0f), theta_dot_R(0.0f),
          r(7.0f), d(20.0f) {}

    void update_state(float dt, float phi_dot_L, float phi_dot_R) {
        // Compute robot's local velocities
        x_dot_R = (r / 2.0f) * (phi_dot_L + phi_dot_R);
        y_dot_R = 0.0f;
        theta_dot_R = (r / d) * (phi_dot_R - phi_dot_L);

        // Compute robot's global velocities
        float x_dot_G = x_dot_R * cos(theta_G) - y_dot_R * sin(theta_G);
        float y_dot_G = x_dot_R * sin(theta_G) + y_dot_R * cos(theta_G);
        float theta_dot_G = theta_dot_R;  

        // Update global state variables
        x_G += x_dot_G * dt;
        y_G += y_dot_G * dt;
        theta_G += theta_dot_G * dt;
    }
};

#endif // ROBOT_CLASS_H
