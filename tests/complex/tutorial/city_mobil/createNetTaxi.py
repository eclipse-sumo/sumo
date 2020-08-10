#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    createNetTaxi.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Melanie Weber
# @date    2008-07-04

"""
Create the XML input files for the generation of the SUMO network
of the CityMobil parking lot.
"""
from __future__ import absolute_import
from __future__ import print_function
import random
import subprocess
import os
import sys
from constants import PREFIX, DOUBLE_ROWS, ROW_DIST, SLOTS_PER_ROW, SLOT_WIDTH
from constants import CAR_CAPACITY, CYBER_CAPACITY, BUS_CAPACITY, TOTAL_CAPACITY
from constants import CYBER_SPEED, CYBER_LENGTH
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa

occupied = 0
nodes = open("%s.nod.xml" % PREFIX, "w")
print("<nodes>", file=nodes)
edges = open("%s.edg.xml" % PREFIX, "w")
print("<edges>", file=edges)
with open("%s_vtypes.rou.xml" % PREFIX, "w") as vtypes:
    sumolib.xml.writeHeader(vtypes, root="routes")
    print(("""    <vType id="car" color="0.7,0.7,0.7"/>
    <vType id="ped_pedestrian" vClass="pedestrian" color="1,0.2,0.2"/>
    <vType id="cybercar" vClass="taxi" length="%s" minGap="1" guiShape="evehicle" maxSpeed="%s"
           color="green" emissionClass="HBEFA2/P_7_7" personCapacity="%s">
        <param key="has.taxi.device" value="true"/>
    </vType>
    <vType id="bus" vClass="bus" color="blue" personCapacity="%s"/>
</routes>""") % (CYBER_LENGTH, CYBER_SPEED, CYBER_CAPACITY, BUS_CAPACITY), file=vtypes)

# streets
nodeID = "main-0"
print('<node id="in" x="-100" y="0"/>', file=nodes)
print('<node id="%s" x="0" y="0"/>' % nodeID, file=nodes)
print('<edge id="mainin" from="in" to="%s" priority="-1" numLanes="2" speed="13.89"/>' %
      nodeID, file=edges)
for row in range(DOUBLE_ROWS):
    nextNodeID = "main%s-%s" % (row, row + 1)
    if row + 1 == DOUBLE_ROWS:
        nextNodeID = "main%s-%s" % (row, row+1)
    x = (row + 1) * ROW_DIST
    print('<node id="%s" x="%s" y="0"/>' % (nextNodeID, x), file=nodes)
    print('<edge id="main%s" from="%s" to="%s" priority="-1" numLanes="2" speed="13.89"/>' % (
        row, nodeID, nextNodeID), file=edges)
    nodeID = nextNodeID
print('<node id="out" x="%s" y="0"/>' % (x + 100), file=nodes)
print('<edge id="mainout" from="%s" to="out" priority="-1" numLanes="2" speed="13.89"/>' %
      nodeID, file=edges)

# parking area (road)
for row in range(DOUBLE_ROWS):
    print("""<edge id="road%s" from="main%s-%s" to="cyber%s" priority="-1" numLanes="2" speed="13.89">
    <lane index="0" allow="pedestrian" width="2.00"/>
    <lane index="1" disallow="pedestrian"/>
</edge>""" % (row, row, row+1, row), file=edges)
    print("""<edge id="-road%s" from="cyber%s" to="main%s-%s" priority="-1" numLanes="2" speed="13.89">
    <lane index="0" allow="pedestrian" width="2.00"/>
    <lane index="1" disallow="pedestrian"/>
</edge>""" % (row, row, row, row+1), file=edges)

# cybercar (automated bus)
y = (SLOTS_PER_ROW + 3) * SLOT_WIDTH
print('<node id="cyber" x="-100" y="%s"/>' % y, file=nodes)
print("""<edge id="cyberin" from="cyber" to="cyber0" numLanes="3" spreadType="center">
    <lane index="0" allow="pedestrian" width="2.00"/>
    <lane index="1" allow="taxi bus"/>
    <lane index="2" allow="taxi bus"/>
</edge>""", file=edges)
print("""<edge id="-cyberin" from="cyber0" to="cyber" numLanes="2" spreadType="center">
    <lane index="0" allow="taxi bus"/>
    <lane index="1" allow="taxi bus"/>
</edge>""", file=edges)
for row in range(DOUBLE_ROWS):
    nodeID = "cyber%s" % row
    x = (row + 1) * ROW_DIST
    print('<node id="%s" x="%s" y="%s"/>' % (nodeID, x, y), file=nodes)
    if row > 0:
        edgeID = "cyber%sto%s" % (row - 1, row)
        print("""<edge id="%s" from="cyber%s" to="cyber%s" numLanes="3" spreadType="center">
    <lane index="0" allow="pedestrian" width="2.00"/>
    <lane index="1" allow="taxi bus"/>
    <lane index="2" allow="taxi bus"/>
</edge>""" % (
            edgeID, row - 1, row), file=edges)
        print("""<edge id="-%s" from="cyber%s" to="cyber%s" numLanes="2" spreadType="center">
    <lane index="0" allow="taxi bus"/>
    <lane index="1" allow="taxi bus"/>
</edge>""" % (
            edgeID, row, row - 1), file=edges)
print('<node id="cyberend" x="%s" y="%s"/>' % (x + 100, y), file=nodes)
print("""<edge id="cyberout" from="cyber%s" to="cyberend" numLanes="3" spreadType="center">
    <lane index="0" allow="pedestrian" width="2.00"/>
    <lane index="1" allow="taxi bus"/>
    <lane index="2" allow="taxi bus"/>
</edge>""" % row, file=edges)
print("""<edge id="-cyberout" from="cyberend" to="cyber%s" numLanes="2" spreadType="center">
    <lane index="0" allow="taxi bus"/>
    <lane index="1" allow="taxi bus"/>
</edge>""" % row, file=edges)

print("</nodes>", file=nodes)
nodes.close()
print("</edges>", file=edges)
edges.close()

subprocess.call([sumolib.checkBinary('netconvert'),
                 '--no-internal-links',
                 '-n', '%s.nod.xml' % PREFIX,
                 '-e', '%s.edg.xml' % PREFIX,
                 '-o', '%s.net.xml' % PREFIX])

# routes cybercar and bus
with open("%s_cyber.rou.xml" % PREFIX, "w") as routes:
    sumolib.xml.writeHeader(routes, root="routes")
    print("""    <flow id="c" type="cybercar" begin="50" period="100" number="%s" line="taxi">
        <route edges="cyberin cyber0to1"/>
    </flow>
</routes>""" % (TOTAL_CAPACITY // CYBER_CAPACITY), file=routes)

with open("%s_bus.rou.xml" % PREFIX, "w") as routes:
    sumolib.xml.writeHeader(routes, root="routes")
    print("""    <flow id="b" from="cyberin" to="cyberout" type="bus"
          begin="50" period="100" number="%s" line="taxi">""" %
          (TOTAL_CAPACITY // BUS_CAPACITY), file=routes)
    for row in range(DOUBLE_ROWS-1):
        edgeID = "cyber%sto%s" % (row, row + 1)
        print('    <stop busStop="%sstop" duration="10"/>' % edgeID, file=routes)
    print("    </flow>\n</routes>", file=routes)

# Bus Stops / Parking Areas
stops = open("%s.add.xml" % PREFIX, "w")
print("<additional>", file=stops)
for row in range(DOUBLE_ROWS):
    print("""    <parkingArea id="ParkArea%s" lane="road%s_1" roadsideCapacity="10" angle="90" length="10"/>
    <parkingArea id="ParkArea-%s" lane="-road%s_1" roadsideCapacity="10" angle="90" length="10"/>""" %
          (row, row, row, row), file=stops)

for row in range(DOUBLE_ROWS-1):
    edgeID = "cyber%sto%s" % (row, row + 1)
    print('    <busStop id="%sstop" lane="%s_1" startPos="5" endPos="30"/>' % (edgeID, edgeID), file=stops)
print("</additional>", file=stops)
stops.close()

totalSlots = 2 * DOUBLE_ROWS * SLOTS_PER_ROW

for period in range(5, 50, 5):
    # routes person
    routes = open("%s_demand%02i.rou.xml" % (PREFIX, period), "w")
    print("<routes>", file=routes)
    for v in range(SLOTS_PER_ROW):
        for idx in range(DOUBLE_ROWS):
            busStop = "cyber%sto%sstop" % (idx, idx+1) if idx < DOUBLE_ROWS - 1 else "cyber%sto%sstop" % (idx-1, idx)
            print("""    <trip id="v%s.%s" type="car" depart="%s" from="mainin" to="road%s">
        <stop parkingArea="ParkArea%s" duration="10000"/>
    </trip>""" % (idx, v, v * period, idx, idx), file=routes)
            print("""    <trip id="v-%s.%s" type="car" depart="%s" from="mainin" to="-road%s">
        <stop parkingArea="ParkArea-%s" duration="10000"/>
    </trip>""" % (idx, v, v * period, idx, idx), file=routes)
            for p in range(random.randint(1, CAR_CAPACITY)):
                for infix in ("", "-"):
                    vehId = "v%s%s.%s" % (infix, idx, v)
                    print("""    <person id="%sp%s" type="ped_pedestrian" depart="triggered">
        <ride from="mainin" to="%sroad%s" lines="%s"/>
        <walk busStop="%s"/>
        <ride to="cyberout" lines="taxi"/>
    </person>""" % (vehId, p, infix, idx, vehId, busStop), file=routes)
    print("</routes>", file=routes)
    routes.close()

    # sumo config, the "traditional" bus does not work currently
    config = open("%s%02i.sumocfg" % (PREFIX, period), "w")
    print("""<configuration>
    <input>
        <net-file value="%s.net.xml"/>
        <route-files value="%s_vtypes.rou.xml,%s_bus.rou.xml,%s_demand%02i.rou.xml"/>
        <additional-files value="%s.add.xml"/>
        <no-step-log value="True"/>
        <time-to-teleport value="0"/>
    </input>
</configuration>""" % (PREFIX, PREFIX, PREFIX, PREFIX, period, PREFIX), file=config)
    config.close()

    config = open("%s%02i_cyber.sumocfg" % (PREFIX, period), "w")
    print("""<configuration>
    <input>
        <net-file value="%s.net.xml"/>
        <route-files value="%s_vtypes.rou.xml,%s_cyber.rou.xml,%s_demand%02i.rou.xml"/>
        <additional-files value="%s.add.xml"/>
        <no-step-log value="True"/>
        <time-to-teleport value="0"/>
        <device.taxi.dispatch-algorithm value="routeExtension"/>
    </input>
</configuration>""" % (PREFIX, PREFIX, PREFIX, PREFIX, period, PREFIX), file=config)
    config.close()
