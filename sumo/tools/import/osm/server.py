#!/usr/bin/env python
"""
@file    server.py
@author  Jakob Stigloher
@date    2014-14-10
@version $Id$

Browser GUI for OSMget, OSMbuild, optionally randomTrips and SUMO GUI

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import struct
import SocketServer
import webbrowser
import os
import sys
import datetime
from base64 import b64encode
from hashlib import sha1
from mimetools import Message
from StringIO import StringIO
import subprocess

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
sys.path.append(os.path.join(SUMO_HOME, "tools", "trip"))
sys.path.append(os.path.join(SUMO_HOME, "tools"))

polyfile = os.path.join(SUMO_HOME, "data", "typemap", "osmPolyconvert.typ.xml")
typefile = os.path.join(SUMO_HOME, "data", "typemap", "osmNetconvert.typ.xml")
pedestrianstypefile = os.path.join(
    SUMO_HOME, "data", "typemap", "osmNetconvertPedestrians.typ.xml")
shipstypefile = os.path.join(
    SUMO_HOME, "data", "typemap", "osmNetconvertShips.typ.xml")

import osmGet
import osmBuild
import randomTrips
import sumolib
import route2trips

RANDOMSEED = "42"


def build(handler, prefix, bbox=False):
    sumo = sumolib.checkBinary('sumo')

    if bbox:
        sumogui = sumolib.checkBinary('sumo-gui')
    else:
        # offline test mode
        sumogui = sumo

    def callSumo(extraopts):
        guisettingsname = prefix + ".view.xml"
        print "Writing gui settings file:", guisettingsname
        with open(guisettingsname, 'w') as f:
            f.write("""
<viewsettings>
    <scheme name="real world"/>
    <delay value="20"/>
</viewsettings>
""")
        configname = prefix + ".sumocfg"
        print "Writing config file:", configname
        opts = [sumo, "-n", netname, "-a", polyname, "--gui-settings-file",
                guisettingsname, "-v", "--no-step-log", "--save-configuration", configname]
        opts += extraopts
        subprocess.call(opts)

        print "Calling SUMO GUI"

        try:
            subprocess.call([sumogui, "-c", configname])
        except:
            print "SUMO GUI canceled"

    if bbox:
        # get the coordinates and cast them to float
        size = map(float, bbox.split(","))
        # calculates the area
        size = (size[0] - size[2]) * (size[3] - size[1])
        areaFactor = abs(size) * 5000
        # to adjust period by the area
        print "Calling osmGet"
        osmGet.get(["-b", bbox, "-p", prefix])
    else:
        # offline test mode
        areaFactor = 1

    print "Calling osmBuild"
    # output name for the osm file, will be used by osmBuild, can be after the
    # process deleted
    osmname = prefix + "_bbox.osm.xml"
    # output name for the net file, will be used by osmBuild, randomTrips and
    # sumo-gui
    netname = prefix + ".net.xml"
    # output name for the poly file, will be used by osmBuild and sumo-gui
    polyname = prefix + ".poly.xml"
    options = ["-f", osmname, "-p", prefix, "-m", polyfile]
    typefiles = []
    netconvertOptions = osmBuild.DEFAULT_NETCONVERT_OPTS + ",--junctions.corner-detail,5"
    if handler.pedestrians.enable:  # drop?
        # sidewalks are already included via typefile
        netconvertOptions += ",--crossings.guess"
        typefiles.append(pedestrianstypefile)
    else:
        typefiles.append(typefile)
    if handler.ships.enable:
        typefiles.append(shipstypefile)
    options += ["--netconvert-typemap", ','.join(typefiles)]
    options += ["--netconvert-options", netconvertOptions]
    osmBuild.build(options)

    if handler.vehicles.enable or handler.bicycles.enable or handler.pedestrians.enable or handler.rails.enable or handler.ships.enable:
        print "Calling randomTrips"
        # routenames stores all routefiles and will join the items later, will
        # be used by sumo-gui
        routenames = []

        if handler.vehicles.enable:
            routename = prefix + ".vehicles.rou.xml"
            tripname = prefix + ".vehicles.trips.xml"
            routenames.append(tripname)
            randomTrips.main(randomTrips.get_options(
                handler.vehicles.parseTripOpts(netname, routename, areaFactor)))
            route2trips.main([routename], outfile=tripname)

        if handler.bicycles.enable:
            routename = prefix + ".bicycles.rou.xml"
            tripname = prefix + ".bicycles.trips.xml"
            routenames.append(tripname)
            randomTrips.main(randomTrips.get_options(
                handler.bicycles.parseTripOpts(netname, routename, areaFactor)))
            route2trips.main([routename], outfile=tripname)

        if handler.pedestrians.enable:
            routename = prefix + ".pedestrians.rou.xml"
            routenames.append(routename)
            randomTrips.main(randomTrips.get_options(
                handler.pedestrians.parseTripOpts(netname, routename, areaFactor)))

        if handler.rails.enable:
            routename = prefix + ".rails.rou.xml"
            tripname = prefix + ".rails.trips.xml"
            routenames.append(tripname)
            randomTrips.main(randomTrips.get_options(
                handler.rails.parseTripOpts(netname, routename, areaFactor)))
            route2trips.main([routename], outfile=tripname)

        if handler.ships.enable:
            routename = prefix + ".ships.rou.xml"
            tripname = prefix + ".ships.trips.xml"
            routenames.append(tripname)
            randomTrips.main(randomTrips.get_options(
                handler.ships.parseTripOpts(netname, routename, areaFactor)))
            route2trips.main([routename], outfile=tripname)

        callSumo(["-r", ",".join(routenames), "--ignore-route-errors"])

    else:
        callSumo([])

    print "Done."


class Settings:
    enable = False
    # randomTrips options
    fringeFactor = 1.0
    period = 10.0
    time = 3600
    # param: additional parameters, should be defined later

    def __init__(self, param):
        self.param = param

    # this method will be called to return an options array for randomTrips.py
    def parseTripOpts(self, netname, routename, areaFactor):
        opts = ["-n", netname, "--seed", RANDOMSEED, "--fringe-factor", self.fringeFactor,
                "-p", self.period / areaFactor, "-r", routename, "-e", self.time]
        opts += self.param
        return opts


def initSettings(handler):
    handler.vehicles = Settings(
        ["--vehicle-class", "passenger", "--vclass", "passenger", "--prefix", "veh", "--min-distance", "300"])
    handler.vehicles.fringeFactor = 5
    handler.bicycles = Settings(
        ["--vehicle-class", "bicycle", "--vclass", "bicycle", "--prefix", "bike", "--max-distance", "8000"])
    handler.bicycles.fringeFactor = 2
    handler.pedestrians = Settings(
        ["--pedestrians", "--prefix", "ped", "--max-distance", "2000"])
    handler.rails = Settings(
        ["--vehicle-class", "rail_urban", "--vclass", "rail_urban", "--prefix", "train"])
    handler.rails.fringeFactor = 40
    handler.ships = Settings(
        ["--vehicle-class", "ship", "--vclass", "ship", "--prefix", "ship"])
    handler.ships.fringeFactor = 40
    handler.ships.period = 100


class WebSocketsHandler(SocketServer.StreamRequestHandler):
    magic = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11'

    def setup(self):
        SocketServer.StreamRequestHandler.setup(self)
        print "connection established", self.client_address
        self.handshake_done = False
        # initialises settings objects
        initSettings(self)

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

    # Handshake method, needed to be a WebSocketServer
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
            # build with coordinates provided by JavaScript
            #bottom, left, top, right
            # as prefix is currently used a timestamp
            # try:
            build(self, datetime.datetime.now().strftime(
                "%Y-%m-%d-%H-%M"), message[8:])
            # except:
            #    print "Build failed."

        # settings handler, they have the syntax <target><setting>?: <value>
        # target can be vehicles or pedestrians
        elif message[:10] == "vehicles: ":
            self.vehicles.enable = bool(message[10:])
        elif message[:10] == "bicycles: ":
            self.bicycles.enable = bool(message[10:])
        elif message[:13] == "pedestrians: ":
            self.pedestrians.enable = bool(message[13:])
        elif message[:7] == "rails: ":
            self.rails.enable = bool(message[7:])

        elif message[:22] == "vehiclesFringeFactor: " and message[22:] != "":
            self.vehicles.fringeFactor = float(message[22:])
        elif message[:22] == "bicyclesFringeFactor: " and message[22:] != "":
            self.bicycles.fringeFactor = float(message[22:])
        elif message[:25] == "pedestriansFringeFactor: " and message[25:] != "":
            self.pedestrians.fringeFactor = float(message[25:])
        elif message[:19] == "railsFringeFactor: " and message[19:] != "":
            self.rails.fringeFactor = float(message[19:])

        elif message[:16] == "vehiclesPeriod: " and message[16:] != "":
            self.vehicles.period = float(message[16:])
        elif message[:16] == "bicyclesPeriod: " and message[16:] != "":
            self.bicycles.period = float(message[16:])
        elif message[:19] == "pedestriansPeriod: " and message[19:] != "":
            self.pedestrians.period = float(message[19:])
        elif message[:13] == "railsPeriod: " and message[13:] != "":
            self.pedestrians.period = float(message[13:])

        elif message[:14] == "vehiclesTime: " and message[14:] != "":
            self.vehicles.time = float(message[14:])
        elif message[:14] == "bicyclesTime: " and message[14:] != "":
            self.bicycles.time = float(message[14:])
        elif message[:17] == "pedestriansTime: " and message[17:] != "":
            self.pedestrians.time = float(message[17:])
        elif message[:11] == "railsTime: " and message[11:] != "":
            self.rails.time = float(message[11:])

        # else an invalid message


def main():
    # initializes a WebSocketServer at port 9999 -- if you change the port
    # here, change it in script.js also!
    server = SocketServer.TCPServer(("localhost", 9999), WebSocketsHandler)
    # to automatically open the webpage in a browser
    webbrowser.open(
        os.path.join(os.path.dirname(os.path.realpath(__file__)), "index.html"))
    try:
        server.serve_forever()
    except:
        server.shutdown()


if __name__ == "__main__":
    if len(sys.argv) > 1:
        # debug mode: run with the given prefix
        class DummyHandler:
            pass
        dh = DummyHandler()
        initSettings(dh)
        dh.vehicles.enable = True
        dh.bicycles.enable = True
        dh.pedestrians.enable = True
        dh.rails.enable = True
        dh.ships.enable = True

        dh.vehicles.period = 4
        dh.pedestrians.period = 4
        dh.rails.period = 10
        dh.ships.period = 200
        build(dh, *sys.argv[1:])
    else:
        main()
