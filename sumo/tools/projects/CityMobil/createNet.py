#!/usr/bin/env python
"""
@file    createNet.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-04
@version $Id$

Create the XML input files for the generation of the SUMO network
of the CityMobil parking lot.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os, random
from constants import *

occupied = 0
nodes = open("%s.nod.xml" % PREFIX, "w")
print >> nodes, "<nodes>"
edges = open("%s.edg.xml" % PREFIX, "w")
print >> edges, "<edges>"
routes = open("%s.rou.xml" % PREFIX, "w")
print >> routes, """<routes>
    <vtype id="car" length="6" maxspeed="50" color="0.6,0.6,0.6"/>
    <vtype id="person" length=".5" maxspeed="5" color="1,0.2,0.2"/>
    <vtype id="cybercar" length="7" maxspeed="%s" color="0,1,0"/>""" % CYBER_SPEED
#streets
nodeID = "main-0"
print >> nodes, '<node id="in" x="-100" y="0"/>' 
print >> nodes, '<node id="%s" x="0" y="0"/>' % nodeID 
print >> edges, '<edge id="mainin" fromnode="in" tonode="%s" nolanes="2" spread_type="center"/>' % nodeID
for row in range(DOUBLE_ROWS):
    nextNodeID = "main%s-%s" % (row, row+1)
    if row + 1 == DOUBLE_ROWS:
        nextNodeID = "main%s-" % row 
    x = (row+1) * ROW_DIST
    print >> nodes, '<node id="%s" x="%s" y="0"/>' % (nextNodeID, x) 
    print >> edges, '<edge id="main%s" fromnode="%s" tonode="%s" nolanes="2" spread_type="center"/>' % (row, nodeID, nextNodeID)
    nodeID = nextNodeID 
print >> nodes, '<node id="out" x="%s" y="0"/>' % (x+100) 
print >> edges, '<edge id="mainout" fromnode="%s" tonode="out" nolanes="2" spread_type="center"/>' % nodeID
roads = ["road-0"] 
for row in range(DOUBLE_ROWS-1):
    roads.append("road%s-%s" % (row, row+1))
roads.append("road%s-" % (row+1))
for idx, road in enumerate(roads):
    nodeID = road.replace("road", "main")
    for slot in range(SLOTS_PER_ROW):
        partID = "%s-%s" % (road, slot)
        print >> nodes, '<node id="%st" x="%s" y="%s"/>' % (partID, idx*ROW_DIST, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%s" fromnode="%s" tonode="%st" nolanes="2" spread_type="center"/>' % (partID, nodeID, partID)
        print >> edges, '<edge id="-%s" fromnode="%st" tonode="%s" nolanes="2" spread_type="center"/>' % (partID, partID, nodeID)
        nodeID = "%st" % partID
for row in range(DOUBLE_ROWS):
    for slot in range(SLOTS_PER_ROW):
        slotID = "slot%s-%sl" % (row, slot)
        source = "%s-%st" % (roads[row], slot)
        print >> nodes, '<node id="%st" x="%s" y="%s"/>' % (slotID, row*ROW_DIST+SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%s" fromnode="%s" tonode="%st" spread_type="center"/>' % (slotID, source, slotID)
        print >> edges, '<edge id="-%s" fromnode="%st" tonode="%s" spread_type="center"/>' % (slotID, slotID, source)
        slotID = "slot%s-%sr" % (row, slot)
        source = "%s-%st" % (roads[row+1], slot)
        print >> nodes, '<node id="%st" x="%s" y="%s"/>' % (slotID, (row+1)*ROW_DIST-SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%s" fromnode="%s" tonode="%st" spread_type="center"/>' % (slotID, source, slotID)
        print >> edges, '<edge id="-%s" fromnode="%st" tonode="%s" spread_type="center"/>' % (slotID, slotID, source)
#footpaths
y = (SLOTS_PER_ROW+1) * SLOT_WIDTH
print >> nodes, '<node id="foot" x="-100" y="%s"/>' % y
print >> edges, '<edge id="footmainin" fromnode="foot" tonode="foot0" speed="5" spread_type="center"/>'
for row in range(DOUBLE_ROWS):
    nodeID = "foot%s" % row
    x = row * ROW_DIST + ROW_DIST/2
    print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (nodeID, x, y) 
    if row > 0:
        edgeID = "footmain%sto%s" % (row-1, row)
        print >> edges, '<edge id="%s" fromnode="foot%s" tonode="foot%s" speed="5" spread_type="center"/>' % (edgeID, row-1, row)
    for slot in reversed(range(SLOTS_PER_ROW)):
        slotID = "foot%s-%s" % (row, slot)
        print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (slotID, x, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%sto%s" fromnode="%s" tonode="%s" speed="5" spread_type="center"/>' % (nodeID, slot, nodeID, slotID)
        print >> edges, '<edge id="-%sto%s" fromnode="%s" tonode="%s" speed="5" spread_type="center"/>' % (nodeID, slot, slotID, nodeID)
        print >> nodes, '<node id="%srt" x="%s" y="%s"/>' % (slotID, x+SLOT_FOOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%sr" fromnode="%s" tonode="%srt" spread_type="center"/>' % (slotID, slotID, slotID)
        print >> edges, '<edge id="-%sr" fromnode="%srt" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        print >> nodes, '<node id="%slt" x="%s" y="%s"/>' % (slotID, x-SLOT_FOOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%sl" fromnode="%s" tonode="%slt" spread_type="center"/>' % (slotID, slotID, slotID)
        print >> edges, '<edge id="-%sl" fromnode="%slt" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        nodeID = slotID
        vSlot = slotID.replace("foot", "slot") 
        if random.uniform(0,1) < OCCUPATION_PROBABILITY:
            occupied += 1
            print >> routes, """\
    <vehicle id="v%sr" type="car" depart="0" arrivalpos="10000">
        <route edges="%sr -%sr"/>
    </vehicle>""" % (vSlot, vSlot, vSlot)
        else:
            print >> routes, """\
    <vehicle id="p%sr" type="person" depart="0" period="1" repno="%s" arrivalpos="10000">
        <route edges="%sr -%sr"/>
    </vehicle>""" % (slotID, CAR_CAPACITY-1, slotID, slotID)
        if random.uniform(0,1) < OCCUPATION_PROBABILITY:
            occupied += 1
            print >> routes, """\
    <vehicle id="v%sl" type="car" depart="0" arrivalpos="10000">
        <route edges="%sl -%sl"/>
    </vehicle>""" % (vSlot, vSlot, vSlot)
        else:
            print >> routes, """\
    <vehicle id="p%sl" type="person" depart="0" period="1" repno="%s" arrivalpos="10000">
        <route edges="%sl -%sl"/>
    </vehicle>""" % (slotID, CAR_CAPACITY-1, slotID, slotID)
x = DOUBLE_ROWS * ROW_DIST + ROW_DIST/2
print >> nodes, '<node id="foot%s" x="%s" y="%s"/>' % (DOUBLE_ROWS, x, y) 
edgeID = "footmain%sto%s" % (DOUBLE_ROWS-1, DOUBLE_ROWS)
print >> edges, '<edge id="%s" fromnode="foot%s" tonode="foot%s" speed="5" spread_type="center"/>' % (edgeID, DOUBLE_ROWS-1, DOUBLE_ROWS)
print >> nodes, '<node id="footend" x="%s" y="%s"/>' % (x+100, y) 
print >> edges, '<edge id="footmainout" fromnode="foot%s" tonode="footend" speed="5" spread_type="center"/>' % DOUBLE_ROWS 
print >> nodes, '<node id="fair" x="%s" y="%s"/>' % (x+100, y-10)
print >> edges, '<edge id="footfairin" fromnode="fair" tonode="foot%s" speed="5" spread_type="center"/>' % DOUBLE_ROWS

#cybercar (automated bus)
y = (SLOTS_PER_ROW+3) * SLOT_WIDTH
print >> nodes, '<node id="cyber" x="-100" y="%s"/>' % y
print >> edges, '<edge id="cyberin" fromnode="cyber" tonode="cyber0" nolanes="2" spread_type="center"/>' 
print >> edges, '<edge id="-cyberin" fromnode="cyber0" tonode="cyber" nolanes="2" spread_type="center"/>' 
for row in range(DOUBLE_ROWS+1):
    nodeID = "cyber%s" % row
    x = row * ROW_DIST + ROW_DIST/2
    print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (nodeID, x, y) 
    if row > 0:
        edgeID = "cyber%sto%s" % (row-1, row)
        print >> edges, '<edge id="%s" fromnode="cyber%s" tonode="cyber%s" nolanes="2" spread_type="center"/>' % (edgeID, row-1, row)
        print >> edges, '<edge id="-%s" fromnode="cyber%s" tonode="cyber%s" nolanes="2" spread_type="center"/>' % (edgeID, row, row-1)
print >> nodes, '<node id="cyberend" x="%s" y="%s"/>' % (x+100, y) 
print >> edges, '<edge id="cyberout" fromnode="cyber%s" tonode="cyberend" nolanes="2" spread_type="center"/>' % row 
print >> edges, '<edge id="-cyberout" fromnode="cyberend" tonode="cyber%s" nolanes="2" spread_type="center"/>' % row 


print >> nodes, "</nodes>"
nodes.close()
print >> edges, "</edges>"
edges.close()

numBusses = TOTAL_CAPACITY / BUS_CAPACITY
print >> routes, """    <vehicle id="b" type="cybercar" depart="0" period="100" repno="%s" arrivalpos="10000">
        <route edges="cyberin"/>
    </vehicle>""" % (numBusses - 1)
totalSlots = 2 * DOUBLE_ROWS * SLOTS_PER_ROW
if occupied < totalSlots:
    print >> routes, """    <vehicle id="v" type="car" depart="10" period="20" repno="%s" arrivalpos="10000">
        <route edges="mainin"/>
    </vehicle>""" % (totalSlots-occupied-1)
if occupied > 0:
    print >> routes, """    <vehicle id="p" type="person" depart="10" period="15" repno="%s" arrivalpos="10000">
        <route edges="footfairin"/>
    </vehicle>
</routes>""" % (occupied*CAR_CAPACITY-1)
routes.close()

routes = open("%s_cyber.rou.xml" % PREFIX, "w")
print >> routes, """<routes>
    <vehicle id="c" type="cybercar" depart="50" period="100" repno="%s" arrivalpos="10000">
        <route edges="cyberin"/>
    </vehicle>
</routes>""" % (TOTAL_CAPACITY / CYBER_CAPACITY - numBusses - 1)
routes.close()

os.system("netconvert -n %s.nod.xml -e %s.edg.xml --add-internal-links -o %s.net.xml" % (PREFIX, PREFIX, PREFIX))

config = open("%s.sumo.cfg" % PREFIX, "w")
print >> config, """<c>
    <net-file>%s.net.xml</net-file>
    <route-files>%s.rou.xml</route-files>
    <use-internal-links>x</use-internal-links>
    <no-step-log>x</no-step-log>
    <time-to-teleport>0</time-to-teleport>
    <remote-port>%s</remote-port>
</c>""" % (PREFIX, PREFIX, PORT)
config.close()

config = open("%s_cyber.sumo.cfg" % PREFIX, "w")
print >> config, """<c>
    <net-file>%s.net.xml</net-file>
    <route-files>%s.rou.xml,%s_cyber.rou.xml</route-files>
    <use-internal-links>x</use-internal-links>
    <no-step-log>x</no-step-log>
    <time-to-teleport>0</time-to-teleport>
    <remote-port>%s</remote-port>
</c>""" % (PREFIX, PREFIX, PREFIX, PORT)
config.close()