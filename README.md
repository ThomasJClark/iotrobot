# iotrobot
Firmware for a simple IoT-connected Arduino robot the communicates using MQTT

## Arduino Sketch
To compile the Arduino sketch, these libraries must be installed:
 *   https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
 *   https://github.com/adafruit/Adafruit_LSM303
 *   https://github.com/adafruit/Adafruit_L3GD20
 *   https://github.com/sparkfun/BMP180_Breakout/
 *   https://github.com/sparkfun/SFE_CC3000_Library
 *   https://github.com/knolleary/pubsubclient/

The sketch attempts to connect to connect to an AP with SSID "nuc" and connect to an MQTT broker on the AP.  The actual SSID/password combination can be changed to the correct one by editing `iotrobot.ino`.  For the demo, this is an Intel NUC running JBoss Fuse 6.2.0.

## Python Clients
To run the Python client, paho-mqtt, Pygame, and PyOpenGL must be installed.

`iotrobot-client` displays any `"robot/..."` topics published, and allows setting motor speeds with the arrow keys.

    ./iotrobot-client.py 192.168.1.1 # If the client is connected to the "nuc" network
    ./iotrobot-client.py <hostname>  # If the NUC is connected through some other interface
    ./iotrobot-client.py             # If running locally on the NUC

`iotrobot-opengl` displays a rotating 3d box based on the IMU values published by the robot. It can be run with the same options as `iotrobot-client`.

![client](https://cloud.githubusercontent.com/assets/3964980/8552983/3f574812-24ae-11e5-89e5-a5864acf040b.png)
