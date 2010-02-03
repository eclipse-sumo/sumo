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

if len(sys.argv)<5:
    print >> sys.stderr, "Call: osmBuild.py <PREFIX> <TILES_NUMBER> <SUMONET> <OUTPUT>"
    sys.exit()


prefix = sys.argv[1]#"bs"
num = int(sys.argv[2])#20

call = ""
for i in range(0,num):
	if i!=0:
		call = call + ","
	call = call + prefix + str(i) + "_" + str(num) + ".osm.xml"
call1 = "%SUMO%\\polyconvert --osm-files " + call + " -n " + sys.argv[3] + " --osm.keep-full-type -o " + sys.argv[4] + " -v --typemap osm_type_map.xml"
print call1
os.system(call1)

	