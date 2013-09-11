#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    createNet.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2008-07-04
@version $Id$

Create the XML input files for the generation of the SUMO network
of the CityMobil parking lot.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, random
from constants import *

occupied = 0
nodes = open("%s.nod.xml" % PREFIX, "w")
print >> nodes, "<nodes>"
edges = open("%s.edg.xml" % PREFIX, "w")
print >> edges, "<edges>"
connections = open("%s.con.xml" % PREFIX, "w")
print >> connections, "<connections>"
routes = open("%s.rou.xml" % PREFIX, "w")
print >> routes, """<routes>
    <vType id="car" length="3" minGap=".5" guiShape="passenger" maxSpeed="50" color="0.7,0.7,0.7"/>
    <vType id="person" length=".25" minGap="0" guiShape="pedestrian" width=".25" maxSpeed="5" color="1,0.2,0.2"/>
    <vType id="cybercar" length="%s" minGap="1" guiShape="evehicle" maxSpeed="%s" color="0,1,0"/>""" % (CYBER_LENGTH, CYBER_SPEED)
#streets
nodeID = "main-0"
print >> nodes, '<node id="in" x="-100" y="0"/>' 
print >> nodes, '<node id="%s" x="0" y="0"/>' % nodeID 
print >> edges, '<edge id="mainin" from="in" to="%s" numLanes="2" spreadType="center"/>' % nodeID
for row in range(DOUBLE_ROWS):
    nextNodeID = "main%s-%s" % (row, row+1)
    if row + 1 == DOUBLE_ROWS:
        nextNodeID = "main%s-" % row 
    x = (row+1) * ROW_DIST
    print >> nodes, '<node id="%s" x="%s" y="0"/>' % (nextNodeID, x) 
    print >> edges, '<edge id="main%s" from="%s" to="%s" numLanes="2" spreadType="center"/>' % (row, nodeID, nextNodeID)
    nodeID = nextNodeID 
print >> nodes, '<node id="out" x="%s" y="0"/>' % (x+100) 
print >> edges, '<edge id="mainout" from="%s" to="out" numLanes="2" spreadType="center"/>' % nodeID
roads = ["road-0"] 
for row in range(DOUBLE_ROWS-1):
    roads.append("road%s-%s" % (row, row+1))
roads.append("road%s-" % (row+1))
for idx, road in enumerate(roads):
    nodeID = road.replace("road", "main")
    for slot in range(SLOTS_PER_ROW):
        partID = "%s-%s" % (road, slot)
        print >> nodes, '<node id="%st" x="%s" y="%s"/>' % (partID, idx*ROW_DIST, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%s" from="%s" to="%st" numLanes="2" spreadType="center"/>' % (partID, nodeID, partID)
        print >> edges, '<edge id="-%s" from="%st" to="%s" numLanes="2" spreadType="center"/>' % (partID, partID, nodeID)
        nodeID = "%st" % partID
for row in range(DOUBLE_ROWS):
    for slot in range(SLOTS_PER_ROW):
        slotID = "slot%s-%sl" % (row, slot)
        source = "%s-%st" % (roads[row], slot)
        print >> nodes, '<node id="%st" x="%s" y="%s"/>' % (slotID, row*ROW_DIST+SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%s" from="%s" to="%st" spreadType="center"/>' % (slotID, source, slotID)
        print >> edges, '<edge id="-%s" from="%st" to="%s" spreadType="center"/>' % (slotID, slotID, source)
        slotID = "slot%s-%sr" % (row, slot)
        source = "%s-%st" % (roads[row+1], slot)
        print >> nodes, '<node id="%st" x="%s" y="%s"/>' % (slotID, (row+1)*ROW_DIST-SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%s" from="%s" to="%st" spreadType="center"/>' % (slotID, source, slotID)
        print >> edges, '<edge id="-%s" from="%st" to="%s" spreadType="center"/>' % (slotID, slotID, source)
#footpaths
y = (SLOTS_PER_ROW+1) * SLOT_WIDTH
print >> nodes, '<node id="foot" x="-100" y="%s"/>' % y
print >> edges, '<edge id="footmainin" from="foot" to="foot0" speed="5" spreadType="center"/>'
for row in range(DOUBLE_ROWS):
    nodeID = "foot%s" % row
    x = row * ROW_DIST + ROW_DIST/2
    print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (nodeID, x, y) 
    if row > 0:
        edgeID = "footmain%sto%s" % (row-1, row)
        print >> edges, '<edge id="%s" from="foot%s" to="foot%s" speed="5" spreadType="center"/>' % (edgeID, row-1, row)
    for slot in reversed(range(SLOTS_PER_ROW)):
        slotID = "foot%s-%s" % (row, slot)
        print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (slotID, x, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%sto%s" from="%s" to="%s" speed="5" spreadType="center"/>' % (nodeID, slot, nodeID, slotID)
        print >> edges, '<edge id="-%sto%s" from="%s" to="%s" speed="5" spreadType="center"/>' % (nodeID, slot, slotID, nodeID)
        print >> nodes, '<node id="%srt" x="%s" y="%s"/>' % (slotID, x+SLOT_FOOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%sr" from="%s" to="%srt" spreadType="center"/>' % (slotID, slotID, slotID)
        print >> edges, '<edge id="-%sr" from="%srt" to="%s" spreadType="center"/>' % (slotID, slotID, slotID)
        print >> nodes, '<node id="%slt" x="%s" y="%s"/>' % (slotID, x-SLOT_FOOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%sl" from="%s" to="%slt" spreadType="center"/>' % (slotID, slotID, slotID)
        print >> edges, '<edge id="-%sl" from="%slt" to="%s" spreadType="center"/>' % (slotID, slotID, slotID)
        nodeID = slotID
        vSlot = slotID.replace("foot", "slot") 
        if random.uniform(0,1) < OCCUPATION_PROBABILITY:
            occupied += 1
            print >> routes, """\
    <vehicle id="v%sr" type="car" depart="0" departPos="0">
        <route edges="%sr -%sr"/>
    </vehicle>""" % (vSlot, vSlot, vSlot)
        else:
            print >> routes, """\
    <flow id="p%sr" type="person" begin="0" period="1" number="%s">
        <route edges="%sr -%sr"/>
    </flow>""" % (slotID, CAR_CAPACITY, slotID, slotID)
        if random.uniform(0,1) < OCCUPATION_PROBABILITY:
            occupied += 1
            print >> routes, """\
    <vehicle id="v%sl" type="car" depart="0" departPos="0">
        <route edges="%sl -%sl"/>
    </vehicle>""" % (vSlot, vSlot, vSlot)
        else:
            print >> routes, """\
    <flow id="p%sl" type="person" begin="0" period="1" number="%s">
        <route edges="%sl -%sl"/>
    </flow>""" % (slotID, CAR_CAPACITY, slotID, slotID)
x = DOUBLE_ROWS * ROW_DIST + ROW_DIST/2
print >> nodes, '<node id="foot%s" x="%s" y="%s"/>' % (DOUBLE_ROWS, x, y) 
edgeID = "footmain%sto%s" % (DOUBLE_ROWS-1, DOUBLE_ROWS)
print >> edges, '<edge id="%s" from="foot%s" to="foot%s" speed="5" spreadType="center"/>' % (edgeID, DOUBLE_ROWS-1, DOUBLE_ROWS)
print >> nodes, '<node id="footend" x="%s" y="%s"/>' % (x+100, y) 
print >> edges, '<edge id="footmainout" from="foot%s" to="footend" speed="5" spreadType="center"/>' % DOUBLE_ROWS 
print >> nodes, '<node id="fair" x="%s" y="%s"/>' % (x+100, y-10)
print >> edges, '<edge id="footfairin" from="fair" to="foot%s" speed="5" spreadType="center"/>' % DOUBLE_ROWS

#cybercar (automated bus)
y = (SLOTS_PER_ROW+3) * SLOT_WIDTH
print >> nodes, '<node id="cyber" x="-100" y="%s"/>' % y
print >> edges, '<edge id="cyberin" from="cyber" to="cyber0" numLanes="2" spreadType="center"/>' 
print >> edges, '<edge id="-cyberin" from="cyber0" to="cyber" numLanes="2" spreadType="center"/>' 
for row in range(DOUBLE_ROWS+1):
    nodeID = "cyber%s" % row
    x = row * ROW_DIST + ROW_DIST/2
    print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (nodeID, x, y) 
    if row > 0:
        edgeID = "cyber%sto%s" % (row-1, row)
        print >> edges, '<edge id="%s" from="cyber%s" to="cyber%s" numLanes="2" spreadType="center"/>' % (edgeID, row-1, row)
        print >> edges, '<edge id="-%s" from="cyber%s" to="cyber%s" numLanes="2" spreadType="center"/>' % (edgeID, row, row-1)
        if row < DOUBLE_ROWS:
            print >> connections, '<connection from="%s" to="cyber%sto%s"/>' % (edgeID, row, row+1)
            print >> connections, '<connection from="-cyber%sto%s" to="-%s"/>' % (row, row+1, edgeID)
        else:
            print >> connections, '<connection from="%s" to="cyberout"/>' % edgeID
            print >> connections, '<connection from="-cyberout" to="-%s"/>' % edgeID
print >> nodes, '<node id="cyberend" x="%s" y="%s"/>' % (x+100, y) 
print >> edges, '<edge id="cyberout" from="cyber%s" to="cyberend" numLanes="2" spreadType="center"/>' % row 
print >> edges, '<edge id="-cyberout" from="cyberend" to="cyber%s" numLanes="2" spreadType="center"/>' % row 


print >> nodes, "</nodes>"
nodes.close()
print >> edges, "</edges>"
edges.close()
print >> connections, "</connections>"
connections.close()

os.system("%s --no-internal-links -n %s.nod.xml -e %s.edg.xml -x %s.con.xml -o %s.net.xml" % (NETCONVERT, PREFIX, PREFIX, PREFIX, PREFIX))

numBusses = TOTAL_CAPACITY / BUS_CAPACITY
print >> routes, """    <flow id="b" type="cybercar" begin="0" period="100" number="%s">
        <route edges="cyberin"/>
    </flow>
</routes>""" % numBusses
routes.close()

routes = open("%s_cyber.rou.xml" % PREFIX, "w")
print >> routes, """<routes>
    <flow id="c" type="cybercar" begin="50" period="100" number="%s">
        <route edges="cyberin"/>
    </flow>
</routes>""" % (TOTAL_CAPACITY / CYBER_CAPACITY - numBusses)
routes.close()

stops = open("%s.add.xml" % PREFIX, "w")
print >> stops, "<additional>"
for row in range(DOUBLE_ROWS):
    edgeID = "cyber%sto%s" % (row, row+1)
    print >> stops, '    <busStop id="%sstop" lane="%s_0"' % (edgeID, edgeID),
    print >> stops, 'startPos="%s" endPos="%s"/>' % (STOP_POS-2*CYBER_LENGTH-1, STOP_POS)
for edge in ["cyberin", "cyberout"]:
    print >> stops, '    <busStop id="%sstop" lane="%s_0"' % (edge, edge),
    print >> stops, 'startPos="%s" endPos="%s"/>' % (90-2*CYBER_LENGTH-1, 90)
print >> stops, '    <edgeData id="dump" freq="3600" file="aggregated.xml" excludeEmpty="true" type="hbefa"/>' 
print >> stops, "</additional>"
stops.close()

totalSlots = 2 * DOUBLE_ROWS * SLOTS_PER_ROW
bat = open("%s.bat" % PREFIX, "w")
breakbat = open("%s_break.bat" % PREFIX, "w")
for period in range(5, 50, 5):
    routes = open("%s_demand%02i.rou.xml" % (PREFIX, period), "w")
    print >> routes, "<routes>"
    if occupied < totalSlots:
        print >> routes, """    <flow id="v" type="car" begin="10" period="%s" number="%s">
        <route edges="mainin"/>
    </flow>""" % (period, totalSlots-occupied)
    if occupied > 0:
        print >> routes, """    <flow id="p" type="person" begin="10" period="%s" number="%s">
        <route edges="footfairin"/>
    </flow>""" % (period, occupied*CAR_CAPACITY)
    print >> routes, "</routes>"
    routes.close()

    config = open("%s%02i.sumocfg" % (PREFIX, period), "w")
    print >> config, """<configuration>
    <input>
        <net-file value="%s.net.xml"/>
        <route-files value="%s.rou.xml,%s_demand%02i.rou.xml"/>
        <additional-files value="%s.add.xml"/>
        <no-step-log value="True"/>
        <time-to-teleport value="0"/>
        <remote-port value="%s"/>
    </input>
</configuration>""" % (PREFIX, PREFIX, PREFIX, period, PREFIX, PORT)
    config.close()
    print >> bat, "simpleManager.py -d %s" % period
    print >> breakbat, "simpleManager.py -b 120 -d %s" % period

    config = open("%s%02i_cyber.sumocfg" % (PREFIX, period), "w")
    print >> config, """<configuration>
    <input>
        <net-file value="%s.net.xml"/>
        <route-files value="%s.rou.xml,%s_cyber.rou.xml,%s_demand%02i.rou.xml"/>
        <additional-files value="%s.add.xml"/>
        <no-step-log value="True"/>
        <time-to-teleport value="0"/>
        <remote-port value="%s"/>
    </input>
</configuration>""" % (PREFIX, PREFIX, PREFIX, PREFIX, period, PREFIX, PORT)
    config.close()
    print >> bat, "agentManager.py -c -d %s" % period
    print >> breakbat, "agentManager.py -c -b 120 -d %s" % period
bat.close()
breakbat.close()
