#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "intervaltimer.h"

struct Pose {
  float x;
  float y;
  float theta;
  Pose(float x, float y, float theta) : x(x), y(y), theta(theta) {}
  void set(float newX, float newY, float newTheta) {
    x = newX;
    y = newY;
    theta = newTheta;
  }
};

class ForwardKinematics {
  float trackWidth;
  Pose pose;

  IntervalTimer updateTimer;

  void update(float leftVelocity, float rightVelocity, float dt) {
    float v = (rightVelocity + leftVelocity) / 2;
    float thetaDot = (rightVelocity - leftVelocity) / trackWidth;

    float xDot = v * cos(pose.theta);
    float yDot = v * sin(pose.theta);

    pose.x += xDot * dt;
    pose.y += yDot * dt;
    pose.theta += thetaDot * dt;
  }

 public:
  ForwardKinematics(float trackWidth, unsigned long interval)
      : trackWidth(trackWidth), pose(0, 0, 0), updateTimer(interval) {}

  void setup() {}

  void loopStep(float leftVelocity, float rightVelocity) {
    if (updateTimer) {
      update(leftVelocity, rightVelocity, updateTimer.getLastDelta() / 1000.0);
    }
  }

  void setPose(float newX = 0, float newY = 0, float newTheta = 0) { pose.set(newX, newY, newTheta); }

  Pose getPose() { return pose; }
};

#endif
