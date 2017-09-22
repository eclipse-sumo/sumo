#!/usr/bin/env python
"""
@file    makeSumolympicWalkers.py
@author  Leonhard Luecken
@date    2016-11-25
@version $Id$

python script for building pedestrian demand

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""
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
    xml_string += '    <person depart="%f" id="p%d">\n' % (departTime, i)
    xml_string += '        <walk departPos="%f" edges="%s %s"/>\n' % (
        departPos, startEdge, endEdge)
    xml_string += '    </person>\n'
xml_string += "</routes>\n"
with open(outfile, "w") as f:
    f.write(xml_string)
