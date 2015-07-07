/*
 * drive.h - Class that encapsulates motor control
 * Copyright (C) 2015 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef DRIVE_H
#define DRIVE_H

#include "log.h"

class Drive {
  Adafruit_MotorShield m_motorShield;
  Adafruit_DCMotor *m_frontLeftMotor, *m_backLeftMotor, *m_frontRightMotor, *m_backRightMotor;

public:
  Drive() :
    m_frontLeftMotor(m_motorShield.getMotor(1)),
    m_backLeftMotor(m_motorShield.getMotor(2)),
    m_frontRightMotor(m_motorShield.getMotor(3)),
    m_backRightMotor(m_motorShield.getMotor(4)) {}

  void begin() {
    LOG("Initializing motor driver...");
    m_motorShield.begin();
  }

  void onPublish(char *topic, byte *payload, unsigned int len) {
    float speed = *(float *) payload;
    if (strcmp_P(topic, ROBOT_MOTOR_FRONT_LEFT) == 0) {
      setFrontLeft(speed);
    } else if (strcmp_P(topic, ROBOT_MOTOR_BACK_LEFT) == 0) {
      setBackLeft(speed);
    } else if (strcmp_P(topic, ROBOT_MOTOR_FRONT_RIGHT) == 0) {
      setFrontRight(speed);
    } else if (strcmp_P(topic, ROBOT_MOTOR_BACK_RIGHT) == 0) {
      setBackRight(speed);
    } else {
      LOG("Unrecognized motor topic");
    }
  }

  void setFrontLeft(float speed) { setMotor(m_frontLeftMotor, speed); }

  void setBackLeft(float speed) { setMotor(m_backLeftMotor, speed); }

  void setFrontRight(float speed) { setMotor(m_frontRightMotor, speed); }

  void setBackRight(float speed) { setMotor(m_backRightMotor, speed); }

protected:
  void setMotor(Adafruit_DCMotor *motor, float speed)  {
    byte motorCommand, motorSpeed;

    if (speed > 0.001f) {
      motorCommand = FORWARD;
      motorSpeed = 255 * speed;
    } else if (speed < -0.001f) {
      motorCommand = BACKWARD;
      motorSpeed = 255 * -speed;
    } else {
      motorCommand = BRAKE;
      motorSpeed = 0;
    }

    motor->setSpeed(motorSpeed);
    motor->run(motorCommand);
  }
};

#endif /* DRIVE_H */
