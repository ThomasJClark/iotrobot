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

#include "log.h"
#include "topics.h"
#include "drive.h"
#include "sensors.h"

Drive drive;
Sensors sensors;

SFE_CC3000 wifi = SFE_CC3000(2, 7, 10);
SFE_CC3000_Client wifiClient = SFE_CC3000_Client(wifi);

byte BROKER_ADDRESS[] = {192, 168, 1, 1};
PubSubClient mqttClient(BROKER_ADDRESS, 1883, onPublish, wifiClient);

/*
 * Intitialize all of the peripherals and set up a network connection
 */
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  drive.begin();
  sensors.begin();

  LOG("Initializing WiFi shield...");
  while (!wifi.init()) {
    LOG("error initializing WiFi shield");
  }

  LOG("Connecting to access point...");
  while (!wifi.connect("nuc", WLAN_SEC_WPA2, "redhat123", 10000)) {
    LOG("Error connecting to access point.");
    delay(1000);
    LOG("Retrying...");
  }

  LOG("Connecting to MQTT broker...");
  while (!mqttClient.connect("iotrobot", "admin", "admin")) {
    LOG("Error connecting to MQTT broker.");
    delay(1000);
    LOG("Retrying...");
  }

  LOG("subscribing to control topics...");
  while (!mqttClient.subscribe(getTopicString(ROBOT_MOTORS))) {
    LOG("error subscribing to topic");
    delay(500);
  }

  LOG("Starting main loop.");
}

void loop() {
  // Publish everything
  sensors.publish(mqttClient);

  mqttClient.loop();
}

void onPublish(char *topic, byte *payload, unsigned int len) {
  // For any robot/motor/+ topics, pass the message along to the Drive class
  if (strncmp(topic, "robot/motor/", strlen("robot/motor/")) == 0) {
    drive.onPublish(topic, payload, len);
  }
}

