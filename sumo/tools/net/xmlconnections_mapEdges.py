#!/usr/bin/env python
"""
@file    xmlconnections_mapEdges.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id$

Reads edge id replacements from "edgemap.txt"; the format of this file is
 <OLD_EDGE_ID>-><NEW_EDGE_ID>
Reads the given connections file <CONNECTIONS> and replaces old edge names by new.
The result is written to <CONNECTIONS>.mod.xml

Call: xmlconnections_mapEdges.py <CONNECTIONS>

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import sys

if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <CONNECTIONS>"
    sys.exit()

# read map
mmap = {}
fdi = open("edgemap.txt")
for line in fdi:
	if line.find("->")<0:
		continue
	(orig, dest) = line.strip().split("->")
	dest = dest.split(",")
	mmap[orig] = dest
fdi.close()

fdi = open(sys.argv[1])
fdo = open(sys.argv[1]+".mod.xml", "w")
for line in fdi:
	for orig in mmap:
		line = line.replace('from="'+orig+'"', 'from="'+mmap[orig][-1]+'"')
		line = line.replace('to="'+orig+'"', 'to="'+mmap[orig][0]+'"')
	fdo.write(line)
fdi.close()
fdo.close()
		
