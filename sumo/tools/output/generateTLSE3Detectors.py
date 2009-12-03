#!/usr/bin/env python
"""
@file    generateTLSE3Detectors.py 
@author  Daniel.Krajzewicz@dlr.de
@date    2007-10-25
@version $Id: generateTLSE3Detectors.py 7660 2009-09-03 09:09:50Z behrisch $

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
import sys, os
from xml.sax import saxutils, make_parser, handler
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet
from optparse import OptionParser

# initialise 
optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="Net-File to work with", type="string")
optParser.add_option("-l", "--detector-length", dest="detectorLength",
                     help="length of the detector in meters (-1 for maximal length)", type="int", default=250)
optParser.add_option("-d", "--distance-to-TLS", dest="distanceToTLS",
                     help="distance of the detector to the traffic light in meters", type="float", default=.1)
optParser.add_option("-f", "--frequency", dest="frequency",
                     help="frequency", type="int", default=60)
optParser.add_option("-o", "--output", dest="output",
                     help="the name of the file to write the detector definitions into", type="string", default="e3.add.xml")
optParser.add_option("-r", "--results-file", dest="results",
                     help="the name of the file the detectors write their output into", type="string", default="e3output.xml")

optParser.set_usage('\ngenerateTLSE3Detectors.py -n inputs\\pasubio\\pasubio.net.xml -l 250 -d .1 -f 60')
# parse options
(options, args) = optParser.parse_args()
if not options.netfile:
    print "Missing arguments"
    optParser.print_help()
    exit()

netfile = options.netfile
det_length_input = options.detectorLength
distToTLS = options.distanceToTLS
freq = options.frequency

[dirname, filename] = os.path.split(netfile)
prefix = filename.split('.')[0]
dest = os.path.join(dirname, options.output)
detectorFile = open(dest, "w")

print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(netfile)
net = net.getNet()

print >> detectorFile, "<additional>"
for tls in net._tlss:
    edges = tls.getEdges()
    for e in edges:
        id = tls._id + "_" + e._id
        print >> detectorFile, '    <e3-detector id="e3_%s" freq="%s" file="%s">' % (id, freq, options.results)
        iedges = net.getDownstreamEdges(e, det_length_input, True)
        for ie in iedges:
            pos = ie[1]
            if ie[3]:
                pos = .1
            for l in ie[0]._lanes:
                print >> detectorFile, '        <det_entry lane="%s" pos="%s"/>' % (l.getID(), pos)
        for l in e._lanes:
            print >> detectorFile, '        <det_exit lane="%s" pos="-.1"/>' % (l.getID())
        print >> detectorFile, '    </e3-detector>'
print >> detectorFile, "</additional>"
detectorFile.close()
