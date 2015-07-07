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
import pygame
from pygame.locals import *
from OpenGL.GL import *
import sys
import math

# Colors
BASE03 = (0.0/255.0, 43.0/255.0, 54.0/255.0)
BASE02 = (7.0/255.0, 54.0/255.0, 66.0/255.0)
BASE01 = (88.0/255.0, 110.0/255.0, 117.0/255.0)
BASE00 = (101.0/255.0, 123.0/255.0, 131.0/255.0)
BASE0 = (131.0/255.0, 148.0/255.0, 150.0/255.0)
BASE1 = (147.0/255.0, 161.0/255.0, 161.0/255.0)
BASE2 = (238.0/255.0, 232.0/255.0, 213.0/255.0)
BASE3 = (253.0/255.0, 246.0/255.0, 227.0/255.0)
YELLOW = (181.0/255.0, 137.0/255.0, 0.0/255.0)
ORANGE = (203.0/255.0, 75.0/255.0, 22.0/255.0)
RED = (220.0/255.0, 50.0/255.0, 47.0/255.0)
MAGENTA = (211.0/255.0, 54.0/255.0, 130.0/255.0)
VIOLET = (108.0/255.0, 113.0/255.0, 196.0/255.0)
BLUE = (38.0/255.0, 139.0/255.0, 210.0/255.0)
CYAN = (42.0/255.0, 161.0/255.0, 152.0/255.0)
GREEN = (133.0/255.0, 153.0/255.0, 0.0/255.0)

class IoTRobotGraphicalClient:
    dirty = True
    width, height = None, None

    def __init__(self, broker="localhost"):
        self.sensors = {}
        self.measurements = {}
        self.on_resize(200, 200)

        client = mqtt.Client("Robot OpenGL Client")
        client.username_pw_set("admin", "admin")
        client.on_connect = self.on_connect
        client.on_message = self.on_message
        client.connect(broker)
        client.loop_start()


    def run(self):
        clock = pygame.time.Clock()

        glClearColor(BASE02[0], BASE02[1], BASE02[2], 1.0)
        glEnable(GL_DEPTH_TEST)

        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    sys.exit(0)
                elif event.type == pygame.VIDEORESIZE:
                    self.on_resize(event.w, event.h)

            self.redraw()
            clock.tick(100)


    def on_resize(self, width, height):
        self.width = width
        self.height = height
        self.screen = pygame.display.set_mode((self.width, self.height), DOUBLEBUF|OPENGL|RESIZABLE)


    def on_connect(self, client, data, rc, _):
        """Subscribe to all of the robot/ MQTT topics"""
        client.subscribe("robot/#", qos=2)


    def on_message(self, client, data, msg):
        """Update whatever sensor was published and redraw the screen"""
        value, = struct.unpack("<f", msg.payload)
        self.sensors[msg.topic] = value

        if msg.topic in ("robot/angle/roll", "robot/angle/pitch"):
            self.dirty = True

    def redraw(self):
        if not self.dirty: return
        self.dirty = False

        if "robot/angle/roll" in self.sensors and "robot/angle/pitch" in self.sensors:
            # Draw a box rotated by the calculated Euler angles
            glViewport(0, 0, self.width, self.height)
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

            glPushMatrix()

            glRotatef(self.sensors["robot/angle/pitch"], 1, 0, 0)
            glRotatef(self.sensors["robot/angle/roll"], 0, 0, 1)

            glBegin(GL_QUADS)
            glColor3f(*RED)
            glVertex3f(-0.4, -0.2, -0.6)
            glVertex3f(-0.4, -0.2, +0.6)
            glVertex3f(-0.4, +0.2, +0.6)
            glVertex3f(-0.4, +0.2, -0.6)
            glVertex3f(+0.4, +0.2, -0.6)
            glVertex3f(+0.4, +0.2, +0.6)
            glVertex3f(+0.4, -0.2, +0.6)
            glVertex3f(+0.4, -0.2, -0.6)
            glColor3f(*GREEN)
            glVertex3f(-0.4, -0.2, -0.6)
            glVertex3f(+0.4, -0.2, -0.6)
            glVertex3f(+0.4, +0.2, -0.6)
            glVertex3f(-0.4, +0.2, -0.6)
            glVertex3f(+0.4, +0.2, +0.6)
            glVertex3f(-0.4, +0.2, +0.6)
            glVertex3f(-0.4, -0.2, +0.6)
            glVertex3f(+0.4, -0.2, +0.6)
            glColor3f(*BLUE)
            glVertex3f(-0.4, -0.2, -0.6)
            glVertex3f(+0.4, -0.2, -0.6)
            glVertex3f(+0.4, -0.2, +0.6)
            glVertex3f(-0.4, -0.2, +0.6)
            glVertex3f(-0.4, +0.2, -0.6)
            glVertex3f(+0.4, +0.2, -0.6)
            glVertex3f(+0.4, +0.2, +0.6)
            glVertex3f(-0.4, +0.2, +0.6)
            glEnd()

            glPopMatrix()

            pygame.display.flip()

if __name__ == "__main__":
    IoTRobotGraphicalClient(*sys.argv[1:]).run()
