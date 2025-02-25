#include "pid.h"
#include <Arduino.h>

static const int minThrottle = 100;
static const int maxThrottle = 600;

float kp = 1.5, ki = 0.001, kd = 0.05;  // Tune these values
float roll, pitch;                   // Assume these are updated from IMU
float rollError, pitchError;
float rollIntegral = 0, pitchIntegral = 0;
float rollPrevError = 0, pitchPrevError = 0;
static unsigned int time;

int motorFL;
int motorFR;
int motorBL;
int motorBR;


static float euler_angles[3];

int get_speed(int motor) {
  if (motor == FR) return motorFR;
  if (motor == BR) return motorBR;
  if (motor == FL) return motorFL;
  if (motor == BL) return motorBL;
  else {
    Serial.println("error invalid motor number");
  }
  return 0;
}

void update_pid(int baseThrottle, float roll, float pitch) {
  rollError = 0 - roll;
  pitchError = 0 - pitch;

  float dt = millis() - time;
  dt = dt / 1000;

  // PID Computation
  rollIntegral += rollError * dt;
  pitchIntegral += pitchError * dt;
  pitchIntegral = constrain(pitchIntegral, -200, 200);
  rollIntegral = constrain(rollIntegral, -200, 200);

  float rollDerivative = (rollError - rollPrevError) / dt;
  float pitchDerivative = (pitchError - pitchPrevError) / dt;

  float rollCorrection = kp * rollError + ki * rollIntegral + kd * rollDerivative;
  float pitchCorrection = kp * pitchError + ki * pitchIntegral + kd * pitchDerivative;

  rollPrevError = rollError;
  pitchPrevError = pitchError;

  motorFL = constrain(baseThrottle - rollCorrection + pitchCorrection, minThrottle, maxThrottle);
  motorFR = constrain(baseThrottle + rollCorrection + pitchCorrection, minThrottle, maxThrottle);
  motorBL = constrain(baseThrottle - rollCorrection + pitchCorrection, minThrottle, maxThrottle);
  motorBR = constrain(baseThrottle + rollCorrection - pitchCorrection, minThrottle, maxThrottle);
}