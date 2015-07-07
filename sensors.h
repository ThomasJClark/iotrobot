/*
 * sensors.h - Class that encapsulates IMU and temperature sensors
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
#ifndef SENSORS_H
#define SENSORS_H

#include "log.h"

#define FILTER_CONSTANT 0.95f

class Sensors {
  Adafruit_LSM303 m_accelerometer;
  Adafruit_L3GD20 m_gyro;
  SFE_BMP180 m_temperatureSensor;

  float m_temperature;
  float m_filteredRoll, m_filteredPitch;
  unsigned long m_lastCalcTime;

public:
  void begin() {
    LOG("Initializing sensors...");

    if (!m_accelerometer.begin()) {
      PANIC("error initializing accelerometer");
    }

    if (!m_gyro.begin(Adafruit_L3GD20::L3DS20_RANGE_250DPS)) {
      PANIC("error initializing gyro");
    }

    if (!m_temperatureSensor.begin()) {
      PANIC("Error initializing pressure/temperature sensor");
    }
  }

  Adafruit_L3GD20::l3gd20Data getGyroData() const {
    return m_gyro.data;
  }

  Adafruit_LSM303::lsm303AccelData_s getAccelerometerData() const {
    return m_accelerometer.accelData;
  }

  Adafruit_LSM303::lsm303MagData_s getMagnetometerData() const {
    return m_accelerometer.magData;
  }

  float getTemperatureData() const {
    return m_temperature;
  }

  void publish(PubSubClient &mqttClient) {
    m_temperatureSensor.startTemperature();
    m_accelerometer.read();
    m_gyro.read();

    m_gyro.data.x += 0.09185684634;
    m_gyro.data.y -= 4.49060166228;
    m_gyro.data.z += 1.28516079974;

    calculateAngles();

    // Publish the robot acceleration and magnometer reading
    mqttClient.publish(getTopicString(ROBOT_ACCELEROMETER_X), (byte *)&m_accelerometer.accelData.x, 4);
    mqttClient.publish(getTopicString(ROBOT_ACCELEROMETER_Y), (byte *)&m_accelerometer.accelData.y, 4);
    mqttClient.publish(getTopicString(ROBOT_ACCELEROMETER_Z), (byte *)&m_accelerometer.accelData.z, 4);
    mqttClient.publish(getTopicString(ROBOT_MAGNETOMETER_X), (byte *)&m_accelerometer.magData.x, 4);
    mqttClient.publish(getTopicString(ROBOT_MAGNETOMETER_Y), (byte *)&m_accelerometer.magData.y, 4);
    mqttClient.publish(getTopicString(ROBOT_MAGNETOMETER_Z), (byte *)&m_accelerometer.magData.z, 4);

    // Publish the gyro readings
    mqttClient.publish(getTopicString(ROBOT_GYROSCOPE_X), (byte *)&m_gyro.data.x, 4);
    mqttClient.publish(getTopicString(ROBOT_GYROSCOPE_Y), (byte *)&m_gyro.data.y, 4);
    mqttClient.publish(getTopicString(ROBOT_GYROSCOPE_Z), (byte *)&m_gyro.data.z, 4);

    // Publish the temperature
    double temperature;
    m_temperatureSensor.getTemperature(temperature);
    m_temperature = (float) temperature;
    mqttClient.publish(getTopicString(ROBOT_TEMPERATURE), (byte *)&m_temperature, 4);

    // Publish the roll and pitch
    mqttClient.publish(getTopicString(ROBOT_ANGLE_ROLL), (byte *)&m_filteredRoll, 4);
    mqttClient.publish(getTopicString(ROBOT_ANGLE_PITCH), (byte *)&m_filteredPitch, 4);
  }

private:
  /*
   * Calculate the filtered roll and pitch
   */
  void calculateAngles() {
    float dt;
    if (!m_lastCalcTime) {
      m_lastCalcTime = millis();
      return;
    } else {
      unsigned long currentTime = millis();
      dt = 0.001f * (currentTime - m_lastCalcTime);
      m_lastCalcTime = currentTime;
    }

    const float accelX = m_accelerometer.accelData.x,
        accelY = m_accelerometer.accelData.y,
        accelZ = m_accelerometer.accelData.z,
        gyroX = m_gyro.data.x,
        gyroY = m_gyro.data.y,
        gyroZ = m_gyro.data.z,
        accelX2 = m_accelerometer.accelData.x * m_accelerometer.accelData.x,
        accelY2 = m_accelerometer.accelData.y * m_accelerometer.accelData.y,
        accelZ2 = m_accelerometer.accelData.z * m_accelerometer.accelData.z;

    const float roll = degrees(-atan2(accelX, sqrt(accelY2 + accelZ2)));
    m_filteredRoll = FILTER_CONSTANT * (m_filteredRoll + gyroY * dt)
        + (1 - FILTER_CONSTANT) * roll;

    const float pitch = degrees(atan2(accelY, sqrt(accelX2 + accelZ2)));
    m_filteredPitch = FILTER_CONSTANT * (m_filteredPitch + gyroX * dt)
        + (1 - FILTER_CONSTANT) * pitch;
  }
};

#endif /* SENSORS_H */
