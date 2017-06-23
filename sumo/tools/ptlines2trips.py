#!/usr/bin/env python
"""
@file    ptlines2trips.py
@author  Gregor Laemmel
@date    2017-06-23
@version $Id: ptlines2trips.py 23851 2017-04-06 21:05:49Z behrisch $


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
    optParser.add_option("-n", "--net-file", dest="netfile", help="network file")
    optParser.add_option("-l", "--ptlines-file", dest="ptlines", help="public transit lines file")
    optParser.add_option("-s", "--ptstops-file", dest="ptstops", help="public transit stops file")
    optParser.add_option("-t", "--trips-file", dest="trips", default="trips.trips.xml", help="output trips file")
    (options, args) = optParser.parse_args()
    return options


def main():
    options = get_options()
    net = sumolib.net.readNet(options.netfile)

    stopsLanes = {}
    for stop in sumolib.output.parse_fast(options.ptstops, 'busStop', ['id', 'lane']):
        print(stop)
        stopsLanes[stop.id] = stop.lane

    with open(options.trips, 'w') as fouttrips:
        sumolib.writeXMLHeader(
            fouttrips, "$Id: ptlines2trips.py 24746 2017-06-19 09:04:59Z behrisch $", "routes")
        trp_nr = 0
        for line in sumolib.output.parse(options.ptlines, 'ptLine'):
            stops = line._child_dict['busStop']
            fr = ''
            stop_ids = []
            for stop in stops:
                laneId = stopsLanes[stop.id]
                edge_id, lane_index = laneId.rsplit("_", 1)
                if fr == '':
                    fr = edge_id
                    dep_lane = laneId

                to = edge_id
                edge = net.getEdge(edge_id)
                stop_ids.append(stop.id)
            print("from=" + fr + " to=" + to + " departLane=" + dep_lane)
            fouttrips.write(
                '\t<trip id="%s" depart="0" departLane="%s" from="%s" to="%s" >\n' % (trp_nr, 'best', fr, to))
            trp_nr += 1
            for stop in stop_ids:
                fouttrips.write('\t\t<stop busStop="%s" duration="5" />\n' % (stop))
                print("stop = " + stop)
            fouttrips.write('\t</trip>\n')
        fouttrips.write("</routes>\n")


if __name__ == "__main__":
    main()
