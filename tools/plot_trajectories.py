#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    plot_trajectories.py
# @author  Jakob Erdmann
# @author  Mirko Barthauer
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
import os
import sys
from collections import defaultdict
import matplotlib
if 'matplotlib.backends' not in sys.modules:
    if 'TEXTTEST_SANDBOX' in os.environ or (os.name == 'posix' and 'DISPLAY' not in os.environ):
        matplotlib.use('Agg')
import matplotlib.pyplot as plt  # noqa
import math  # noqa

from sumolib.xml import parse_fast_nested  # noqa
from sumolib.miscutils import uMin, uMax, parseTime  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.visualization import helpers  # noqa

KEYS = {
    't': 'Time',
    's': 'Speed',
    'd': 'Distance',
    'a': 'Acceleration',
    'i': 'Angle',
    'x': 'x-Position',
    'y': 'y-Position',
    'k': 'kilometrage',
    'g': 'gap',
}


def getOptions(args=None):
    optParser = ArgumentParser(conflict_handler="resolve")
    helpers.addInteractionOptions(optParser)
    helpers.addPlotOptions(optParser)
    optParser.add_option("-t", "--trajectory-type", dest="ttype", default="ds",
                         help="select two letters from [t, s, d, a, i, x, y, k, g] to plot"
                         + " Time, Speed, Distance, Acceleration, Angle,"
                         + " x-Position, y-Position, Kilometrage, leaderGap."
                         + " Default 'ds' plots Distance vs. Speed")
    optParser.add_option("--persons", action="store_true", default=False, help="plot person trajectories")
    optParser.add_option("-s", "--show", action="store_true", default=False, help="show plot directly")
    optParser.add_option("--csv-output", dest="csv_output", help="write plot as csv", metavar="FILE")
    optParser.add_option("-b", "--ballistic", action="store_true", default=False,
                         help="perform ballistic integration of distance")
    optParser.add_option("--filter-route", dest="filterRoute",
                         help="only export trajectories that pass the given list of edges (regardless of gaps)")
    optParser.add_option("--filter-edges", dest="filterEdges",
                         help="only consider data for the given list of edges")
    optParser.add_option("--filter-ids", dest="filterIDs",
                         help="only consider data for the given list of vehicle (or person) ids")
    optParser.add_option("-p", "--pick-distance", dest="pickDist", type=float, default=1,
                         help="pick lines within the given distance in interactive plot mode")
    optParser.add_option("-i", "--invert-distance-angle", dest="invertDistanceAngle", type=float,
                         help="invert distance for trajectories with a average angle near FLOAT")
    optParser.add_option("--label", help="plot label (default input file name")
    optParser.add_option("--invert-yaxis", dest="invertYAxis", action="store_true",
                         default=False, help="Invert the Y-Axis")
    optParser.add_option("--legend", action="store_true", default=False, help="Add legend")
    optParser.add_option("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")

    options, args = optParser.parse_known_args(args=args)
    if len(args) < 1:
        sys.exit("mandatory argument FCD_FILE missing")
    options.fcdfiles = args

    # keep old presets from before integration of common options
    options.nolegend = not options.legend
    options.blind = not options.show
    if options.output is None:
        options.output = "plot.png"

    if options.filterRoute is not None:
        options.filterRoute = options.filterRoute.split(',')
    if options.filterEdges is not None:
        options.filterEdges = set(options.filterEdges.split(','))
    if options.filterIDs is not None:
        options.filterIDs = set(options.filterIDs.split(','))
    return options


def write_csv(data, fname):
    with open(fname, 'w') as f:
        for veh, vals in sorted(data.items()):
            f.write('"%s"\n' % veh)
            for x in zip(*[vals[k] for k in KEYS if k in vals]):
                f.write(" ".join(map(str, x)) + "\n")
            f.write('\n')


def short_names(filenames):
    if len(filenames) == 1:
        return filenames
    reversedNames = [''.join(reversed(f)) for f in filenames]
    prefixLen = len(os.path.commonprefix(filenames))
    suffixLen = len(os.path.commonprefix(reversedNames))
    return [f[prefixLen:-suffixLen] for f in filenames]


def onpick(event):
    mevent = event.mouseevent
    print("veh=%s x=%d y=%d" % (event.label, mevent.xdata, mevent.ydata))


def main(options):
    fig = plt.figure(figsize=(14, 9), dpi=100)
    fig.canvas.mpl_connect('pick_event', onpick)

    xdata = None
    ydata = None
    shortFileNames = short_names(options.fcdfiles)
    xdata = options.ttype[0]
    ydata = options.ttype[1]
    if (len(options.ttype) == 2
            and xdata in KEYS
            and ydata in KEYS):
        xLabel = KEYS[xdata]
        yLabel = KEYS[ydata]
        plt.xlabel(xLabel)
        plt.ylabel(yLabel)
        plt.title(','.join(shortFileNames) if options.label is None else options.label)
    else:
        sys.exit("unsupported plot type '%s'" % options.ttype)

    element = 'vehicle'
    location = 'lane'
    if options.persons:
        element = 'person'
        location = 'edge'

    routes = defaultdict(list)  # vehID -> recorded edges
    # vehID -> (times, speeds, distances, accelerations, angles, xPositions, yPositions, kilometrage)
    attrs = ['id', 'x', 'y', 'angle', 'speed', location]
    if 'k' in options.ttype:
        attrs.append('distance')
    if 'g' in options.ttype:
        attrs.append('leaderGap')
    data = defaultdict(lambda: defaultdict(list))
    for fileIndex, fcdfile in enumerate(options.fcdfiles):
        totalVehs = 0
        filteredVehs = 0
        for timestep, vehicle in parse_fast_nested(fcdfile, 'timestep', ['time'],
                                                   element, attrs):
            totalVehs += 1
            vehID = vehicle.id
            if options.filterIDs and vehID not in options.filterIDs:
                continue
            if len(options.fcdfiles) > 1:
                suffix = shortFileNames[fileIndex]
                if len(suffix) > 0:
                    vehID += "#" + suffix
            if options.persons:
                edge = vehicle.edge
            else:
                edge = vehicle.lane[0:vehicle.lane.rfind('_')]
            if len(routes[vehID]) == 0 or routes[vehID][-1] != edge:
                routes[vehID].append(edge)
            if options.filterEdges and edge not in options.filterEdges:
                continue
            time = parseTime(timestep.time)
            speed = float(vehicle.speed)
            prevTime = time
            prevSpeed = speed
            prevDist = 0
            if vehID in data:
                prevTime = data[vehID]['t'][-1]
                prevSpeed = data[vehID]['s'][-1]
                prevDist = data[vehID]['d'][-1]
            data[vehID]['t'].append(time)
            data[vehID]['s'].append(speed)
            data[vehID]['i'].append(float(vehicle.angle))
            data[vehID]['x'].append(float(vehicle.x))
            data[vehID]['y'].append(float(vehicle.y))
            if 'k' in options.ttype:
                data[vehID]['k'].append(float(vehicle.distance))
            if 'g' in options.ttype:
                data[vehID]['g'].append(float(vehicle.leaderGap))
            if prevTime == time:
                data[vehID]['a'].append(0)
            else:
                data[vehID]['a'].append((speed - prevSpeed) / (time - prevTime))

            if options.ballistic:
                avgSpeed = (speed + prevSpeed) / 2
            else:
                avgSpeed = speed
            data[vehID]['d'].append(prevDist + (time - prevTime) * avgSpeed)
            filteredVehs += 1
        if totalVehs == 0 or filteredVehs == 0 or options.verbose:
            print("Found %s datapoints in %s and kept %s" % (
                totalVehs, fcdfile, filteredVehs))

    if filteredVehs == 0:
        sys.exit()

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

    addArgs = {"picker": line_picker, "linestyle": options.linestyle}
    if options.marker is not None:
        addArgs["marker"] = options.marker

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

        plt.plot(d[xdata], d[ydata], label=vehID, **addArgs)
    if options.invertYAxis:
        plt.axis([minX, maxX, maxY, minY])
    if options.csv_output is not None:
        write_csv(data, options.csv_output)
    helpers.closeFigure(fig, fig.axes[0], options)


if __name__ == "__main__":
    main(getOptions())
