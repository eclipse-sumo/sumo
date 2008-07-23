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
import os
from constants import *

nodes = open("%s.nod.xml" % PREFIX, "w")
print >> nodes, "<nodes>"
edges = open("%s.edg.xml" % PREFIX, "w")
print >> edges, "<edges>"
routes = open("%s.rou.xml" % PREFIX, "w")
print >> routes, """<routes>
    <vtype id="car" length="6" maxspeed="50" color="0.6,0.6,0.6"/>
    <vtype id="person" length=".5" maxspeed="5" color="1,0.2,0.2"/>
    <vtype id="cybercar" length="7" maxspeed="15" color="0,1,0"/>"""
#streets
print >> nodes, '<node id="in" x="-100" y="0"/>' 
print >> edges, '<edge id="mainin" fromnode="in" tonode="main0" nolanes="2" spread_type="center"/>'
for row in range(DOUBLE_ROWS+1):
    nodeID = "main%s" % row
    x = row * ROW_DIST
    print >> nodes, '<node id="%s" x="%s" y="0"/>' % (nodeID, x) 
    if row > 0:
        edgeID = "main%sto%s" % (row-1, row)
        print >> edges, '<edge id="%s" fromnode="main%s" tonode="main%s" nolanes="2" spread_type="center"/>' % (edgeID, row-1, row) 
    for slot in range(SLOTS_PER_ROW):
        slotID = "row%sslot%s" % (row, slot)
        print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (slotID, x, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="row%s%sto%s" fromnode="%s" tonode="%s" nolanes="2" spread_type="center"/>' % (row, nodeID, slot, nodeID, slotID)
        print >> edges, '<edge id="-row%s%sto%s" fromnode="%s" tonode="%s" nolanes="2" spread_type="center"/>' % (row, nodeID, slot, slotID, nodeID)
        if row < DOUBLE_ROWS:
            print >> nodes, '<node id="%sr" x="%s" y="%s"/>' % (slotID, x+SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
            print >> edges, '<edge id="%srp" fromnode="%s" tonode="%sr" spread_type="center"/>' % (slotID, slotID, slotID)
            print >> edges, '<edge id="-%srp" fromnode="%sr" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        if row > 0:
            print >> nodes, '<node id="%sl" x="%s" y="%s"/>' % (slotID, x-SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
            print >> edges, '<edge id="%slp" fromnode="%s" tonode="%sl" spread_type="center"/>' % (slotID, slotID, slotID)
            print >> edges, '<edge id="-%slp" fromnode="%sl" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        nodeID = slotID
print >> nodes, '<node id="out" x="%s" y="0"/>' % (x+100) 
print >> edges, '<edge id="mainout" fromnode="main%s" tonode="out" nolanes="2" spread_type="center"/>' % row 

#footpaths
y = (SLOTS_PER_ROW+1) * SLOT_WIDTH
print >> nodes, '<node id="foot" x="-100" y="%s"/>' % y
print >> edges, '<edge id="footin" fromnode="foot" tonode="foot0" speed="5" spread_type="center"/>'
for row in range(DOUBLE_ROWS):
    nodeID = "foot%s" % row
    x = row * ROW_DIST + ROW_DIST/2
    print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (nodeID, x, y) 
    if row > 0:
        edgeID = "foot%sto%s" % (row-1, row)
        print >> edges, '<edge id="%s" fromnode="foot%s" tonode="foot%s" speed="5" spread_type="center"/>' % (edgeID, row-1, row)
    for slot in reversed(range(SLOTS_PER_ROW)):
        slotID = "foot%sslot%s" % (row, slot)
        print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (slotID, x, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="row%s%sto%s" fromnode="%s" tonode="%s" speed="5" spread_type="center"/>' % (row, nodeID, slot, nodeID, slotID)
        print >> nodes, '<node id="%sr" x="%s" y="%s"/>' % (slotID, x+SLOT_FOOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%srp" fromnode="%s" tonode="%sr" spread_type="center"/>' % (slotID, slotID, slotID)
        print >> edges, '<edge id="-%srp" fromnode="%sr" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        print >> nodes, '<node id="%sl" x="%s" y="%s"/>' % (slotID, x-SLOT_FOOT_LENGTH, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="%slp" fromnode="%s" tonode="%sl" spread_type="center"/>' % (slotID, slotID, slotID)
        print >> edges, '<edge id="-%slp" fromnode="%sl" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        nodeID = slotID 
        print >> routes, """\
    <vehicle id="p%sr" type="person" depart="0" period="1" repno="4" arrivalpos="10000">
        <route edges="%srp -%srp"/>
    </vehicle>
    <vehicle id="p%sl" type="person" depart="0" period="1" repno="4" arrivalpos="10000">
        <route edges="%slp -%slp"/>
    </vehicle>""" % (slotID, slotID, slotID, slotID, slotID, slotID)
print >> nodes, '<node id="footend" x="%s" y="%s"/>' % (x+100, y) 
print >> edges, '<edge id="footout" fromnode="foot%s" tonode="footend" speed="5" spread_type="center"/>' % row 

#cybercar (automated bus)
y = (SLOTS_PER_ROW+2) * SLOT_WIDTH
print >> nodes, '<node id="cyber" x="-100" y="%s"/>' % y
print >> edges, '<edge id="cyberin" fromnode="cyber" tonode="cyber0" nolanes="2" spread_type="center"/>' 
cyberroute = "cyberin"
for row in range(DOUBLE_ROWS):
    nodeID = "cyber%s" % row
    x = row * ROW_DIST + ROW_DIST/2
    print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (nodeID, x, y) 
    if row > 0:
        edgeID = "cyber%sto%s" % (row-1, row)
        print >> edges, '<edge id="%s" fromnode="cyber%s" tonode="cyber%s" nolanes="2" spread_type="center"/>' % (edgeID, row-1, row)
        cyberroute += " " + edgeID 
print >> nodes, '<node id="cyberend" x="%s" y="%s"/>' % (x+100, y) 
print >> edges, '<edge id="cyberout" fromnode="cyber%s" tonode="cyberend" nolanes="2" spread_type="center"/>' % row 
cyberroute += " cyberout" 


print >> nodes, "</nodes>"
nodes.close()
print >> edges, "</edges>"
edges.close()
print >> routes, """    <vehicle id="v" type="car" depart="1" period="10" repno="%s" arrivalpos="10000">
        <route edges="mainin main0to1"/>
    </vehicle>
    <vehicle id="c" type="cybercar" depart="1" period="100" repno="10" arrivalpos="10000">
        <route edges="%s"/>
    </vehicle>
</routes>""" % (2*DOUBLE_ROWS*SLOTS_PER_ROW-1, cyberroute)
routes.close()

os.system("netconvert -n %s.nod.xml -e %s.edg.xml --add-internal-links -o %s.net.xml" % (PREFIX, PREFIX, PREFIX))

config = open("%s.sumo.cfg" % PREFIX, "w")
print >> config, """<c>
    <net-file>%s.net.xml</net-file>
    <route-files>%s.rou.xml</route-files>
    <use-internal-links>x</use-internal-links>
    <time-to-teleport>0</time-to-teleport>
    <remote-port>%s</remote-port>
</c>""" % (PREFIX, PREFIX, PORT)
config.close()