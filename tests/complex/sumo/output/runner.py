#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @date    2011-06-15


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
from xml.sax import parse, handler
from collections import defaultdict
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


class OutputHandler(handler.ContentHandler):

    def __init__(self, lanes):
        self.lanes = lanes
        self.speed = dict([(la, defaultdict(dict)) for la in lanes])
        self.intervals = set()

    def startElement(self, name, attrs):
        if "id" in attrs:
            lane = attrs["id"][3:]
        if name == "interval":
            self.interval = (float(attrs["begin"]), float(attrs["end"]))
            self.intervals.add(self.interval)
            if "flow" in attrs:
                self.speed[lane]["e1"][self.interval] = float(attrs["speed"])
            if "meanMaxJamLengthInVehicles" in attrs:
                self.speed[lane]["e2"][
                    self.interval] = float(attrs["meanSpeed"])
            if "meanSpeedWithin" in attrs:
                self.speed[lane]["e3"][self.interval] = float(
                    attrs["meanSpeedWithin"])
                if self.speed[lane]["e3"][self.interval] == -1.:
                    self.speed[lane]["e3"][
                        self.interval] = float(attrs["meanSpeed"])
        if name == "edge" and "id" in attrs and attrs["id"] + "_0" in self.lanes and "speed" in attrs:
            self.speed[
                attrs["id"] + "_0"]["edge"][self.interval] = float(attrs["speed"])
        if name == "lane" and "id" in attrs and attrs["id"] in self.lanes and "speed" in attrs:
            self.speed[attrs["id"]]["lane"][
                self.interval] = float(attrs["speed"])


def generateDetectorDef(out, freq, enableLoop, laneIDs):
    print(
        '<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation=' +
        '"http://sumo.dlr.de/xsd/additional_file.xsd">', file=out)
    for laneId in laneIDs:
        if enableLoop:
            print('    <e1Detector id="e1_%s" lane="%s" pos="200" freq="%s" file="detector.xml"/>' %
                  (laneId, laneId, freq), file=out)
        print("""
        <e2Detector id="e2_%s" lane="%s" pos="0" length="30000" friendlyPos="true" freq="%s" file="detector.xml"/>
        <e3Detector id="e3_%s" freq="%s" file="detector.xml">
            <detEntry lane="%s" pos="0"/>
            <detExit lane="%s" pos="30000" friendlyPos="true"/>
        </e3Detector>""" % (laneId, laneId, freq, laneId, freq, laneId, laneId), file=out)
    print("""    <edgeData id="dump_15" freq="%s" file="meandataedge.xml" excludeEmpty="true"/>
    <laneData id="dump_15" freq="%s" file="meandatalane.xml" excludeEmpty="true"/>
</additional>""" % (freq, freq), file=out)


def checkOutput(freq, args, withLoop, lanes):
    handler = OutputHandler(lanes)
    for f in ["detector.xml", "meandataedge.xml", "meandatalane.xml"]:
        if os.path.exists(f):
            parse(f, handler)
    for i in sorted(handler.intervals):
        for lane in lanes:
            types = ["e1", "e2", "e3", "edge", "lane"]
            if not withLoop:
                types = types[1:]
            vals = [handler.speed[lane][type].get(i, -1.) for type in types]
            for v in vals[:-1]:
                if abs(v - vals[-1]) > 0.001:
                    print("failed", freq, args, lane, i, zip(types, vals))
                    return
    print("success", freq, args, lanes)


def flush():
    sys.stdout.flush()
    sys.stderr.flush()


sumoBinary = sumolib.checkBinary('sumo')
sumoArgStart = len(sys.argv)
for idx, arg in enumerate(sys.argv):
    if arg[0] == "-":
        sumoArgStart = idx
        break
lanes = sys.argv[1:sumoArgStart]
for stepLength in [".1", "1"]:
    for end in [51, 100, 1000]:
        args = sys.argv[sumoArgStart:] + \
            ["--step-length", stepLength, "--end", str(end)]
        for freq in [.1, 1, 10, 100]:
            withLoop = freq > 55 and end > 55
            for numLanes in range(1, len(lanes) + 1):
                with open("input_additional.add.xml", 'w') as out:
                    generateDetectorDef(out, freq, withLoop, lanes[:numLanes])
                exitCode = subprocess.call(
                    [sumoBinary, "-c", "sumo.sumocfg"] + args)
                flush()
                if exitCode:
                    sys.exit(exitCode)
                checkOutput(freq, args, withLoop, lanes[:numLanes])
                flush()
