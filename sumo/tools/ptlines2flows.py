#!/usr/bin/env python
"""
@file    ptlines2flows.py
@author  Gregor Laemmel
@date    2017-06-23
@version $Id: ptlines2flows.py 23851 2017-04-06 21:05:49Z behrisch $


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
import subprocess

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
    optParser.add_option("-f", "--flows-file", dest="flows", default="flows.rou.xml", help="output flows file")
    optParser.add_option("-i", "--stopinfos-file", dest="stopinfos",default="stopinfos.xml", help="file from '--stop-output'")
    optParser.add_option("-r", "--routes-file", dest="routes",default="vehroutes.xml", help="file from '--vehroute-output'")
    optParser.add_option("-t", "--trips-file", dest="trips", default="trips.trips.xml", help="output trips file")
    optParser.add_option("-p", "--period", dest="period", default="600", help="period")
    optParser.add_option("-b", "--begin", dest="begin", default="0", help="start time")
    optParser.add_option("-e", "--end", dest="end", default="3600", help="end time")
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

    subprocess.call([sumolib.checkBinary("sumo"), "-r", options.trips, "-n",options.netfile, "-a", options.ptstops
                        ,"--vehroute-output",options.routes,"--stop-output",options.stopinfos])

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
            foutflows.write('<flow id="%s" route="%s" begin="%s" end="%s" period="%s" />\n' % (flow,flow,options.begin,options.end,options.period))
        foutflows.write('</routes>\n')

if __name__ == "__main__":
    main()
