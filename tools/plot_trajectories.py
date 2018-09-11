#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    plotTrajectories.py
# @author  Jakob Erdmann
# @date    2018-08-18
# @version $Id$

"""
This script plots fcd data for each vehicle using either
- distance vs speed
- time vs speed
- time vs distance

Individual trajectories can be clicked in interactive mode to print the vehicle Id on the console
"""
from __future__ import absolute_import
from __future__ import print_function
import sys
import os
import math
from collections import defaultdict
from optparse import OptionParser
import matplotlib.pyplot as plt

from sumolib.xml import parse_fast_nested

def getOptions(args=None):
    optParser = OptionParser()
    optParser.add_option("-t", "--trajectory-type", dest="ttype", default="ds",
                         help="select one of ('ds','ts', 'td', 'da', 'ta') for plotting" 
                         + " distanceVsSpeed (default), timeVsSpeed, timeVsDistance, distanceVsAcceleration, timeVsAcceleration")
    optParser.add_option("-s", "--show", action="store_true", default=False, help="show plot directly")
    optParser.add_option("-o", "--output", help="outputfile for saving plots", default="plot.png")
    optParser.add_option("--csv-output", dest="csv_output", help="write plot as csv", metavar="FILE")
    optParser.add_option("-b", "--ballistic", action="store_true", default=False, help="perform ballistic integration of distance")
    optParser.add_option("--filter-route", dest="filterRoute", help="only export trajectories that pass the given list of edges (regardless of gaps)")
    optParser.add_option("--pick-distance", dest="pickDist", type="float", default=1,  help="pick lines within the given distance in interactive plot mode")
    optParser.add_option("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")

    options, args = optParser.parse_args(args=args)
    if len(args) != 1:
        sys.exit("mandatory argument FCD_FILE missing")
    options.fcdfile = args[0]

    if options.filterRoute is not None:
        options.filterRoute = options.filterRoute.split(',')
    return options

def write_csv(data, fname):
    with open(fname, 'w') as f:
        for veh, vals in data.items():
            f.write('"%s"\n' % veh)
            for x in zip(*vals):
                f.write(" ".join(map(str, x)) + "\n")
            f.write('\n')

def onpick(event):
    print(event.label)

def main(options):
    fig = plt.figure(figsize=(14, 9), dpi=100)
    fig.canvas.mpl_connect('pick_event', onpick)

    xdata = 2
    ydata = 1
    if options.ttype == 'ds':
        plt.xlabel("Distance")
        plt.ylabel("Speed")
    elif options.ttype == 'ts':
        plt.xlabel("Time")
        plt.ylabel("Speed")
        xdata = 0
        ydata = 1
    elif options.ttype == 'td':
        plt.xlabel("Time")
        plt.ylabel("Distance")
        xdata = 0
        ydata = 2
    elif options.ttype == 'ta':
        plt.xlabel("Time")
        plt.ylabel("Acceleration")
        xdata = 0
        ydata = 3
    elif options.ttype == 'da':
        plt.xlabel("Distance")
        plt.ylabel("Acceleration")
        xdata = 2
        ydata = 3
    else:
        sys.exit("unsupported plot type '%s'" % options.ttype)

    routes = defaultdict(list) # vehID -> recorded edges
    data = defaultdict(lambda : ([], [], [], [])) # vehID -> (times, speeds, distances, accelerations)
    for timestep, vehicle  in parse_fast_nested(options.fcdfile, 'timestep', ['time'], 'vehicle', ['id', 'speed', 'lane']):
        time = float(timestep.time)
        speed = float(vehicle.speed)
        prevTime = time
        prevSpeed = speed
        prevDist = 0
        if vehicle.id in data:
            prevTime = data[vehicle.id][0][-1]
            prevSpeed = data[vehicle.id][1][-1]
            prevDist = data[vehicle.id][2][-1]
        data[vehicle.id][0].append(time)
        data[vehicle.id][1].append(speed)
        if prevTime == time:
            data[vehicle.id][3].append(0)
        else:
            data[vehicle.id][3].append((speed - prevSpeed) / (time - prevTime))

        if options.ballistic:
            avgSpeed = (speed + prevSpeed) / 2
        else:
            avgSpeed = speed
        data[vehicle.id][2].append(prevDist + (time - prevTime) * avgSpeed)
        edge = vehicle.lane[0:vehicle.lane.rfind('_')]
        if len(routes[vehicle.id]) == 0 or routes[vehicle.id][-1] != edge:
            routes[vehicle.id].append(edge)

    def line_picker(line, mouseevent):
        if mouseevent.xdata is None:
            return False, dict()
        for x,y in zip(line.get_xdata(), line.get_ydata()):
            if (x - mouseevent.xdata) ** 2 + (y - mouseevent.ydata) ** 2 < options.pickDist:
                return True, dict(label=line.get_label())
        return False, dict()


    for vehID, d in data.items():
        if options.filterRoute is not None:
            skip = False
            route = routes[vehID]
            for required in options.filterRoute:
                if not required in route:
                    skip = True
                    break;
            if skip:
                continue
        plt.plot(d[xdata], d[ydata], picker=line_picker, label=vehID)


    plt.savefig(options.output)
    if options.csv_output is not None:
        write_csv(data, options.csv_output)
    if options.show:
        plt.show()

if __name__ == "__main__":
    main(getOptions())
