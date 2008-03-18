#!/usr/bin/env python
"""
@file    networkStatistics.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-02-27
@version $Id: networkStatistics.py 2008-03-18 $

This script is to calculate the global performance indices according to the SUMO-based simulation results.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, string, sys, datetime, random, math

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Vertex, Edge, Vehicle
from network import Net, NetDetectorFlowReader, ZoneConnectionReader, VehInformationReader


optParser = OptionParser()
optParser.add_option("-d", "--district-file", dest="confile",
                     help="read OD Zones from FILE (mandatory)", metavar="FILE")
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-x", "--vehinform-file", dest="vehfile",
                     help="read vehicle information from FILE (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output-file", dest="outputfile", default="Global_MOE.txt",
                     help="write output to FILE", metavar="FILE")  
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")

(options, args) = optParser.parse_args()

if not options.netfile:
    optParser.print_help()
    sys.exit()
parser = make_parser()

if options.verbose:
    print "Reading net"
    
net = Net()

Netreader = NetDetectorFlowReader(net)
parser.setContentHandler(Netreader)
parser.parse(options.netfile)

Zonereader = ZoneConnectionReader(net)
parser.setContentHandler(Zonereader)
parser.parse(options.confile)

Vehreader =VehInformationReader(net)
parser.setContentHandler(Vehreader)
parser.parse(options.vehfile)

if options.verbose:
    print len(net._edges), "edges read"

Totalveh = 0.
Totaltime = 0.
Totallength = 0.
Totalspeed= 0.

for veh in net._vehicles:
    Totalveh += 1
    veh.traveltime = float(veh.arrival - veh.depart)
  
    for link in veh.route:
        veh.travellength += net.getEdge(link).length
    # unit: speed - m/s; traveltime - s; travel length - m    
    veh.speed = veh.travellength / veh.traveltime                              
    Totaltime += veh.traveltime
    Totallength += veh.travellength
    Totalspeed += veh.speed

avetime = Totaltime / Totalveh
avelength = Totallength / Totalveh
avespeed = Totalspeed / Totalveh

foutveh = file(options.outputfile, 'w')
foutveh.write('average vehicular travel time(s) = the sum of all vehicular travel time / the number of vehicles\n')
foutveh.write('average vehicular travel length(m) = the sum of all vehicular travel length / the number of vehicles\n')
foutveh.write('average vehicular travel speed(m/s) = the sum of all vehicular travel speed / the number of vehicles\n')
foutveh.write('Total number of vehicles:%s\n' %Totalveh)
foutveh.write('Total travel time(s):%s, ' %Totaltime)    
foutveh.write('average vehicular travel time(s):%s\n' %avetime)
foutveh.write('Total travel length(m):%s, ' %Totallength)
foutveh.write('average vehicular travel length(m):%s\n' %avelength)
foutveh.write('average vehicular travel speed(m/s):%s\n' %avespeed)
foutveh.close()
        
print 'Ave travel speed and ave travel time are calculated!'
