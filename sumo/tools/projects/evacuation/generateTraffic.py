# -*- coding: cp1252 -*-
#!/usr/bin/env python
"""
@file   generateTraffic.py
@author Martin Taraz
@date 2015-09-09

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os
import sys
from optparse import OptionParser
from xml.sax import parse

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
sys.path.append(os.path.join(SUMO_HOME, "tools"))

import edgesInDistricts as EID
import subprocess
import sumolib


def generate(netFile, mappedSiteFile, intersectionFile, tripFile):
    #EdgesInDistricts
    #evakuierungsbereich
    options = OptionParser()
    options.maxspeed = 1000.
    options.complete = False
    options.internal = False
    options.vclass = None
    options.assign_from = True
    options.verbose = False
    net = sumolib.net.readNet(netFile)
    reader = EID.DistrictEdgeComputer(net)
    polyReader = sumolib.shapes.polygon.PolygonReader(True)
    parse(intersectionFile, polyReader)
    reader.computeWithin(polyReader.getPolygons(), options)
    reader.writeResults("edgesInIntersections.taz.xml", False)

    #Evakuierungsziele
    reader = EID.DistrictEdgeComputer(net)
    polyReader = sumolib.shapes.polygon.PolygonReader(True)
    parse(mappedSiteFile, polyReader)
    reader.computeWithin(polyReader.getPolygons(), options)
    reader.writeResults("evacuationsiteEdges.taz.xml", False)
    print("EdgesInDistricts - done")

    #O/D Matrix
    import xml.etree.cElementTree as ET
    Districts = ET.ElementTree(file=intersectionFile)
    root = Districts.getroot()
    EVA = ET.ElementTree(file='evacuationsiteEdges.taz.xml')
    EV = EVA.getroot()
    EV.remove(EV[0])
    with open('ODMatrix.fma','w') as odm:
        odm.write('$OR \n* From-Tome To-Time \n1.00 2.00\n* Factor \n1.00\n')
        for elem in root.findall("./poly"):
            for ESite in EV.findall("./*"):
                CarAmount = str(int(float(elem.attrib["inhabitants"])/float(3*(len(EV.findall("./*"))-1))))
                odm.write(elem.attrib["id"] + '\t' + ESite.attrib["id"] + '\t' + CarAmount + '\n')
    print("OD Matrix - done")

    #OD2TRIPS
    od2t = sumolib.checkBinary('od2trips')
    od2tOptions = [od2t, '--no-step-log', '-d', odm.name, '-n', 'edgesInIntersections.taz.xml,evacuationsiteEdges.taz.xml', '-o', tripFile]
    subprocess.call(od2tOptions)
