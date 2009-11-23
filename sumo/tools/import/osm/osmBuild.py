#!/usr/bin/env python
"""
@file    osmGet.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id: osmGet.py 6858 2009-02-25 13:27:57Z behrisch $

Retrieves an area from OpenStreetMap.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import os,sys

if len(sys.argv)<4:
    print >> sys.stderr, "Call: osmBuild.py <PREFIX> <TILES_NUMBER> <OUTPUT> [(all|road|passenger),ramps,tls,nointernal]"
    sys.exit()


prefix = sys.argv[1]#"bs"
num = int(sys.argv[2])#20
add = ""
if len(sys.argv)>4:
    defs = sys.argv[4].split(",")
    if "road" in defs:
        add = "--remove-edges.by-vclass rail_slow,rail_fast,bicycle,pedestrian "
    elif "passenger" in defs:
        add = "--remove-edges.by-vclass hov,taxi,bus,delivery,transport,lightrail,cityrail,rail_slow,rail_fast,motorcycle,bicycle,pedestrian "
    if "ramps" in defs:
        add = add + "--guess-ramps "
    if "tls" in defs:
        add = add + "--guess-tls "
    if "nointernal" in defs:
        add = add + "--no-internal-links "
 

call = ""
for i in range(0,num):
    if i!=0:
        call = call + ","
    call = call + prefix + str(i) + "_" + str(num) + ".osm.xml"
call = "%SUMO%\\netconvert --osm " + call + " --proj.utm -o " + sys.argv[3] + " -v " + add
print call
os.system(call)

    