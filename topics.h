/*
 * topics.h - MQTT topic strings, stored in PROGMEM
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
#ifndef TOPICS_H
#define TOPICS_H

static char const ROBOT_ACCELEROMETER_X[] PROGMEM = "robot/accel/x";
static char const ROBOT_ACCELEROMETER_Y[] PROGMEM = "robot/accel/y";
static char const ROBOT_ACCELEROMETER_Z[] PROGMEM = "robot/accel/z";
static char const ROBOT_GYROSCOPE_X[] PROGMEM = "robot/gyro/x";
static char const ROBOT_GYROSCOPE_Y[] PROGMEM = "robot/gyro/y";
static char const ROBOT_GYROSCOPE_Z[] PROGMEM = "robot/gyro/z";
static char const ROBOT_MAGNETOMETER_X[] PROGMEM = "robot/mag/x";
static char const ROBOT_MAGNETOMETER_Y[] PROGMEM = "robot/mag/y";
static char const ROBOT_MAGNETOMETER_Z[] PROGMEM = "robot/mag/z";
static char const ROBOT_ANGLE_ROLL[] PROGMEM = "robot/angle/roll";
static char const ROBOT_ANGLE_PITCH[] PROGMEM = "robot/angle/pitch";
static char const ROBOT_TEMPERATURE[] PROGMEM = "robot/temperature";
static char const ROBOT_MOTOR_FRONT_LEFT[] PROGMEM = "robot/motor/frontLeft";
static char const ROBOT_MOTOR_BACK_LEFT[] PROGMEM = "robot/motor/backLeft";
static char const ROBOT_MOTOR_FRONT_RIGHT[] PROGMEM = "robot/motor/frontRight";
static char const ROBOT_MOTOR_BACK_RIGHT[] PROGMEM = "robot/motor/backRight";
static char const ROBOT_MOTORS[] PROGMEM = "robot/motor/+";

char stringBuffer[32];

/*
 * Get a PROGMEM string copied into RAM.  This is used to pass topic strings
 * to PubSubClient, which expects the strings to be in RAM.
 */
char* getTopicString(const char str[] PROGMEM) {
  strcpy_P(stringBuffer, (char*)str);
  return stringBuffer;
}

#endif /* TOPICS_H */
