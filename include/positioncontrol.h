#ifndef POSITION_CONTROL_H
#define POSITION_CONTROL_H

#include "forwardkinematics.h"
#include "intervaltimer.h"

class PositionControl {
  float goalX;
  float goalY;
  float goalThreshold;

  float maxVelocityLinear;
  float maxVelocityAngular;

  float gainPosition;
  float gainRotation;

  float trackWidth;

  IntervalTimer updateTimer;

 public:
  PositionControl(
      float goalX, float goalY, float goalThreshold, float maxVelocityLinear, float maxVelocityAngular,
      float gainPosition, float gainRotation, float trackWidth, unsigned long interval
  )
      : goalX(goalX)
      , goalY(goalY)
      , goalThreshold(goalThreshold)
      , maxVelocityLinear(maxVelocityLinear)
      , maxVelocityAngular(maxVelocityAngular)
      , gainPosition(gainPosition)
      , gainRotation(gainRotation)
      , trackWidth(trackWidth)
      , updateTimer(interval) {}

  void setup() {}

  bool loopStep(Pose pose, float& leftVelocity, float& rightVelocity) {
    if (!updateTimer) {
      return false;
    }

    // Compute the distance to the goal
    float dx = goalX - pose.x;
    float dy = goalY - pose.y;
    float d = sqrt((dx * dx) + (dy * dy));

    // Check the distance to the goal
    if (d < goalThreshold) {
      leftVelocity = 0;
      rightVelocity = 0;
      return true;
    }

    // Proportional control for linear velocity (d is a positive value)
    float v = min(gainPosition * d, maxVelocityLinear);

    // Proportional control for angular velocity
    float angleToGoal = atan2(dy, dx);
    float angleError = angleToGoal - pose.theta;
    float thetaDot = constrain(gainRotation * angleError, -maxVelocityAngular, maxVelocityAngular);

    // Inverse kinematics (set motor commands based on desired velocities)
    leftVelocity = v - thetaDot * trackWidth / 2.0;
    rightVelocity = v + thetaDot * trackWidth / 2.0;

    return true;
  }
};

#endif
