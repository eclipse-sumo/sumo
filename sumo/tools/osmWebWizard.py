#!/usr/bin/env python
"""
@file    osmWebWizard.py
@author  Jakob Stigloher
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2014-14-10
@version $Id$

Browser GUI for OSMget, OSMbuild, optionally randomTrips and SUMO GUI

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import stat
import traceback
import webbrowser
import datetime
from argparse import ArgumentParser
import json
import threading
import subprocess
import tempfile
import shutil
from zipfile import ZipFile
import base64

import osmGet
import osmBuild
import randomTrips
import sumolib
from webWizard.SimpleWebSocketServer import SimpleWebSocketServer, WebSocket

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.join(
    os.path.dirname(os.path.abspath(__file__)), ".."))

typemapdir = os.path.join(SUMO_HOME, "data", "typemap")
typemaps = {
    "net": os.path.join(typemapdir, "osmNetconvert.typ.xml"),
    "poly": os.path.join(typemapdir, "osmPolyconvert.typ.xml"),
    "urban": os.path.join(typemapdir, "osmNetconvertUrbanDe.typ.xml"),
    "pedestrians": os.path.join(typemapdir, "osmNetconvertPedestrians.typ.xml"),
    "ships": os.path.join(typemapdir, "osmNetconvertShips.typ.xml"),
    "bicycles": os.path.join(typemapdir, "osmNetconvertBicycle.typ.xml"),
}

vehicleParameters = {
    "passenger":  ["--vehicle-class", "passenger",  "--vclass", "passenger",  "--prefix", "veh",   "--min-distance", "300",  "--trip-attributes", 'departLane="best"', "--validate"],
    "truck":      ["--vehicle-class", "truck",      "--vclass", "truck",      "--prefix", "truck", "--min-distance", "600",  "--trip-attributes", 'departLane="best"', "--validate"],
    "bus":        ["--vehicle-class", "bus",        "--vclass", "bus",        "--prefix", "bus",   "--min-distance", "600",  "--trip-attributes", 'departLane="best"', "--validate"],
    "motorcycle": ["--vehicle-class", "motorcycle", "--vclass", "motorcycle", "--prefix", "moto",  "--max-distance", "1200", "--trip-attributes", 'departLane="best"', "--validate"],
    "bicycle":    ["--vehicle-class", "bicycle",    "--vclass", "bicycle",    "--prefix", "bike",  "--max-distance", "8000", "--trip-attributes", 'departLane="best"', "--validate"],
    "tram":       ["--vehicle-class", "tram",       "--vclass", "tram",       "--prefix", "tram",  "--min-distance", "1200", "--trip-attributes", 'departLane="best"', "--validate"],
    "rail_urban": ["--vehicle-class", "rail_urban", "--vclass", "rail_urban", "--prefix", "urban", "--min-distance", "1800", "--trip-attributes", 'departLane="best"', "--validate"],
    "rail":       ["--vehicle-class", "rail",       "--vclass", "rail",       "--prefix", "rail",  "--min-distance", "2400", "--trip-attributes", 'departLane="best"', "--validate"],
    "ship":       ["--vehicle-class", "ship",       "--vclass", "ship",       "--prefix", "ship",                                                                      "--validate"],
    "pedestrian": ["--pedestrians",                                           "--prefix", "ped",   "--max-distance", "2000"]
}

vehicleNames = {
    "passenger": "Cars",
    "truck": "Trucks",
    "bus": "Bus",
    "motorcycle": "Motorcycles",
    "bicycle": "Bicycles",
    "pedestrian": "Pedestrians",
    "tram": "Trams",
    "rail_urban": "Urban Trains",
    "rail": "Trains",
    "ship": "Ships"
}


RANDOMSEED = "42"

# all can read and execute, only user can read batch files
BATCH_MODE = stat.S_IRUSR | stat.S_IRGRP | stat.S_IROTH
BATCH_MODE |= stat.S_IWUSR
BATCH_MODE |= stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH


def quoted_str(s):
    if type(s) == float:
        return "%.6f" % s
    elif type(s) != str:
        return str(s)
    elif '"' in s:
        if os.name == "nt":
            return s.replace('"', '\\"')
        else:
            return "'%s'" % s
    else:
        return s


class Builder(object):
    prefix = "osm"

    def __init__(self, data, local):
        self.files = {}
        self.data = data

        self.tmp = None
        if local:
            now = datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
            for base in ['', os.path.expanduser('~/Sumo')]:
                try:
                    self.tmp = os.path.abspath(os.path.join(base, now))
                    os.makedirs(self.tmp)
                    break
                except:
                    print("Cannot create directory '%s'" % self.tmp)
                    self.tmp = None
        if self.tmp is None:
            self.tmp = tempfile.mkdtemp()

        self.origDir = os.getcwd()
        os.chdir(self.tmp)
        print("Building scenario in '%s'" % self.tmp)

    def report(self, message):
        pass

    def filename(self, use, name):
        self.files[use] = self.prefix + name

    def build(self):
        # output name for the osm file, will be used by osmBuild, can be
        # deleted after the process
        self.filename("osm", "_bbox.osm.xml")
        # output name for the net file, will be used by osmBuild, randomTrips and
        # sumo-gui
        self.filename("net", ".net.xml")

        if 'osm' in self.data:
            # testing mode
            shutil.copy(data['osm'], self.files["osm"])
        else:
            self.report("Downloading map data")
            osmGet.get(
                ["-b", ",".join(map(str, self.data["coords"])), "-p", self.prefix])

        options = ["-f", self.files["osm"], "-p", self.prefix, "-d", self.tmp]

        if self.data["poly"]:
            # output name for the poly file, will be used by osmBuild and
            # sumo-gui
            self.filename("poly", ".poly.xml")
            options += ["-m", typemaps["poly"]]

        typefiles = [typemaps["net"]]
        netconvertOptions = osmBuild.DEFAULT_NETCONVERT_OPTS
        if "pedestrian" in self.data["vehicles"]:
            # sidewalks are already included via typefile
            netconvertOptions += ",--crossings.guess"
            typefiles.append(typemaps["urban"])
            typefiles.append(typemaps["pedestrians"])
        if "ship" in self.data["vehicles"]:
            typefiles.append(typemaps["ships"])
        if "bicycle" in self.data["vehicles"]:
            typefiles.append(typemaps["bicycles"])
        options += ["--netconvert-typemap", ','.join(typefiles)]
        options += ["--netconvert-options", netconvertOptions]

        self.report("Converting map data")
        osmBuild.build(options)

        if self.data["vehicles"]:
            # routenames stores all routefiles and will join the items later, will
            # be used by sumo-gui
            self.routenames = []
            randomTripsCalls = []

            self.edges = sumolib.net.readNet(self.files["net"]).getEdges()

            for vehicle, options in self.data["vehicles"].items():
                self.report("Processing %s" % vehicleNames[vehicle])

                self.filename("route", ".%s.rou.xml" % vehicle)
                self.filename("trips", ".%s.trips.xml" % vehicle)

                try:
                    options = self.parseTripOpts(vehicle, options)
                except ZeroDivisionError:
                    continue

                randomTrips.main(randomTrips.get_options(options))
                randomTripsCalls.append(options)

                # --validate is not called for pedestrians
                if vehicle == "pedestrian":
                    self.routenames.append(self.files["route"])
                else:
                    self.routenames.append(self.files["trips"])

            # create a batch file for reproducing calls to randomTrips.py
            randomTripsPath = os.path.join(
                SUMO_HOME, "tools", "randomTrips.py")
            batchFile = "build.bat"
            with open(batchFile, 'w') as f:
                for opts in sorted(randomTripsCalls):
                    f.write("python %s %s\n" %
                            (randomTripsPath, " ".join(map(quoted_str, opts))))

    def parseTripOpts(self, vehicle, options):
        "Return an option list for randomTrips.py for a given vehicle"

        # calculate the total length of the available lanes
        length = 0.
        for edge in self.edges:
            if edge.allows(vehicle):
                length += edge.getLaneNumber() * edge.getLength()

        period = 3600 / (length / 1000) / options["count"]

        opts = ["-n", self.files["net"], "--seed", RANDOMSEED, "--fringe-factor", options["fringeFactor"],
                "-p", period, "-r", self.files["route"], "-o", self.files["trips"], "-e", self.data["duration"]]
        opts += vehicleParameters[vehicle]
        return opts

    def makeConfigFile(self):
        "Save the configuration for SUMO in a file"

        self.report("Generating configuration file")

        self.filename("guisettings", ".view.xml")
        with open(self.files["guisettings"], 'w') as f:
            f.write("""
<viewsettings>
    <scheme name="real world"/>
    <delay value="20"/>
</viewsettings>
""")
        sumo = sumolib.checkBinary("sumo")

        self.filename("config", ".sumocfg")
        opts = [sumo, "-n", self.files["net"], "--gui-settings-file", self.files["guisettings"],
                "--duration-log.statistics",
                "--device.rerouting.adaptation-steps", "180",
                "-v", "--no-step-log", "--save-configuration", self.files["config"], "--ignore-route-errors"]

        if self.data["vehicles"]:
            opts += ["-r", ",".join(self.routenames)]

        if self.data["poly"]:
            opts += ["-a", self.files["poly"]]

        subprocess.call(opts)

    def createBatch(self):
        "Create a batch / bash file "

        # use bat as extension, as only Windows needs the extension .bat
        self.files["batch"] = "run.bat"

        with open(self.files["batch"], "w") as batchfile:
            batchfile.write("sumo-gui -c " + self.files["config"])

        os.chmod(self.files["batch"], BATCH_MODE)

    def openSUMO(self):
        self.report("Calling SUMO")

        sumogui = sumolib.checkBinary("sumo-gui")

        subprocess.Popen([sumogui, "-c", self.files["config"]])

    def createZip(self):
        "Create a zip file with everything inside which SUMO GUI needs, returns it base64 encoded"

        self.report("Building zip file")

        self.filename("zip", ".zip")

        with ZipFile(self.files["zip"], "w") as zipfile:
            files = ["net", "guisettings", "config", "batch"]

            if self.data["poly"]:
                files += ["poly"]

            # translate the pseudo file names to real file names
            files = map(lambda name: self.files[name], files)

            if self.data["vehicles"]:
                files += self.routenames

            # add the files to the zip
            for name in files:
                zipfile.write(name)

        # now open the zip file as raw
        with open(self.files["zip"], "rb") as zipfile:
            content = zipfile.read()

        return base64.b64encode(content)

    def finalize(self):
        try:
            shutil.rmtree(self.tmp)
        except:
            pass


class OSMImporterWebSocket(WebSocket):

    local = False

    def report(self, message):
        print(message)
        self.sendMessage(unicode("report " + message))
        # number of remaining steps
        self.steps -= 1

    def handleMessage(self):
        data = json.loads(self.data)

        thread = threading.Thread(target=self.build, args=(data,))
        thread.start()

    def build(self, data):
        builder = Builder(data, self.local)
        builder.report = self.report

        self.steps = len(data["vehicles"]) + 4
        self.sendMessage(unicode("steps " + str(self.steps)))

        try:
            builder.build()
            builder.makeConfigFile()
            builder.createBatch()

            if self.local:
                builder.openSUMO()
            else:
                data = builder.createZip()
                builder.finalize()

                self.sendMessage(unicode("zip " + data))
        except:
            print(traceback.format_exc())
            # reset 'Generate Scenario' button
            while self.steps > 0:
                self.report("Recovering")
        os.chdir(builder.origDir)

parser = ArgumentParser(
    description="OSM Web Wizard for SUMO - Websocket Server")
parser.add_argument("--remote", action="store_true",
                    help="In remote mode, SUMO GUI will not be automatically opened instead a zip file will be generated.")
parser.add_argument("--testing", action="store_true",
                    help="Only a pre-defined scenario will be generated for testing purposes.")
parser.add_argument("--address", default="", help="Address for the Websocket.")
parser.add_argument("--port", type=int, default=8010,
                    help="Port for the Websocket. Please edit script.js when using an other port than 8010.")

if __name__ == "__main__":
    args = parser.parse_args()
    OSMImporterWebSocket.local = args.testing or not args.remote
    if args.testing:
        data = {u'duration': 900,
                u'vehicles': {u'passenger': {u'count': 6, u'fringeFactor': 5},
                              u'bicycle': {u'count': 2, u'fringeFactor': 2},
                              u'pedestrian': {u'count': 4, u'fringeFactor': 1},
                              u'rail_urban': {u'count': 8, u'fringeFactor': 40},
                              u'ship': {u'count': 1, u'fringeFactor': 40}},
                u'osm': os.path.abspath('osm_bbox.osm.xml'),
                u'poly': True}
        builder = Builder(data, True)
        builder.build()
        builder.makeConfigFile()
        builder.createBatch()
        subprocess.call(
            [sumolib.checkBinary("sumo"), "-c", builder.files["config"]])
    else:
        if not args.remote:
            webbrowser.open("file://" +
                            os.path.join(os.path.dirname(os.path.abspath(__file__)), "webWizard", "index.html"))

        server = SimpleWebSocketServer(
            args.address, args.port, OSMImporterWebSocket)
        server.serveforever()
