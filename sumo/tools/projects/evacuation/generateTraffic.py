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
def generate():
    #EdgesInDistricts
    #evakuierungsbereich
    options = OptionParser()
    options.maxspeed = 1000.
    options.complete = False
    options.internal = False
    options.vclass = None
    options.assign_from = True
    options.verbose = False
    options.net_file = "testserveroutput.net.xml"
    options.taz_files = "Schnittflachen.xml"
    options.output = "EdgesInSchnittflachen.taz.xml"
    options.weighted = False
    nets = options.net_file.split(",")
    reader = EID.DistrictEdgeComputer(sumolib.net.readNet(nets[0]))
    tazFiles = nets + options.taz_files.split(",")
    polyReader = sumolib.shapes.polygon.PolygonReader(True)
    for tf in tazFiles:
        parse(tf, polyReader)
    reader.computeWithin(polyReader.getPolygons(), options)
    reader.writeResults(options.output, options.weighted)

    #Evakuierungsziele
    options.net_file = "testserveroutput.net.xml"
    options.taz_files = "ConvertedEvaSite.poly.xml"
    options.output = "EvacuationsiteEdges.taz.xml"
    options.weighted = False
    nets = options.net_file.split(",")
    reader = EID.DistrictEdgeComputer(sumolib.net.readNet(nets[0]))
    tazFiles = nets + options.taz_files.split(",")
    polyReader = sumolib.shapes.polygon.PolygonReader(True)
    for tf in tazFiles:
        parse(tf, polyReader)
    reader.computeWithin(polyReader.getPolygons(), options)
    print(polyReader.getPolygons())
    reader.writeResults(options.output, options.weighted)
    print("EdgesInDistricts - done")

    #O/D Matrix(V-fortmat)
    outString = '$OR \n* From-Tome To-Time \n1.00 2.00\n* Factor \n1.00\n'
    import xml.etree.cElementTree as ET
    Districts = ET.ElementTree(file='Schnittflachen.xml')
    root = Districts.getroot()
    EVA = ET.ElementTree(file='EvacuationsiteEdges.taz.xml')
    EV = EVA.getroot()
    EV.remove(EV[0])     
    for elem in root.findall("./poly"):
        for ESite in EV.findall("./*"):
            CarAmount = str(int(float(elem.attrib["inhabitants"])/float(3*(len(EV.findall("./*"))-1))))
            outString += elem.attrib["id"] + '\t' + ESite.attrib["id"] + '\t' + CarAmount +'\n'
    ODM = open('ODMatrix.xml','w')                     
    ODM.write(outString)
    ODM.close()
    print("OD Matrix - done")

    #OD2TRIPS
    od2t = sumolib.checkBinary('od2trips')
    od2tOptions = [od2t ,'-d','ODMATRIX.xml', '-n', 'EdgesInSchnittflachen.taz.xml,EvacuationsiteEdges.taz.xml', '--output-prefix', 'TRAFFIC' , '-o' , '.xml']
    subprocess.call(od2tOptions)
if __name__ == "__main__":
    generate()
