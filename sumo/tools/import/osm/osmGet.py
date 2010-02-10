#!/usr/bin/env python
"""
@file    osmGet.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id$

Retrieves an area from OpenStreetMap.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import os,sys

if len(sys.argv)<3:
	print >> sys.stderr, "Call: osmGet.py <PREFIX> <BOUNDING_BOX> <TILES_NUMBER>"
	sys.exit()

prefix = sys.argv[1]#"bs"
box = sys.argv[2]#"10.38,52.22,10.74,52.34"
num = int(sys.argv[3])#20

boxV = box.split(",")
bb = float(boxV[0])
ee = float(boxV[2])

if bb>ee or boxV[1]>boxV[3]:
	print >> sys.stderr, "Box minimum is larger than box maximum."
	sys.exit()


b = bb
for i in range(0,num):
	e = b + (ee-bb) / float(num)
	cbox = str(b) + "," + boxV[1] + "," + str(e) + "," + boxV[3]
	call = "wget.exe http://api.openstreetmap.org/api/0.6/map?bbox=" + cbox + " -O " + prefix + str(i) + "_" + str(num) + ".osm.xml"
	print call
	os.system(call)
	b = e

	