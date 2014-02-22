#!/usr/bin/env python
"""
@file    visum_convertEdgeTypes.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2009-05-27
@version $Id$


This script converts edge type definitions (STRECKENTYP) into their
 SUMO-representation.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys, os

if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " <VISUM-NET> <OUTPUT>"
    sys.exit()
print "Reading VISUM..."
fd = open(sys.argv[1])
fdo = open(sys.argv[2], "w")
fdo.write("<types>\n")
parsingTypes = False
for line in fd:
    if parsingTypes:
        if line[0]=='*' or line[0]=='$' or line.find(";")<0:
            parsingTypes = False
            continue

        values = line.strip().split(";")
        map = {}
        for i in range(0, len(attributes)):
            map[attributes[i]] = values[i]
        fdo.write('   <type id="' + map["nr"])
        fdo.write('" priority="' + str(100-int(map["rang"])))
        fdo.write('" numLanes="' + map["anzfahrstreifen"])
        fdo.write('" speed="' + str(float(map["v0iv"])/3.6))
        fdo.write('"/>\n')

    if line.find("$STRECKENTYP")==0:
        parsingTypes = True
        attributes = line[len("$STRECKENTYP:"):].lower().split(";")
fdo.write("</types>\n")

