"""
@file    evaluator.py
@author  Daniel Krajzewicz
@date    13-06-07
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import absolute_import
from __future__ import print_function

import sumolib.output
from matplotlib.pyplot import figure, show, axes, sci
from matplotlib import cm, colors
from pylab import *
import numpy as np
from mpl_toolkits.mplot3d import axes3d


flow1def = "0;2000;200".split(";")
flow2def = "0;2000;200".split(";")
# ["static", "agentbased", "actuated", "sotl_phase", "sotl_platoon", "sotl_wave", "sotl_marching", "swarm"]
types = ["static", "actuated", "swarm"]
fillSteps = 3600  # 3600
measureSteps = 3600  # 36000
simSteps = fillSteps + measureSteps

durationM = {}
waitStepsM = {}
durationMinMax = [0, 0]
waitStepsMinMax = [0, 0]
f1range = range(int(flow1def[0]), int(flow1def[1]), int(flow1def[2]))
rf1range = range(int(flow1def[0]), int(flow1def[1]), int(flow1def[2]))
rf1range.reverse()
f2range = range(int(flow2def[0]), int(flow2def[1]), int(flow2def[2]))
rf2range = range(int(flow2def[0]), int(flow2def[1]), int(flow2def[2]))
rf2range.reverse()
for t in types:
    print("Processing outputs for %s" % t)
    durationM[t] = []
    waitStepsM[t] = []
    for f1 in rf1range:
        print(" f1 at %s" % f1)
        durationM[t].append([])
        waitStepsM[t].append([])
        for f2 in f2range:
            duration = 0
            waitSteps = 0
            vehNum = 0
            # summary
            pd = sumolib.output.parse(
                "results/tripinfos_%s_%s_%s.xml" % (t, f1, f2), "tripinfo")
            for v in pd:
                if float(v.depart) < 3600:
                    continue
                duration = duration + float(v.duration)
                waitSteps = waitSteps + float(v.waitSteps)
                vehNum = vehNum + 1
            if vehNum != 0:
                duration = duration / float(vehNum)
                waitSteps = waitSteps / float(vehNum)
            durationM[t][-1].append(duration)
            waitStepsM[t][-1].append(waitSteps)
            if duration > durationMinMax[1]:
                durationMinMax[1] = duration
            if waitSteps > waitStepsMinMax[1]:
                waitStepsMinMax[1] = waitSteps

    """
   <interval begin="0.00" end="60.00" id="2i_l0" nSamples="55" 
   meanSpeed="13.89" meanOccupancy="1.19" maxOccupancy="3.90" 
   meanMaxJamLengthInVehicles="0.00" meanMaxJamLengthInMeters="0.00" maxJamLengthInVehicles="0" maxJamLengthInMeters="0.00" 
   jamLengthInVehiclesSum="0" jamLengthInMetersSum="0.00" meanHaltingDuration="0.00" maxHaltingDuration="0.00" 
   haltingDurationSum="0.00" meanIntervalHaltingDuration="0.00" maxIntervalHaltingDuration="0.00" intervalHaltingDurationSum="0.00" 
   startedHalts="0.00" meanVehicleNumber="0.92" maxVehicleNumber="3" />
    """


def makeIMSHOWfigure(matrix, oname, t, rangeX, rangeY, minMax=None):
    if minMax:
        im = imshow(
            matrix, vmin=minMax[0], vmax=minMax[1], interpolation='nearest')
    else:
        im = imshow(matrix, interpolation='nearest')
    legend()
    cb = colorbar(shrink=0.5)
    xticks(range(0, len(matrix)), rangeX, size=14)
    yticks(range(0, len(matrix[0])), rangeY, size=14)
    title(t)
    savefig(oname)
    clf()


for t in types:
    makeIMSHOWfigure(durationM[t], "durationSS_%s_png" %
                     t, "average travel time\n(%s)" % t, f1range, rf2range, durationMinMax)
    makeIMSHOWfigure(waitStepsM[t], "waitStepsSS_%s_png" %
                     t, "average waiting steps\n(%s)" % t, f1range, rf2range, waitStepsMinMax)
