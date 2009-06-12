#!/usr/bin/env python
"""
@file    visum_convertRoutes.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-06-12
@version $Id: visum_convertRoutes.py 6858 2009-02-25 13:27:57Z behrisch $


Converts given VISUM-routes into SUMO-routes

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""


import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../../lib"))
import sumonet


found = 0
foundN = 0
missing = 0
missingN = 0
route = ""
routes = []
no = 0
id = ""
route = ""

def addRouteChecking(ok):
    global found
    global foundN
    global missing
    global missingN
    global routes
    global route
    global id
    global no
    route = route.strip()
    if route!="":
        # append already built route
        if ok:
            routes.append((id, no, route))
            found = found + 1
            foundN = foundN + no
        else:
            missing = missing + 1
            missingN = missingN + no
        route = ""


def sorter(idx):
	def t(i, j):
		if i[idx] < j[idx]:
			return -1
		elif i[idx] > j[idx]:
			return 1
		else:
			return 0


    
if len(sys.argv) < 4:
    print "Usage: " + sys.argv[0] + " <SUMO_NET> <VISUM_ROUTES> <OUTPUT> [<BEGIN_TIME> <END_TIME>]"
    sys.exit()
print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
net = net.getNet()

# initialise nodes/edge map
emap = {}
for e in net._edges:
    if e._from._id not in emap:
        emap[e._from._id] = {}
    if e._to._id not in emap[e._from._id]:
        emap[e._from._id][e._to._id] = e._id

# fill with read values
print "Reading routes..."
parse = False
ok = True
fd = open(sys.argv[2])
for line in fd:
    if line.find("$")==0 or line.find("*")==0 or line.find(";")<0:
        parse = False
        addRouteChecking(ok);

    if parse:
        values = line.strip().split(";")
        amap = {}
        for i in range(0, len(attributes)):
            amap[attributes[i]] = values[i]
        if amap["origzoneno"]!="":
            # route begin (not the route)
            addRouteChecking(ok);
            id = amap["origzoneno"] + "_" + amap["destzoneno"] + "_" + amap["pathindex"]
            no = float(amap["prtpath\\vol(ap)"])
            route = " "
            ok = True
        else:
            if not ok:
                continue;
            fromnode = amap["fromnodeno"]
            tonode = amap["tonodeno"]
            link = amap["linkno"]
            if fromnode not in emap:
                if no!=0:
                    print "Missing from-node '" + fromnode + "'; skipping"
                ok = False
                continue
            if tonode not in emap[fromnode]:
                if no!=0:
                    print "No connection between from-node '" + fromnode + "' and to-node '" + tonode + "'; skipping"
                ok = False
                continue
            edge = emap[fromnode][tonode]
            if link!=edge and link!=edge[1:]:
                if no!=0:
                    print "Mismatching edge '" + link + "' (from '" + fromnode + "', to '" + tonode + "'); skipping"
                ok = False
                continue
            route = route + edge + " "

    if line.find("$PRTPATHLINK:")==0:
        attributes = line[13:].strip().lower().split(";")
        parse = True

addRouteChecking(ok);
fd.close()

print " " + str(found) + " routes found (" + str(foundN) + " vehs)"
print " " + str(missing) + " routes missing (" + str(missingN) + " vehs)"

# convert to vehicles
print "Generating vehicles..."
beg = 0
end = 3600
if len(sys.argv) == 6:
    beg = int(sys.argv[4])
    end = int(sys.argv[5])
emissions = []
for r in routes:
    for i in range(0, int(r[1])):
        t = float(beg) + float(end-beg) / float(r[1]) * float(i)
        emissions.append( ( int(t), r[0]+"__"+str(i), r[2] ) )

# sort emissions
print "Sorting routes..."
emissions.sort(sorter(0))

# save emissions
print "Writing routes..."
fdo = open(sys.argv[3], "w")
fdo.write("<routes>\n")
for emission in emissions:
	fdo.write('    <vehicle id="' + emission[1] + '" depart="' + str(emission[0]) + '"><route edges="' + emission[2] + '"/></vehicle>\n');
fdo.write("</routes>\n")	
fdo.close()

