#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    makeSumolympicWalkers.py
# @author  Leonhard Luecken
# @date    2016-11-25
# @version $Id$

outfile = "sumolympicWalks.rou.xml"
startEdge = "beg"
endEdge = "end"
# Startzeit
departTime = 0.
# Startposition
departPos = -30.
# Anzahl Fussgaenger
numberTrips = 200
# Generiere XML Datei
xml_string = "<routes>\n"
for i in range(numberTrips):
    xml_string += '    <person depart="%f" id="p%d" departPos="%f">\n' % (departTime, i, departPos)
    xml_string += '        <walk edges="%s %s"/>\n' % (startEdge, endEdge)
    xml_string += '    </person>\n'
xml_string += "</routes>\n"
with open(outfile, "w") as f:
    f.write(xml_string)
