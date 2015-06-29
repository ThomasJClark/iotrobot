/*
 * drive.h - Class that encapsulates IMU and temperature sensors
 */
#ifndef SENSORS_H
#define SENSORS_H

#include "log.h"

class Sensors {
  Adafruit_LSM303 m_accelerometer;
  Adafruit_L3GD20 m_gyro;
  SFE_BMP180 m_temperatureSensor;

  float m_temperature;

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
  }
};

#endif /* SENSORS_H */

