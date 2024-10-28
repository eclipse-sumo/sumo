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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2012-08-10

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools"))
import traci  # noqa
import sumolib  # noqa


PORT = 8813
DELTA_T = 1000

netconvertBinary = sumolib.checkBinary('netconvert')
sumoBinary = sumolib.checkBinary('sumo')


edges = [
    ["beg", ['<route id="left2end" edges="beg beg2left left left2end end"/>',
             '<route id="right2end" edges="beg beg2right right right2end end"/>']],
    ["preBeg", ['<route id="left2end" edges="preBeg beg beg2left left left2end end"/>',
                '<route id="right2end" edges="preBeg beg beg2right right right2end end"/>']]
]

rerouter = [
    ["closing", '<closingReroute id="middle"/>'],
    ["destProb", '<destProbReroute id="left2end" probability=".5"/>',
     '<destProbReroute id="right2end" probability=".5"/>'],
    ["routeProb",  '<routeProbReroute id="left2end" probability=".5"/>',
     '<routeProbReroute id="right2end" probability=".5"/>']
]

times = [
    # times for rerouter actions - begin of simulation, begin of rerouter
    # working, end of rerouter working
    [0, 0, 86400],
    [0, 100, 86400],
    [100, 0, 86400],
    [0, 50, 100],
    [50, 50, 100],
    [100, 50, 100]
]


def writeRerouterDefinition(fdo, edge, t, rerouter):
    fdo.write('    <interval begin="%s" end="%s">\n' % (t[1], t[2]))
    for i in range(1, len(rerouter)):
        fdo.write('        %s\n' % rerouter[i].replace("%e", edge))
    fdo.write('    </interval>\n')


def writeRerouter(edge, t, rerouter, embedded):
    fdo = open("rerouter.xml", "w")
    fdo.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    fdo.write('<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
              + ' xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">\n')

    definition_file = "input_definition.def.xml"
    if embedded:
        fdo.write('<rerouter id="rerouter" edges="%s">\n' % (edge))
        writeRerouterDefinition(fdo, edge, t, rerouter)
        fdo.write('</rerouter>\n')
    else:
        fdo.write('<rerouter id="rerouter" edges="%s">\n' % (edge))
        fdo.write('  <include href="%s"/>\n' % definition_file)
        fdo.write('</rerouter>\n')

    fdo.write('</additional>\n')
    fdo.close()
    if not embedded:
        fdo = open(definition_file, "w")
        writeRerouterDefinition(fdo, edge, t, rerouter)
        fdo.close()


def writeRoutes(routes, multiRef):
    fdo = open("input_routes.rou.xml", "w")
    fdo.write(
        '<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation=' +
        '"http://sumo.dlr.de/xsd/routes_file.xsd">\n\n')
    fdo.write(
        '    <vType id="DEFAULT_VEHTYPE" accel="2.6" decel="4.5" sigma="0" length="3" minGap="2" maxSpeed="70"/>\n')
    for r in routes:
        fdo.write('    ' + r + '\n')
    if multiRef:
        fdo.write(
            '    <route id="base" edges="preBeg beg middle end rend"/>\n')
    fdo.write('\n')
    for i in range(0, 10):
        if multiRef:
            fdo.write(
                '    <vehicle id="%s" depart="%s" departPos="0" route="base"/>\n' % (i, i * 20))
        else:
            fdo.write(
                '    <vehicle id="%s" depart="%s" departPos="0">\n' % (i, i * 20))
            fdo.write('        <route edges="preBeg beg middle end rend"/>\n')
            fdo.write('    </vehicle>\n')
    fdo.write('</routes>\n')
    fdo.close()


def getTimeOnEdge(route, exitTimes, edge, depart):
    i = route.index(edge)
    if i == 0:
        return depart, float(exitTimes[0])
    else:
        return float(exitTimes[i - 1]), float(exitTimes[i])


def verify(vehroutes, edge):
    for v in vehroutes:
        if v.routeDistribution is None:
            wasRerouted = False
            entryTime, leaveTime = getTimeOnEdge(
                v.route[0].edges.split(), v.route[0].exitTimes.split(), edge, float(v.depart))
        else:
            wasRerouted = True
            fr = v.routeDistribution[0].route[-1]
            entryTime, leaveTime = getTimeOnEdge(
                fr.edges.split(), fr.exitTimes.split(), edge, float(v.depart))
        if entryTime >= t[1] and entryTime < t[2] and not wasRerouted:
            print("Vehicle '%s' entering at %s was not rerouted; times (%s, %s, %s)" % (
                v.id, entryTime, t[0], t[1], t[2]))
            return
        if wasRerouted and (entryTime < t[1] and entryTime >= t[2]):
            print("Vehicle '%s' entering at %s was rerouted though the rerouter was off; times (%s, %s, %s)" % (
                v.id, entryTime, t[0], t[1], t[2]))


print(">>> Building the network")
sys.stdout.flush()
retcode = subprocess.call(
    [netconvertBinary, "-c", "netconvert.netccfg"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()

for r in rerouter:
    nd = open(os.path.devnull)
    print("Checking %s rerouting" % r[0])
    for edge in edges:
        print(" Checking at edge '%s'" % (edge[0]))
        for t in times:
            print("  Checking for simulation begin=%s, rerouter starts at %s, ends at %s" % (
                t[0], t[1], t[2]))
            for multi in [True, False]:
                print("    Checking with multi-referenced routes: %s" % multi)
                for emb in [True, False]:
                    print(
                        "     Checking with embedded rerouter definition: %s" % emb)
                    writeRoutes(edge[1], multi)
                    writeRerouter(edge[0], t, r, emb)

                    sys.stdout.flush()
                    retcode = subprocess.call(
                        [sumoBinary, "-c", "sumo.sumocfg", "-b", str(t[0])], stdout=nd, stderr=sys.stderr)
                    sys.stdout.flush()
                    v = {}

                    vehroutes = list(sumolib.output.parse(
                        "vehroutes.xml", ["vehicle"], {'route': ['exitTimes', 'edges']}))

                    wvn = 10 - int((t[0] + 10) / 20)
                    if len(vehroutes) != wvn:
                        print(
                            "Mismatching number of vehicles (%s instead of %s)" % (len(vehroutes), wvn))

                    verify(vehroutes, edge[0])
                    try:
                        os.remove("vehroutes.xml")
                    except OSError:
                        pass
    nd.close()
