#!/usr/bin/env python
"""
@file    randomTrips.py
@author  Michael.Behrisch@dlr.de
@date    2010-03-06
@version $Id$

Generates random trips for the given network.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""

import os, sys, random, bisect, datetime, subprocess
from optparse import OptionParser
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet

def randomEdge(edges, cumWeights):
    r = random.random() * cumWeights[-1]
    return edges[bisect.bisect(cumWeights, r)]

optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                        help="define the net file (mandatory)")
optParser.add_option("-o", "--output-trip-file", dest="tripfile",
                     default="trips.trips.xml", help="define the output trip filename")
optParser.add_option("-r", "--route-file", dest="routefile",
                     help="generates route file with duarouter")
optParser.add_option("-t", "--trip-id-prefix", dest="tripprefix",
                     default="t", help="prefix for the trip ids")
optParser.add_option("-a", "--trip-parameters", dest="trippar",
                     default="", help="additional trip parameters")
optParser.add_option("-b", "--begin", type="int", default=0, help="begin time")
optParser.add_option("-e", "--end", type="int", default=3600, help="end time")
optParser.add_option("-p", "--period", type="int", default=1, help="repetition period")
optParser.add_option("-s", "--seed", type="int", help="random seed")
optParser.add_option("-l", "--length", action="store_true",
                     default=False, help="weight edge probability by length")
optParser.add_option("-L", "--lanes", action="store_true",
                     default=False, help="weight edge probability by number of lanes")
optParser.add_option("-v", "--verbose", action="store_true",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.netfile:
    optParser.print_help()
    sys.exit()

net = sumonet.readNet(options.netfile)
if options.seed:
    random.seed(options.seed)
probs=[]
total = 0
for edge in net._edges:
    prob = 1
    if options.length:
        prob *= edge.getLength()
    if options.lanes:
        prob *= edge.getLaneNumber()
    total += prob
    probs.append(total)

idx = 0
fouttrips = file(options.tripfile, 'w')
print >> fouttrips, """<?xml version="1.0"?>
<!-- generated on %s by $Id$ -->
<tripdefs>""" % datetime.datetime.now()
for depart in range(options.begin, options.end, options.period):
    label = "%s%s" % (options.tripprefix, idx)
    sourceEdge = randomEdge(net._edges, probs)
    sinkEdge = randomEdge(net._edges, probs)
    print >> fouttrips, '    <tripdef id="%s" depart="%s" from="%s" to="%s" %s/>' \
                        % (label, depart, sourceEdge.getID(), sinkEdge.getID(), options.trippar)
    idx += 1
fouttrips.write("</tripdefs>")
fouttrips.close()

if options.routefile:
    subprocess.call(['duarouter', '-n', options.netfile, '-t', options.tripfile, '-o', options.routefile])
