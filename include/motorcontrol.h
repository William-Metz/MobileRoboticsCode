#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

/*
We are using the DRV8835 breakout board from Pololu:
https://www.pololu.com/product/2135

For each motor
- The PWM, analog output pin of each motor sets the speed (0 is stopped, 255 is full speed)
- The DIR, digital output pin of each motor sets the direction
*/

// TODO: address deprecation:
// https://docs.espressif.com/projects/esp-idf/en/stable/esp32/migration-guides/release-5.x/5.0/peripherals.html#pulse-counter-driver
#include <ESP32Encoder.h>

#include "intervaltimer.h"

const int LEFT_MOTOR_DIR_PIN = 8;
const int LEFT_MOTOR_PWM_PIN = 9;
const int RIGHT_MOTOR_DIR_PIN = 44;
const int RIGHT_MOTOR_PWM_PIN = 7;

const int LEFT_ENCODER_CLOCK_PIN = 3;
const int LEFT_ENCODER_DATA_PIN = 43;
const int RIGHT_ENCODER_CLOCK_PIN = 2;
const int RIGHT_ENCODER_DATA_PIN = 1;

// 7 pulse counts per rotation, 2x for quadrature encoding, 50:1 gear ratio
const long COUNTS_PER_ROTATION = 7 * 2 * 50;

// Map a value from [fromLo, fromHi] to [toLo, toHi]
float mapf(float value, float fromLo, float fromHi, float toLo, float toHi) {
  return (value - fromLo) * (toHi - toLo) / (fromHi - fromLo) + toLo;
}

// Check if a value is near zero
bool isNearZero(float value, float epsilon = 0.01) { return abs(value) < epsilon; }

typedef enum { DIRECTION_FORWARD = LOW, DIRECTION_BACKWARD = HIGH } MotorDirection;

//  ▄    ▄        ▀▀█                    ▄▄▄                  ▄                  ▀▀█
//  ▀▄  ▄▀  ▄▄▄     █                  ▄▀   ▀  ▄▄▄   ▄ ▄▄   ▄▄█▄▄   ▄ ▄▄   ▄▄▄     █
//   █  █  █▀  █    █                  █      █▀ ▀█  █▀  █    █     █▀  ▀ █▀ ▀█    █
//   ▀▄▄▀  █▀▀▀▀    █                  █      █   █  █   █    █     █     █   █    █
//    ██   ▀█▄▄▀    ▀▄▄    █            ▀▄▄▄▀ ▀█▄█▀  █   █    ▀▄▄   █     ▀█▄█▀    ▀▄▄

class ProportionalAccumulatorController {
 public:
  float velocityGain;
  float maxVelocityStep;
  float maxVelocity;
  float velocity;

 public:
  ProportionalAccumulatorController(float velocityGain, float maxVelocityStep, float maxVelocity)
      : velocityGain(velocityGain), maxVelocityStep(maxVelocityStep), maxVelocity(maxVelocity), velocity(0.0) {}

  float computeVelocity(float targetVelocity, float measuredVelocity) {
    float error = targetVelocity - measuredVelocity;

    // Constrain the control signal to limit the instantaneous change in velocity
    float velocityStep = constrain(velocityGain * error, -maxVelocityStep, maxVelocityStep);

    // Constrain velocity to its limits
    velocity = constrain(velocity + velocityStep, -maxVelocity, maxVelocity);

    return velocity;
  }
};

//  ▄▄▄▄▄▄                          █
//  █      ▄ ▄▄    ▄▄▄    ▄▄▄    ▄▄▄█   ▄▄▄    ▄ ▄▄
//  █▄▄▄▄▄ █▀  █  █▀  ▀  █▀ ▀█  █▀ ▀█  █▀  █   █▀  ▀
//  █      █   █  █      █   █  █   █  █▀▀▀▀   █
//  █▄▄▄▄▄ █   █  ▀█▄▄▀  ▀█▄█▀  ▀█▄██  ▀█▄▄▀   █

class Encoder {
  int dataPin;
  int clockPin;
  ESP32Encoder encoder;
  unsigned long lastTime;

  bool firstCheck = true;

 public:
  Encoder(int dataPin, int clockPin) : dataPin(dataPin), clockPin(clockPin) {}

  void setup() {
    encoder.attachHalfQuad(dataPin, clockPin);
    encoder.clearCount();
    lastTime = millis();
  }

  float getRotationsPerSecond() {
    if (firstCheck) {
      firstCheck = false;
      encoder.clearCount();
    }

    unsigned long now = millis();
    float elapsedTime = (now - lastTime) / 1000.0;
    lastTime = now;

    long pulseCount = encoder.getCount();
    encoder.clearCount();

    float rotation = (float)pulseCount / (float)COUNTS_PER_ROTATION;
    float rotationsPerSecond = rotation / (float)(elapsedTime + 0.0001);

    return rotationsPerSecond;
  }
};

//  ▄    ▄          ▄                         ▄▄▄▄            ▀
//  ██  ██  ▄▄▄   ▄▄█▄▄   ▄▄▄    ▄ ▄▄         █   ▀▄  ▄ ▄▄  ▄▄▄    ▄   ▄   ▄▄▄    ▄ ▄▄
//  █ ██ █ █▀ ▀█    █    █▀ ▀█   █▀  ▀        █    █  █▀  ▀   █    ▀▄ ▄▀  █▀  █   █▀  ▀
//  █ ▀▀ █ █   █    █    █   █   █            █    █  █       █     █▄█   █▀▀▀▀   █
//  █    █ ▀█▄█▀    ▀▄▄  ▀█▄█▀   █            █▄▄▄▀   █     ▄▄█▄▄    █    ▀█▄▄▀   █

class MotorDriver {
  int dirPin;
  int pwmPin;

 public:
  MotorDriver(int dirPin, int pwmPin) : dirPin(dirPin), pwmPin(pwmPin) {}

  void setup() {
    pinMode(dirPin, OUTPUT);
    setDirection(DIRECTION_FORWARD);
    stop();
  }

  void stop() { analogWrite(pwmPin, 0); }

  void setDirection(MotorDirection direction) { digitalWrite(dirPin, direction); }

  void setPwmPercent(long percent) {
    percent = constrain(percent, 0, 100);
    long dutyCycle = map(percent, 0, 100, 0, 255);
    analogWrite(pwmPin, dutyCycle);
  }
};

class DualMotorDriver {
  MotorDriver leftDriver;
  MotorDriver rightDriver;

 public:
  DualMotorDriver()
      : leftDriver(LEFT_MOTOR_DIR_PIN, LEFT_MOTOR_PWM_PIN), rightDriver(RIGHT_MOTOR_DIR_PIN, RIGHT_MOTOR_PWM_PIN) {}

  void setup() {
    leftDriver.setup();
    rightDriver.setup();
  }

  void stop() {
    leftDriver.stop();
    rightDriver.stop();
  }

  void setDirection(MotorDirection direction) { setDirection(direction, direction); }
  void setDirection(MotorDirection leftDirection, MotorDirection rightDirection) {
    leftDriver.setDirection(leftDirection);
    rightDriver.setDirection(rightDirection);
  }

  void setPwmPercent(long percent) { setPwmPercent(percent, percent); }
  void setPwmPercent(long leftPercent, long rightPercent) {
    leftDriver.setPwmPercent(leftPercent);
    rightDriver.setPwmPercent(rightPercent);
  }
};

//  ▄    ▄          ▄                           ▄▄▄                  ▄                  ▀▀█
//  ██  ██  ▄▄▄   ▄▄█▄▄   ▄▄▄    ▄ ▄▄         ▄▀   ▀  ▄▄▄   ▄ ▄▄   ▄▄█▄▄   ▄ ▄▄   ▄▄▄     █
//  █ ██ █ █▀ ▀█    █    █▀ ▀█   █▀  ▀        █      █▀ ▀█  █▀  █    █     █▀  ▀ █▀ ▀█    █
//  █ ▀▀ █ █   █    █    █   █   █            █      █   █  █   █    █     █     █   █    █
//  █    █ ▀█▄█▀    ▀▄▄  ▀█▄█▀   █             ▀▄▄▄▀ ▀█▄█▀  █   █    ▀▄▄   █     ▀█▄█▀    ▀▄▄

class MotorControl {
 private:
  // Encoder configuration
  Encoder leftEncoder;
  Encoder rightEncoder;

  // Speed at the left and right wheels
  float leftMeasuredVelocity;
  float rightMeasuredVelocity;

  // Control configuration
  float wheelCircumference;
  float leftTargetVelocity;
  float rightTargetVelocity;
  float maxVelocity;
  long minPwmPercent;
  float maxStep;
  ProportionalAccumulatorController leftController;
  ProportionalAccumulatorController rightController;
  DualMotorDriver motorDriver;

  IntervalTimer updateTimer;

 public:
  MotorControl(
      float wheelCircumference, float leftGain, float rightGain, float maxStep, float maxVelocity, long minPwmPercent,
      unsigned long interval
  )
      : leftEncoder(LEFT_ENCODER_DATA_PIN, LEFT_ENCODER_CLOCK_PIN)
      , rightEncoder(RIGHT_ENCODER_DATA_PIN, RIGHT_ENCODER_CLOCK_PIN)
      , leftMeasuredVelocity(0.0)
      , rightMeasuredVelocity(0.0)
      , wheelCircumference(wheelCircumference)
      , leftTargetVelocity(0.0)
      , rightTargetVelocity(0.0)
      , maxVelocity(maxVelocity)
      , minPwmPercent(minPwmPercent)
      , leftController(leftGain, maxStep, maxVelocity)
      , rightController(rightGain, maxStep, maxVelocity)
      , maxStep(maxStep)
      , motorDriver()
      , updateTimer(interval) {}

  void setup() {
    motorDriver.setup();
    leftEncoder.setup();
    rightEncoder.setup();
  }

  void loopStep(bool isEnabled) {
    // Stop the motors if not enabled
    if (!isEnabled) {
      stop();
    }

    if (updateTimer) {
      // Always measure the velocity (even if not updating motor control)
      leftMeasuredVelocity = leftEncoder.getRotationsPerSecond() * wheelCircumference;
      rightMeasuredVelocity = rightEncoder.getRotationsPerSecond() * wheelCircumference;

      // If not enabled, then the motors were stopped above
      if (isEnabled) {
        if (isNearZero(leftTargetVelocity) && isNearZero(rightTargetVelocity) && leftMeasuredVelocity < maxStep &&
            rightMeasuredVelocity < maxStep) {
          stop();
          return;
        }

        float leftControl = leftController.computeVelocity(leftTargetVelocity, leftMeasuredVelocity);
        long leftPwmPercent = mapf(abs(leftControl), 0, maxVelocity, minPwmPercent, 100);
        MotorDirection leftDirection = leftControl > 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD;

        float rightControl = rightController.computeVelocity(rightTargetVelocity, rightMeasuredVelocity);
        long rightPwmPercent = mapf(abs(rightControl), 0, maxVelocity, minPwmPercent, 100);
        MotorDirection rightDirection = rightControl > 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD;

        // Serial.printf(
        //     "%f, %f, %f, %ld, %f, %f, %ld\n", leftTargetVelocity, leftMeasuredVelocity, leftControl, leftPwmPercent,
        //     rightMeasuredVelocity, rightControl, rightPwmPercent
        // );

        motorDriver.setPwmPercent(leftPwmPercent, rightPwmPercent);
        motorDriver.setDirection(leftDirection, rightDirection);
      }
    }
  }

  void stop() { motorDriver.stop(); }

  void setTargetVelocity(float velocity) { setTargetVelocity(velocity, velocity); }
  void setTargetVelocity(float leftVelocity, float rightVelocity) {
    leftVelocity = constrain(leftVelocity, -maxVelocity, maxVelocity);
    rightVelocity = constrain(rightVelocity, -maxVelocity, maxVelocity);
    leftTargetVelocity = leftVelocity;
    rightTargetVelocity = rightVelocity;
  }

  float getLeftVelocity() { return leftMeasuredVelocity; }
  float getRightVelocity() { return rightMeasuredVelocity; }
};

#endif
