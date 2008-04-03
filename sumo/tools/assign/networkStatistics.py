#!/usr/bin/env python
"""
@file    networkStatistics.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-02-27
@version $Id$

This script is to calculate the global performance indices according to the SUMO-based simulation results.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, string, sys, datetime, random, math

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Vertex, Edge, Vehicle
from network import Net, NetworkReader, DistrictsReader, VehInformationReader


optParser = OptionParser()
optParser.add_option("-t", "--tripinfo-file", dest="tripinfofile",
                     help="read vehicle information from FILE (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output-file", dest="outputfile", default="Global_MOE.txt",
                     help="write output to FILE", metavar="FILE")  
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")

(options, args) = optParser.parse_args()

if not options.tripinfofile:
    optParser.print_help()
    sys.exit()
parser = make_parser()

vehicles = []
parser.setContentHandler(VehInformationReader(vehicles))
parser.parse(options.tripinfofile)

totaltime = 0.
totallength = 0.
totalspeed= 0.
totalstoptime = 0.

for veh in vehicles:
    veh.speed = veh.travellength / veh.traveltime                              
    totaltime += veh.traveltime
    totallength += veh.travellength
    totalspeed += veh.speed
    totalstoptime += veh.waittime

foutveh = file(options.outputfile, 'w')
foutveh.write('average vehicular travel time(s) = the sum of all vehicular travel time / the number of vehicles\n')
foutveh.write('average vehicular travel length(m) = the sum of all vehicular travel length / the number of vehicles\n')
foutveh.write('average vehicular travel speed(m/s) = the sum of all vehicular travel speed / the number of vehicles\n')
foutveh.write('Total number of vehicles:%s\n' % len(vehicles))
foutveh.write('Total waiting time(s):%s, ' % totalstoptime)    
foutveh.write('average vehicular waiting time(s):%s\n' % (totalstoptime / len(vehicles)))
foutveh.write('Total travel time(s):%s, ' % totaltime)    
foutveh.write('average vehicular travel time(s):%s\n' % (totaltime / len(vehicles)))
foutveh.write('Total travel length(m):%s, ' % totallength)
foutveh.write('average vehicular travel length(m):%s\n' % (totallength / len(vehicles)))
foutveh.write('average vehicular travel speed(m/s):%s\n' % (totalspeed / len(vehicles)))
foutveh.close()
        
print 'the avg. travel speed and the avg. travel time are calculated!'
