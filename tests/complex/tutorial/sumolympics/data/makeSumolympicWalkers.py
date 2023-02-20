#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    makeSumolympicWalkers.py
# @author  Leonhard Luecken
# @date    2016-11-25

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
