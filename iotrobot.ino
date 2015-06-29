/*
 * iotrobot.ino - A simple IoT-enabled robot that communicates over MQTT
 * 
 * Dependencies:
 *   https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
 *   https://github.com/adafruit/Adafruit_LSM303
 *   https://github.com/adafruit/Adafruit_L3GD20
 *   https://github.com/sparkfun/BMP180_Breakout/
 *   https://github.com/sparkfun/SFE_CC3000_Library
 *   https://github.com/knolleary/pubsubclient/
 */
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_L3GD20.h>
#include <SFE_BMP180.h>
#include <SFE_CC3000_Client.h>
#include <PubSubClient.h>
#include "topics.h"

#define DEBUG

#ifdef DEBUG
#  define LOG(message) \
  Serial.println(F("[log] " message));
#  define PANIC(message) \
  Serial.println(F("[panic] " message)); \
  while (1) ;
#else
#  define LOG(message) ;
#  define PANIC(message) ;
#endif

Adafruit_MotorShield motorShield = Adafruit_MotorShield();
Adafruit_DCMotor *frontLeftMotor = motorShield.getMotor(1);
Adafruit_DCMotor *backLeftMotor = motorShield.getMotor(2);
Adafruit_DCMotor *frontRightMotor = motorShield.getMotor(3);
Adafruit_DCMotor *backRightMotor = motorShield.getMotor(4);
Adafruit_LSM303 accelerometer;
Adafruit_L3GD20 gyro;
SFE_BMP180 temperature;

SFE_CC3000 wifi = SFE_CC3000(2, 7, 10);
SFE_CC3000_Client wifiClient = SFE_CC3000_Client(wifi);

byte BROKER_ADDRESS[] = {192, 168, 1, 1};
PubSubClient mqttClient(BROKER_ADDRESS, 1883, onPublish, wifiClient);

unsigned long lastPublishTime = 0;

/*
 * Intitialize all of the peripherals and set up a network connection
 */
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  LOG("==========================================================================");

  LOG("Initializing motor shield...");
  motorShield.begin();

  LOG("Initializing WiFi shield...");
  while (!wifi.init()) {
    LOG("error initializing WiFi shield");
  }

  LOG("Connecting to access point...");
  while (!wifi.connect("utx", WLAN_SEC_WPA2, "redhat123", 10000)) {
    LOG("Error connecting to access point.");
    delay(1000);
    LOG("Retrying...");
  }

  LOG("Connecting to MQTT broker...");
  char *id = "iotrobot-A";
  while (!mqttClient.connect(id, "admin", "admin")) {
    LOG("Error connecting to MQTT broker.");
    delay(1000);
    LOG("Retrying...");
    id[9]++;
    Serial.println(id);
  }

  LOG("subscribing to control topics...");
  while (!mqttClient.subscribe(getTopicString(ROBOT_MOTORS))) {
    LOG("error subscribing to topic");
    delay(500);
  }

  LOG("Initializing accelerometer...");
  if (!accelerometer.begin()) {
    PANIC("error initializing accelerometer");
  }

  LOG("Initializing gyro...");
  if (!gyro.begin(Adafruit_L3GD20::L3DS20_RANGE_250DPS)) {
    PANIC("error initializing gyro");
  }

  LOG("Initializing pressure/temperature sensor...");
  if (!temperature.begin()) {
    PANIC("Error initializing pressure/temperature sensor");
  }

  LOG("Starting main loop.");
}

/*
 * Publish all of the sensor data and control the motors
 */
void loop() {
  // Publish everything every 100 milliseconds
  unsigned long currentTime = millis();
  if (currentTime > lastPublishTime + 100) {
    lastPublishTime = currentTime;

    temperature.startTemperature();
    accelerometer.read();
    gyro.read();

    // Publish the robot acceleration and magnometer reading
    mqttClient.publish(getTopicString(ROBOT_ACCELEROMETER_X), (byte *)&accelerometer.accelData.x, 4);
    mqttClient.publish(getTopicString(ROBOT_ACCELEROMETER_Y), (byte *)&accelerometer.accelData.y, 4);
    mqttClient.publish(getTopicString(ROBOT_ACCELEROMETER_Z), (byte *)&accelerometer.accelData.z, 4);
    mqttClient.publish(getTopicString(ROBOT_MAGNETOMETER_X), (byte *)&accelerometer.magData.x, 4);
    mqttClient.publish(getTopicString(ROBOT_MAGNETOMETER_Y), (byte *)&accelerometer.magData.y, 4);
    mqttClient.publish(getTopicString(ROBOT_MAGNETOMETER_Z), (byte *)&accelerometer.magData.z, 4);

    // Publish the gyro readings
    mqttClient.publish(getTopicString(ROBOT_GYROSCOPE_X), (byte *)&gyro.data.x, 4);
    mqttClient.publish(getTopicString(ROBOT_GYROSCOPE_Y), (byte *)&gyro.data.y, 4);
    mqttClient.publish(getTopicString(ROBOT_GYROSCOPE_Z), (byte *)&gyro.data.z, 4);

    // Publish the temperature
    double temp;
    temperature.getTemperature(temp);
    float t32 = (float) temp;
    mqttClient.publish(getTopicString(ROBOT_TEMPERATURE), (byte *)&t32, 4);
  }

  mqttClient.loop();
}

void onPublish(char *topic, byte *payload, unsigned int len) {
#ifdef DEBUG
  Serial.print(F("[message] \""));
  Serial.print(topic);
  Serial.print("\": {");
  for (int i = 0; i < len; i++) {
    Serial.print((int) payload[i], HEX);
    Serial.print(',');
  }
  Serial.println(F("}"));
#endif

  // For any robot/motor/+ topics, unpack the float into a motor command and
  // the speed of the specified motor.
  if (strncmp(topic, "robot/motor/", 12) == 0) {
    float motorPayload = *(float *) payload;
    byte motorCommand, motorSpeed;

    if (motorPayload > 0.001f) {
      motorCommand = FORWARD;
      motorSpeed = 255 * motorPayload;
    } else if (motorPayload < -0.001f) {
      motorCommand = BACKWARD;
      motorSpeed = 255 * -motorPayload;
    } else {
      motorCommand = BRAKE;
      motorSpeed = 0;
    }

    Adafruit_DCMotor *motor;
    if (strcmp_P(topic, ROBOT_MOTOR_FRONT_LEFT) == 0) {
      motor = frontLeftMotor;
    } else if (strcmp_P(topic, ROBOT_MOTOR_BACK_LEFT) == 0) {
      motor = backLeftMotor;
    } else if (strcmp_P(topic, ROBOT_MOTOR_FRONT_RIGHT) == 0) {
      motor = frontRightMotor;
    } else if (strcmp_P(topic, ROBOT_MOTOR_BACK_RIGHT) == 0) {
      motor = backRightMotor;
    } else {
      LOG("Unrecognized motor topic");
    }

    motor->setSpeed(motorSpeed);
    motor->run(motorCommand);
  }
}

