#!/usr/bin/env python2

# Copyright (C) 2015 Red Hat, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import paho.mqtt.client as mqtt
import struct
import time
import curses
import sys
import math

class IoTRobotClient:
    def __init__(self, broker="localhost"):
        """A program that acts as an MQTT client and simply writes all of the
        robot-related values to the terminal.
        """
        self.broker = broker
        self.sensors = {}
        self.active_topic = ""
        self.control_label = ""

    def __call__(self, stdscr):
        """Initialize an MQTT client and begin the main loop"""
        self.stdscr = stdscr
        self.stdscr.timeout(50)

        curses.init_pair(1, curses.COLOR_GREEN, curses.COLOR_BLACK)
        curses.init_pair(2, curses.COLOR_YELLOW, curses.COLOR_BLACK)
        curses.init_pair(3, curses.COLOR_BLUE, curses.COLOR_BLACK)

        client = mqtt.Client("Robot Dashboard")
        client.username_pw_set("admin", "admin")
        client.on_connect = self.on_connect
        client.on_message = self.on_message
        client.connect(self.broker)
        client.loop_start()

        self.drive(client, 0.0, 0.0, 0.0, 0.0)
        self.redraw()

        while True:
            key = stdscr.getch()

            if key != -1:
                if key in (curses.KEY_UP, ord('w')):
                    self.drive(client, 1.0, 1.0, 1.0, 1.0)
                    self.control_label = "Forward"
                elif key in (curses.KEY_DOWN, ord('s')):
                    self.drive(client, -1.0, -1.0, -1.0, -1.0)
                    self.control_label = "Backward"
                elif key in (curses.KEY_LEFT, ord('a')):
                    self.drive(client, -1.0, -1.0, 1.0, 1.0)
                    self.control_label = "Left"
                elif key in (curses.KEY_RIGHT, ord('d')):
                    self.drive(client, 1.0, 1.0, -1.0, -1.0)
                    self.control_label = "Right"
                elif key == ord('q'):
                    self.drive(client, 0.0, 0.0, 0.0, 0.0)
                    break
                else:
                    self.drive(client, 0.0, 0.0, 0.0, 0.0)
                    self.control_label = "Stop"

                self.redraw()

            self.stdscr.refresh()

        client.loop_stop()

    def on_connect(self, client, data, rc, _):
        """Subscribe to all of the robot/ MQTT topics"""
        client.subscribe("robot/#", qos=2)
        self.redraw()

    def on_message(self, client, data, msg):
        """Update whatever sensor was published and redraw the screen"""
        self.active_topic = msg.topic

        if len(msg.payload) == 1:
            self.sensors[msg.topic] = ord(msg.payload[0])
        elif len(msg.payload) == 4:
            self.sensors[msg.topic], = struct.unpack("<f", msg.payload)

        self.redraw()

    def redraw(self):
        """Render a list of known topics and their values using ncurses"""
        self.stdscr.clear()
        self.stdscr.box()
        self.stdscr.addstr(0, 4, "Robot Dashboard")

        self.stdscr.addstr(1, 2, "Control:")
        self.stdscr.addstr(1, 11, self.control_label, curses.color_pair(2  ))

        lines = len(self.sensors)
        cols = max(map(len, self.sensors)) if self.sensors else 0
        for i, topic in enumerate(sorted(self.sensors)):
            self.stdscr.addstr(i+3, 2, topic, curses.color_pair(1))
            self.stdscr.addstr(i+3, cols+3, "=", curses.A_NORMAL)
            self.stdscr.addstr(i+3, cols+5, str(self.sensors[topic]),
                curses.A_BOLD if topic == self.active_topic else curses.A_NORMAL)

    def drive(self, client, frontLeft, backLeft, frontRight, backRight):
        """Publish the desired speeds of all four motors"""
        client.publish("robot/motor/frontLeft", bytearray(struct.pack("<f", frontLeft)))
        client.publish("robot/motor/backLeft", bytearray(struct.pack("<f", backLeft)))
        client.publish("robot/motor/frontRight", bytearray(struct.pack("<f", frontRight)))
        client.publish("robot/motor/backRight", bytearray(struct.pack("<f", backRight)))

if __name__ == "__main__":
    curses.wrapper(IoTRobotClient(*sys.argv[1:]))
