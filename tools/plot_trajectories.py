#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    plot_trajectories.py
# @author  Jakob Erdmann
# @date    2018-08-18

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
from collections import defaultdict
from optparse import OptionParser
import matplotlib.pyplot as plt
import math

from sumolib.xml import parse_fast_nested
from sumolib.miscutils import uMin, uMax


def getOptions(args=None):
    optParser = OptionParser()
    optParser.add_option("-t", "--trajectory-type", dest="ttype", default="ds",
                         help="select two letters from [t, s, d, a, i, x, y] to plot"
                         + " Time, Speed, Distance, Acceleration, Angle, x-Position, y-Position."
                         + " Default 'ds' plots Distance vs. Speed")
    optParser.add_option("-s", "--show", action="store_true", default=False, help="show plot directly")
    optParser.add_option("-o", "--output", help="outputfile for saving plots", default="plot.png")
    optParser.add_option("--csv-output", dest="csv_output", help="write plot as csv", metavar="FILE")
    optParser.add_option("-b", "--ballistic", action="store_true", default=False,
                         help="perform ballistic integration of distance")
    optParser.add_option("--filter-route", dest="filterRoute",
                         help="only export trajectories that pass the given list of edges (regardless of gaps)")
    optParser.add_option("-p", "--pick-distance", dest="pickDist", type="float", default=1,
                         help="pick lines within the given distance in interactive plot mode")
    optParser.add_option("-i", "--invert-distance-angle", dest="invertDistanceAngle", type="float",
                         help="invert distance for trajectories with a average angle near FLOAT")
    optParser.add_option("--label", help="plot label (default input file name")
    optParser.add_option("--invert-yaxis", dest="invertYAxis", action="store_true",
                         default=False, help="Invert the Y-Axis")
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
    mevent = event.mouseevent
    print("veh=%s x=%d y=%d" % (event.label, mevent.xdata, mevent.ydata))


def main(options):
    fig = plt.figure(figsize=(14, 9), dpi=100)
    fig.canvas.mpl_connect('pick_event', onpick)

    xdata = 2
    ydata = 1
    typespec = {
        't': ('Time', 0),
        's': ('Speed', 1),
        'd': ('Distance', 2),
        'a': ('Acceleration', 3),
        'i': ('Angle', 4),
        'x': ('x-Position', 5),
        'y': ('y-Position', 6),
    }

    if (len(options.ttype) == 2
            and options.ttype[0] in typespec
            and options.ttype[1] in typespec):
        xLabel, xdata = typespec[options.ttype[0]]
        yLabel, ydata = typespec[options.ttype[1]]
        plt.xlabel(xLabel)
        plt.ylabel(yLabel)
        plt.title(options.fcdfile if options.label is None else options.label)
    else:
        sys.exit("unsupported plot type '%s'" % options.ttype)

    routes = defaultdict(list)  # vehID -> recorded edges
    # vehID -> (times, speeds, distances, accelerations, angles, xPositions, yPositions)
    data = defaultdict(lambda: ([], [], [], [], [], [], []))
    for timestep, vehicle in parse_fast_nested(options.fcdfile, 'timestep', ['time'],
                                               'vehicle', ['id', 'x', 'y', 'angle', 'speed', 'lane']):
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
        data[vehicle.id][4].append(float(vehicle.angle))
        data[vehicle.id][5].append(float(vehicle.x))
        data[vehicle.id][6].append(float(vehicle.y))
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
        # minxy = None
        # mindist = 10000
        for x, y in zip(line.get_xdata(), line.get_ydata()):
            dist = math.sqrt((x - mouseevent.xdata) ** 2 + (y - mouseevent.ydata) ** 2)
            if dist < options.pickDist:
                return True, dict(label=line.get_label())
            # else:
            #    if dist < mindist:
            #        print("   ", x,y, dist, (x - mouseevent.xdata) ** 2, (y - mouseevent.ydata) ** 2)
            #        mindist = dist
            #        minxy = (x, y)
        # print(mouseevent.xdata, mouseevent.ydata, minxy, dist,
        #        line.get_label())
        return False, dict()

    minY = uMax
    maxY = uMin
    minX = uMax
    maxX = uMin

    for vehID, d in data.items():
        if options.filterRoute is not None:
            skip = False
            route = routes[vehID]
            for required in options.filterRoute:
                if required not in route:
                    skip = True
                    break
            if skip:
                continue
        if options.invertDistanceAngle is not None:
            avgAngle = sum(d[4]) / len(d[4])
            if abs(avgAngle - options.invertDistanceAngle) < 45:
                maxDist = d[2][-1]
                for i, v in enumerate(d[2]):
                    d[2][i] = maxDist - v

        minY = min(minY, min(d[ydata]))
        maxY = max(maxY, max(d[ydata]))
        minX = min(minX, min(d[xdata]))
        maxX = max(maxX, max(d[xdata]))

        plt.plot(d[xdata], d[ydata], picker=line_picker, label=vehID)
    if options.invertYAxis:
        plt.axis([minX, maxX, maxY, minY])

    plt.savefig(options.output)
    if options.csv_output is not None:
        write_csv(data, options.csv_output)
    if options.show:
        plt.show()


if __name__ == "__main__":
    main(getOptions())
