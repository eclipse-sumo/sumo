#!/usr/bin/python
"""
@file    sort_routes.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2011-07-14
@version $Id$

This script sorts the vehicles in the given route file by their depart time
SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2007-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sys
import os
import re
from xml.dom import pulldom

def sort_departs(routefilename, outfile):
    routes_doc = pulldom.parse(sys.argv[1])
    vehicles = []
    for event, parsenode in routes_doc:
        if event == pulldom.START_ELEMENT and (parsenode.localName == 'vehicle' or parsenode.localName == 'flow'):
            vehicle = parsenode # now we know it's a vehicle or a flow
            routes_doc.expandNode(vehicle)
            if (parsenode.localName == 'vehicle'):
                depart = int(float(vehicle.getAttribute('depart')))
                vehicles.append((depart, vehicle.toprettyxml(indent="", newl="")))
            elif (parsenode.localName == 'flow'):
                begin = int(float(vehicle.getAttribute('begin')))
                vehicles.append((begin, vehicle.toprettyxml(indent="", newl="")))
    print('read %s vehicles.' % len(vehicles))
    vehicles.sort()
    for depart, vehiclexml in vehicles:
        outfile.write(" "*4)
        outfile.write(vehiclexml)
        outfile.write("\n")
    print('wrote %s vehicles.' % len(vehicles))

if len(sys.argv) != 2:
    print("Supply exactly one argument: the routefile to be sorted")
    sys.exit(1)

routefilename = sys.argv[1]
outfile = open(routefilename + ".sorted", 'w')
for line in open(routefilename):
    outfile.write(line)
    if '<routes' in line:
        break
sort_departs(routefilename, outfile)
outfile.write('</routes>')
outfile.close()

