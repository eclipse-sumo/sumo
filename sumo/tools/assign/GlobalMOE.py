# python
# This script is for the incremental traffic assignment with a sumo-based traffic network
# The Dijkstra algorithm is applied for searching the shortest paths.
# The necessary inputs include:
# SUMO net (.net.xml), OD-matrix file, zone connectors file, parameter file, OD-zone file and CR-curve file (for defining link cost functions)

import os, string, sys, datetime, random, math

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Vertex, Edge, Vehicle                                          # import the characteristics of Vertices, Edges and paths
from network import Net, NetDetectorFlowReader, ZoneConnectionReader, VehInformationReader   # import characteristices of network and read xml files for retriving netork and zone connectors data

## Program execution
optParser = OptionParser()

optParser.add_option("-c", "--zonalconnection-file", dest="confile",              #  XML file: containing the link information connectings to the respective traffic zone
                     help="read OD Zones from FILE (mandatory)", metavar="FILE")
optParser.add_option("-n", "--net-file", dest="netfile",                          # XML file: containing the network geometric (link-node) information
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-x", "--vehinform-file", dest="vehfile",                    # txt file: containing the control paramenter for incremental traffic assignment
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
    
net = Net()                                                                    # generate the investigated network from the respective SUMO-network

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
        
    veh.speed = veh.travellength / veh.traveltime                              # unit: m/s
    Totaltime += veh.traveltime                                                # unit: s
    Totallength += veh.travellength                                            # unit: m
    Totalspeed += veh.speed

avetime = Totaltime / Totalveh
avelength = Totallength / Totalveh
avespeed = Totalspeed / Totalveh

foutveh = file(options.outputfile, 'w')                                           # initialize the file for recording the routes
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
