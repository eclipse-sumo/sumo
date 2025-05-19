#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    plotStops.py
# @author  Jakob Erdmann
# @date    2025-05-16

"""
This script plots a train schedule from a route file or stop-output file
It uses plotXMLAttributes.py in the background but prepares a tick file for a
specified route id or vehicle id beforehand
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys

sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', 'visualization'))
import sumolib  # noqa
from sumolib import openz  # noqa
from sumolib.options import ArgumentParser, RawDescriptionHelpFormatter  # noqa
import plotXMLAttributes  # noqa


def getOptions(args=None):
    op = ArgumentParser(
        description=__doc__.split('\n')[1],
        epilog=__doc__,
        formatter_class=RawDescriptionHelpFormatter, conflict_handler='resolve')

    op.add_option("-r", "--route-file", dest="routeFile", category="input", required=True,
                  help="Route file for obtaining the ordering of stops")
    op.add_option("-a", "--stop-file", dest="stopFile", category="input", type=op.file,
                  help="Read ordering of stops from file")
    op.add_option("-s", "--stopinfo-file", dest="stopinfoFile", category="input", type=op.file,
                  help="plot simulation stop-output file")
    op.add_option("-t", "--time-attribute", dest="timeAttr", default="until",
                  help="attribute(s) for plotting time")
    op.add_option("-S", "--stop-order-output", dest="stopList", category="output", type=op.file, default="stoplist.txt",
                  help="write ordering and stop names to file")
    op.add_option("-i", "--veh-id", dest="vehID", required=True,
                  help="plot route for the given vehicle id")
    op.add_option("--filter-ids", dest="filterIDs",
                  help="only plot data points from the given list of ids")
    op.add_option("--label",
                  help="plot label (default input file name")
    op.add_option("--legend", action="store_true", default=False, category="visualization",
                  help="Add legend")
    op.add_option("--csv-output", dest="csv_output", category="output",
                  help="write plot as csv")
    sumolib.visualization.helpers.addInteractionOptions(op)
    op.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    options = op.parse_args(args=args)

    if options.stopinfoFile and options.timeAttr == "until":
        options.timeAttr = "started,ended"
    return options


FOUND_ATTR = []


def getStopID(stop):
    for attr in ['busStop', 'trainStop', 'edge', 'lane']:
        if stop.hasAttribute(attr):
            if not FOUND_ATTR:
                FOUND_ATTR.append(attr)
            return stop.getAttribute(attr)
    return None


def main(options):
    stopNames = dict()  # stopID -> stopName
    routeStops = dict()  # routeID -> stopIDs

    for stop in sumolib.xml.parse(options.stopFile, ['busStop', 'trainStop']):
        stopNames[stop.id] = stop.getAttributeSecure('attr_name', stop.id)

    for route in sumolib.xml.parse(options.routeFile, 'route'):
        if route.id:
            routeStops[route.id] = [getStopID(stop) for stop in route.stop]

    stops = []
    idelem = None
    xattr = None
    for veh in sumolib.xml.parse(options.routeFile, ['vehicle', 'trip', 'flow']):
        if veh.id == options.vehID:
            if veh.stop:
                stops = [getStopID(stop) for stop in veh.stop]
                idelem = veh.name
            else:
                stops = routeStops[veh.route]
                idelem = 'route'
            xattr = FOUND_ATTR[0]

    if not stops:
        print("Error: could not find vehicle trip or flow with id '%s' in route-file '%s'" % (
            options.vehID, options.routeFile), file=sys.stderr)
        sys.exit(1)
        return

    with openz(options.stopList, 'w') as slf:
        stopName2index = {}
        for i, stop in enumerate(stops):
            name = stopNames.get(stop)
            slf.write("%.3i:%s:%s\n" % (i, stop, name))
            stopName2index[name] = i

        for stop, name in stopNames.items():
            if name in stopName2index:
                slf.write("%.3i:%s:%s\n" % (stopName2index[name], stop, name))

    plot(options, idelem, xattr)


def plot(options, idelem, xattr):

    if options.stopinfoFile:
        pxargs = [options.stopinfoFile]
    else:
        pxargs = [options.routeFile, '--idelem', idelem]

    pxargs += ['-x', xattr,
               '-y', options.timeAttr,
               '--ytime1',
               '--invert-yaxis',
               '--marker', 'o',
               '--xstr',
               '--xticks-file', options.stopList,
               '--xticksorientation', '45']

    if options.label:
        pxargs += ['--label', options.label]
    if options.legend:
        pxargs += ['--legend']
    if options.filterIDs:
        pxargs += ['--filter-ids', options.filterIDs]
    if options.output:
        pxargs += ['-o', options.output]
    if options.blind:
        pxargs += ['--blind']
    if options.csv_output:
        pxargs += ['--csv-output', options.csv_output]

    if options.verbose:
        print("plotXMLAttributes.py " + " ".join(pxargs))

    plotXMLAttributes.main(plotXMLAttributes.getOptions(pxargs))


if __name__ == "__main__":
    main(getOptions())
