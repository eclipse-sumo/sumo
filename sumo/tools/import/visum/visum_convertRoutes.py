#!/usr/bin/env python
"""
@file    visum_convertRoutes.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007-02-21
@version $Id$

This script converts given VISUM-routes for a given SUMO-network.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""


import os, string, sys, StringIO, random
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
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



# initialise 
optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="Net-File to work with", type="string", default=None)
optParser.add_option("-r", "--visum-routes", dest="routes",
                     help="The VISUM-routes files to parse", type="string", default=None)
optParser.add_option("-o", "--output", dest="output",
                     help="Name of the file to write", type="string", default=None)
optParser.add_option("-b", "--begin", dest="begin",
                     help="The begin time of the routes to generate", type="int", default=0)
optParser.add_option("-e", "--end", dest="end",
                     help="The end time (+1) of the routes to generate", type="int", default=3600)
optParser.add_option("-p", "--prefix", dest="prefix",
                     help="ID prefix to use", type="string", default=None)
optParser.add_option("-t", "--type", dest="type",
                     help="The type to use for vehicles", type="string", default=None)
optParser.add_option("-u", "--uniform", dest="uniform",
                     help="Whether departures shall be distributed uniform in each interval", action="store_true", default=False)
optParser.add_option("-l", "--timeline", dest="timeline",
                     help="Percentages over a day", type="string", default=None)

optParser.set_usage('\nvisum_convertRoutes.py -n visum.net.xml -r visum_routes.att -o visum.rou.xml')
# parse options
(options, args) = optParser.parse_args()
if not options.netfile or not options.routes or not options.output:
    print "Missing arguments"
    optParser.print_help()
    exit()

print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(options.netfile)
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
fd = open(options.routes)
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

    if line.find("$PRTPATHLINK:")==0 or line.find("$IVTEILWEG:")==0:
        attributes = line[line.find(":")+1:].strip().lower().split(";")
        for i in range(0, len(attributes)):
            if attributes[i]=="qbeznr":
                attributes[i] = "origzoneno"
            if attributes[i]=="zbeznr":
                attributes[i] = "destzoneno"
            if attributes[i]=="iv-weg\\bel(ap)":
                attributes[i] = "prtpath\\vol(ap)"
            if attributes[i]=="wegind":
                attributes[i] = "pathindex"
            if attributes[i]=="vonknotnr":
                attributes[i] = "fromnodeno"
            if attributes[i]=="nachknotnr":
                attributes[i] = "tonodeno"
            if attributes[i]=="strnr":
                attributes[i] = "linkno"
        parse = True

addRouteChecking(ok);
fd.close()

print " " + str(found) + " routes found (" + str(foundN) + " vehs)"
print " " + str(missing) + " routes missing (" + str(missingN) + " vehs)"

timeline = None
# apply timeline
if options.timeline:
    timeline = []
    nNo = 0
    vals = options.timeline.split(",")
    sum = 0
    for v in vals:
        timeline.append(float(v))
        sum += float(v)
    print sum
    if len(timeline)!=24:
        print "The timeline must have 24 entries"
        sys.exit()
    nRoutes = []

# convert to vehicles
print "Generating vehicles..."
emissions = []
begin = options.begin
end = options.end

if not timeline:
    for r in routes:
        for i in range(0, int(r[1])):
            if options.uniform:
                t = float(begin) + float(end-begin) / float(r[1]) * float(i)
            else:
                t = float(begin) + float(end-begin) * random.random()
            emissions.append( ( int(t), r[0]+"__"+str(i), r[2] ) )
else:
    for r in routes:
        left = 0.
        tbeg = 0
        j = 0
        for t in timeline:
            fno = (float(r[1])+left) * t / 100.
            no = int(fno)
            left += fno - no
            if left>=1.:
                left -= 1
                no += 1
            for i in range(0, no):
                if options.uniform:
                    t = tbeg + float(3600) / float(r[1]) * float(i)
                else:
                    t = tbeg + float(3600) * random.random()
                emissions.append( ( int(t), r[0]+"__"+str(j), r[2] ) )
                j = j + 1
            nNo += no
            tbeg += 3600
    print " " + str(nNo) + " vehicles after applying timeline"


# sort emissions
print "Sorting routes..."
emissions.sort(sorter(0))

# save emissions
print "Writing routes..."
fdo = open(options.output, "w")
fdo.write("<routes>\n")
for emission in emissions:
    fdo.write('    <vehicle id="')
    if options.prefix:
        fdo.write(options.prefix + "_")
    fdo.write(emission[1] + '" depart="' + str(emission[0]) + '"')
    if options.type:
        fdo.write(' type="' + options.type + '"')
    fdo.write('><route edges="' + emission[2] + '"/></vehicle>\n')
fdo.write("</routes>\n")    
fdo.close()
print " " + str(len(emissions)) + " vehicles written"

