#!/usr/bin/env python
"""
@file    stopinfos2flows.py
@author  Gregor Laemmel
@date    2017-06-23
@version $Id: stopinfos2flows.py 23851 2017-04-06 21:05:49Z behrisch $


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os
import sys

from optparse import OptionParser

import sumolib

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
    from sumolib.miscutils import Colorgen
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    optParser = OptionParser()
    optParser.add_option("-i", "--stopinfos-file", dest="stopinfos", help="file from '--stop-output'")
    optParser.add_option("-r", "--routes-file", dest="routes", help="file from '--vehroute-output'")
    optParser.add_option("-f", "--flows-file", dest="flows", default="flows.xml", help="output flows file")
    (options, args) = optParser.parse_args()
    return options


def main():
    options = get_options()

    stopsUntil = {}
    for stop in sumolib.output.parse_fast(options.stopinfos, 'stopinfo', ['id','ended','busStop']):
        print(stop)
        stopsUntil[stop.busStop] = stop.ended

    with open(options.flows, 'w') as foutflows:
        flows = []
        sumolib.writeXMLHeader(
            foutflows, "$Id: ptlines2trips.py 24746 2017-06-19 09:04:59Z behrisch $", "routes")
        trp_nr = 0
        for vehicle in sumolib.output.parse(options.routes, 'vehicle'):
            id = vehicle.id
            flows.append(id)
            edges = vehicle.routeDistribution[0]._child_dict['route'][1].edges
            stops = vehicle.stop
            foutflows.write(
                '\t<route id="%s" edges="%s" >\n' % (id,edges))
            for stop in stops:
                print(stop.busStop + " " + stop.duration + " " + stopsUntil[stop.busStop])
                foutflows.write(
                    '\t\t<stop busStop="%s" duration="%s" until="%s" />\n'%(stop.busStop,stop.duration,stopsUntil[stop.busStop])
                )
            print(edges)
            foutflows.write('\t</route>\n')
        for flow in flows:
            foutflows.write('<flow id="%s" route="%s" begin="0" end="7200" period="60" />\n' % (flow,flow))
        foutflows.write('</routes>\n')


if __name__ == "__main__":
    main()
