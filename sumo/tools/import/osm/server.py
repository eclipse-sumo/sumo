#!/usr/bin/env python
"""
@file    server.py
@author  Jakob Stigloher
@date    2014-14-10
@version $Id$

Browser GUI for OSMget, OSMbuild, optionally randomTrips and SUMO GUI

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import struct
import SocketServer
import webbrowser
import os, sys
import datetime
from base64 import b64encode
from hashlib import sha1
from mimetools import Message
from StringIO import StringIO
import subprocess

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
sys.path.append(os.path.join(SUMO_HOME, "tools", "trip"))
sys.path.append(os.path.join(SUMO_HOME, "tools"))

polyfile = os.path.join(SUMO_HOME, "data", "typemap", "osmPolyconvert.typ.xml")
typefile = os.path.join(SUMO_HOME, "data", "typemap", "osmNetconvert.typ.xml")
pedestrianstypefile = os.path.join(SUMO_HOME, "data", "typemap", "osmNetconvertPedestrians.typ.xml")

import osmGet
import osmBuild
import randomTrips
import sumolib

class Settings:
    enable = False
    #randomTrips options
    fringeFactor = 1.0
    period = 10.0
    time = 3600
    #param: additional parameters, should be defined later
    def __init__(self, param):
       self.param = param

    #this method will be called to return an options array for randomTrips.py
    def parseTripOpts(self, netname, routename, areaFactor):
        opts = ["-n", netname, "--fringe-factor", self.fringeFactor, "-p", self.period / areaFactor, "-o", routename, "-e", self.time]
        opts += self.param
        return opts

class WebSocketsHandler(SocketServer.StreamRequestHandler):
    magic = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11'

    def setup(self):
        SocketServer.StreamRequestHandler.setup(self)
        print "connection established", self.client_address
        self.handshake_done = False
        #initialises settings objects
        self.vehicles = Settings(["--vehicle-class", "passenger", "--vclass", "passenger", "--prefix", "p-"])
        self.bicycles = Settings(["--vehicle-class", "bicycle", "--vclass", "bicycle", "--prefix", "bc-"])
        self.pedestrians = Settings(["--pedestrians"])

    def handle(self):
        while True:
            if not self.handshake_done:
                self.handshake()
            else:
                self.read_next_message()

    def read_next_message(self):
        length = ord(self.rfile.read(2)[1]) & 127
        if length == 126:
            length = struct.unpack(">H", self.rfile.read(2))[0]
        elif length == 127:
            length = struct.unpack(">Q", self.rfile.read(8))[0]
        masks = [ord(byte) for byte in self.rfile.read(4)]
        decoded = ""
        for char in self.rfile.read(length):
            decoded += chr(ord(char) ^ masks[len(decoded) % 4])
        self.on_message(decoded)

    def send_message(self, message):
        self.request.send(chr(129))
        length = len(message)
        if length <= 125:
            self.request.send(chr(length))
        elif length >= 126 and length <= 65535:
            self.request.send(126)
            self.request.send(struct.pack(">H", length))
        else:
            self.request.send(127)
            self.request.send(struct.pack(">Q", length))
        self.request.send(message)

    #Handshake method, needed to be a WebSocketServer
    def handshake(self):
        data = self.request.recv(1024).strip()
        headers = Message(StringIO(data.split('\r\n', 1)[1]))
        if headers.get("Upgrade", None) != "websocket":
            return
        print 'Handshaking...'
        key = headers['Sec-WebSocket-Key']
        digest = b64encode(sha1(key + self.magic).hexdigest().decode('hex'))
        response = 'HTTP/1.1 101 Switching Protocols\r\n'
        response += 'Upgrade: websocket\r\n'
        response += 'Connection: Upgrade\r\n'
        response += 'Sec-WebSocket-Accept: %s\r\n\r\n' % digest
        self.handshake_done = self.request.send(response)

    def on_message(self, message):
        print message
        if message[0:8] == "coords: ":
            #build with coordinates provided by JavaScript
            #bottom, left, top, right
            #as prefix is currently used a timestamp
            #try:
                self.build(datetime.datetime.now().strftime("%Y-%m-%d-%H-%M"), message[8:])
            #except:
            #    print "Build failed."

        #settings handler, they have the syntax <target><setting>?: <value>
        #target can be vehicles or pedestrians
        elif message[:10] == "vehicles: ":
            self.vehicles.enable = bool(message[10:])
        elif message[:10] == "bicycles: ":
            self.bicycles.enable = bool(message[10:])
        elif message[:13] == "pedestrians: ":
            self.pedestrians.enable = bool(message[13:])

        elif message[:22] == "vehiclesFringeFactor: " and message[22:] != "":
            self.vehicles.fringeFactor = float(message[22:])
        elif message[:22] == "bicyclesFringeFactor: " and message[22:] != "":
            self.bicycles.fringeFactor = float(message[22:])
        elif message[:25] == "pedestriansFringeFactor: " and message[25:] != "":
            self.pedestrians.fringeFactor = float(message[25:])

        elif message[:16] == "vehiclesPeriod: " and message[16:] != "":
            self.vehicles.period = float(message[16:])
        elif message[:16] == "bicyclesPeriod: " and message[16:] != "":
            self.bicycles.period = float(message[16:])
        elif message[:19] == "pedestriansPeriod: " and message[19:] != "":
            self.pedestrians.period = float(message[19:])

        elif message[:14] == "vehiclesTime: " and message[14:] != "":
            self.vehicles.time = float(message[14:])
        elif message[:14] == "bicyclesTime: " and message[14:] != "":
            self.bicycles.time = float(message[14:])
        elif message[:17] == "pedestriansTime: " and message[17:] != "":
            self.pedestrians.time = float(message[17:])

        #else an invalid message

    def build(self, prefix, bbox=False):
        def callSumo(extraopts):
            configname = prefix + ".sumocfg"
            print "Writing config file:", configname
            opts = ["sumo", "-n", netname, "-a", polyname, "--save-configuration", configname]
            opts += extraopts
            subprocess.call(opts)

            print "Calling SUMO GUI"

            try:
                subprocess.call(["sumo-gui", "-c", configname])
            except:
                print "SUMO GUI canceled"

        if bbox:
            #get the coordinates and cast them to float
            size = map(float, bbox.split(","))
            #calculates the area
            size = (size[0] - size[2]) * (size[3] - size[1])
            areaFactor = abs(size) * 5000; #to adjust period by the area
            print "Calling osmGet"
            osmGet.get(["-b", bbox, "-p", prefix])
        else:
            areaFactor = 1

        print "Calling osmBuild"
        #output name for the osm file, will be used by osmBuild, can be after the process deleted
        osmname = prefix + "_bbox.osm.xml"
        #output name for the net file, will be used by osmBuild, randomTrips and sumo-gui
        netname = prefix + ".net.xml"
        #output name for the poly file, will be used by osmBuild and sumo-gui
        polyname = prefix + ".poly.xml"
        options = ["-f", osmname, "-p", prefix, "-m", polyfile]
        if self.pedestrians.enable: #drop?
            options += ["--pedestrians", "--netconvert-typemap", pedestrianstypefile]
        else:
            options += ["--netconvert-typemap", typefile]
        osmBuild.build(options)

        if self.vehicles.enable or self.bicycles.enable or self.pedestrians.enable:
            print "Calling randomTrips"
            #routenames stores all routefiles and will join the items later, will be used by sumo-gui
            routenames = []

            if self.vehicles.enable:
                routename = prefix + ".trips-vehicles.xml"
                routenames.append(routename)
                randomTrips.main(randomTrips.get_options(self.vehicles.parseTripOpts(netname, routename, areaFactor)))

            if self.bicycles.enable:
                routename = prefix + ".trips-bicycles.xml"
                routenames.append(routename)
                randomTrips.main(randomTrips.get_options(self.bicycles.parseTripOpts(netname, routename, areaFactor)))

            if self.pedestrians.enable:
                routename = prefix + ".trips-pedestrians.xml"
                routenames.append(routename)
                randomTrips.main(randomTrips.get_options(self.pedestrians.parseTripOpts(netname, routename, areaFactor)))

            callSumo(["-r", ",".join(routenames), "--ignore-route-errors"])

        else:
            callSumo([])

        print "Done."
        #ready to serve another export

def main():
    #initializes a WebSocketServer at port 9999 -- if you change the port here, change it in script.js also!
    server = SocketServer.TCPServer(("localhost", 9999), WebSocketsHandler)
    #to automatically open the webpage in a browser
    webbrowser.open(os.path.join(os.path.dirname(os.path.realpath(__file__)), "index.html"))
    try:
        server.serve_forever()
    except:
        server.shutdown()


if __name__ == "__main__":
    main()
