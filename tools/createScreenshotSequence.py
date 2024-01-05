#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    createScreenshotSequence.py
# @author  Mirko Barthauer
# @date    2023-05-25

from __future__ import print_function
import os
import sys
from datetime import datetime

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
import traci  # noqa


def getOptions(args=None):
    argParser = sumolib.options.ArgumentParser(
        description="Run SUMO simulation and take screenshots of chosen time intervals")
    argParser.add_argument("--sumocfg", category="input", dest="sumocfg",
                           help="define the sumocfg to be loaded")
    argParser.add_argument("-o", "--output-dir", category="output", dest="outdir",
                           default="screenshots", help="define the output directory to write the screenshots to")
    argParser.add_argument("--begin", category="input", type=float,
                           help="simulation time the recording begins")
    argParser.add_argument("--end", category="input", type=float,
                           help="simulation time the recording ends")
    argParser.add_argument("--view", category="input", type=str, default="View #0",
                           help="View ID to use for the screenshots")
    argParser.add_argument("-p", "--prefix", category="input", dest="prefix",
                           default="", help="define a prefix of the screenshot filename")
    argParser.add_argument("--zoom",
                           help="linear interpolation of zoom values given the key frames syntax t1:v1[;t2:v2 ...]")
    argParser.add_argument("--include-time", dest="includeTime", category="processing", action="store_true",
                           default=False,
                           help="whether to include the system time at simulation begin in the file name")
    argParser.add_argument("--image-format", category="processing", dest="imageFormat",
                           default="png", help="image format to use")

    options = argParser.parse_args(args=args)
    if not options.sumocfg:
        argParser.print_help()
        sys.exit(1)
    return options


def main(options):
    # create directory if it does not exist
    outputPath = os.path.abspath(options.outdir)

    if not os.path.exists(outputPath):
        os.mkdir(outputPath)

    # remember system time before the simulation begins
    formattedTime = datetime.now().strftime("%Y%m%d-%H_%M_%S")
    traci.start(["sumo-gui", "-c", options.sumocfg])
    recordAll = options.begin is None and options.end is None
    listener = ScreenshotHelper(outputPath, "%s%s" % (options.prefix, formattedTime if options.includeTime else ""),
                                options.imageFormat, 1, view=options.view, recordAll=recordAll)
    if not recordAll:
        listener.addTimeInterval(begin=options.begin, end=options.end)
    if options.zoom is not None:
        zoomTargets = [(float(pair[:pair.index(":")]), float(pair[pair.index(":")+1:]))
                       for pair in options.zoom.split(";")]
        for t, value in zoomTargets:
            listener.addZoomTarget(t, value)
    traci.addStepListener(listener)
    sumoEndTime = listener.getEndTime()
    t = float(traci.simulation.getOption("begin"))
    simStep = float(traci.simulation.getOption("step-length"))
    while t < sumoEndTime:
        traci.simulationStep()
        t += simStep
    traci.close()


class ScreenshotHelper(traci.StepListener):

    def __init__(self, outputDir, prefix, imageFormat, frequency, view="View #0", recordAll=True):
        traci.StepListener()
        # init member variables
        self._outputDir = outputDir
        self._prefix = prefix
        self._imageFormat = imageFormat
        self._frequency = frequency
        self._view = view
        self.__counter = 0
        self._recordIntervals = []
        self._zoomTargets = []
        self._startTime = float(traci.simulation.getOption("begin"))
        self._endTime = float(traci.simulation.getOption("end"))
        self._simTime = self._startTime
        self._simStep = float(traci.simulation.getOption("step-length"))
        self._initialZoom = traci.gui.getZoom(viewID=view)
        if recordAll:
            self.addTimeInterval()
        self._imageCount = int((self._endTime - self._startTime) /
                               (float(traci.simulation.getOption("step-length")) * frequency))
        self._fileNameTemplate = "%s%0" + str(len(str(self._imageCount))) + "d.%s"

    def addZoomTarget(self, timeKey, zoomValue):
        self._zoomTargets.append((timeKey, zoomValue))
        self._zoomTargets.sort(key=lambda x: x[0])

    def addTimeInterval(self, begin=None, end=None):
        if begin is None:
            begin = self._startTime
        else:
            begin = max(0, begin)
        if end is None:
            if self._endTime > 0:
                end = self._endTime
            else:
                end = sys.maxsize
        else:
            end = max(begin + self._simStep, min(end, sys.maxsize))
        self._recordIntervals.append((begin, end))
        self._recordIntervals.sort(key=lambda x: x[0])

    def getBeginTime(self):
        return self._recordIntervals[0][0]

    def getEndTime(self):
        return self._recordIntervals[-1][1]

    def _updateTime(self):
        if self._recordIntervals[0][1] < self._simTime:
            self._recordIntervals.pop(0)
        if len(self._zoomTargets) > 0 and self._zoomTargets[0][0] < self._simTime:
            self._zoomTargets.pop(0)

    def step(self, t):
        self._updateTime()
        if len(self._recordIntervals) == 0:
            return False
        if len(self._zoomTargets) > 0:
            currentZoom = traci.gui.getZoom(viewID=self._view)
            newZoom = currentZoom + (self._zoomTargets[0][1] - currentZoom) * \
                self._simStep / (self._zoomTargets[0][0] - self._simTime)
            traci.gui.setZoom(self._view, newZoom)
        if (self._simTime > self._recordIntervals[0][0] and self._simTime <= self._recordIntervals[0][1]
                and self.__counter % self._frequency == 0):
            traci.gui.screenshot(self._view, os.path.join(self._outputDir, self._fileNameTemplate %
                                 (self._prefix, self.__counter, self._imageFormat)))
            self.__counter += 1
        self._simTime += self._simStep
        return True


if __name__ == "__main__":
    if not main(getOptions()):
        sys.exit(1)
