#!/usr/bin/env python
"""
@file    incrementalAssignment.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-10-25
@version $Id$

This script is for the incremental traffic assignment with a sumo-based traffic network.
The Dijkstra algorithm is applied for searching the shortest paths.
The necessary inputs include:
- SUMO net (.net.xml),
- OD-matrix file,
- districts file,
- parameter file and
- CRcurve file (for defining link cost functions)

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import sys, datetime

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Vertex, Edge, Vehicle
from network import Net, NetworkReader, DistrictsReader, ExtraSignalInformationReader
from dijkstra import dijkstra 
from inputs import getMatrix, getConnectionTravelTime
from outputs import timeForInput, outputODZone, outputNetwork, sortedVehOutput, outputStatistics, vehPoissonDistr
from assign import doIncAssign

# for measuring the required time for reading input files
inputreaderstart = datetime.datetime.now()                                        
# initialize the file for recording the process and the errors
foutlog = file('incrementalAssign_log.txt', 'w')
foutlog.write('The traffic assignment will be executed with the incremental assignment.\n')
foutlog.write('All vehicular releasing times are determined randomly(uniform).\n')

optParser = OptionParser()

optParser.add_option("-m", "--matrix-file", dest="mtxpsfile",      
                     help="read OD matrix for passenger vehilces(long dist.) from FILE (mandatory)", metavar="FILE")
optParser.add_option("-n", "--net-file", dest="netfile", 
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-u", "--curve-file", dest="curvefile", default="CRcurve.txt",
                     help="read CRcurve from FILE", metavar="FILE")
optParser.add_option("-d", "--district-file", dest="confile",  
                     help="read OD Zones from FILE (mandatory)", metavar="FILE")
optParser.add_option("-s", "--extrasignal-file", dest="sigfile",
                     help="read extra/updated signal timing plans from FILE", metavar="FILE")
optParser.add_option("-i", "--iterations", dest="iteration", type="int",
                     default=20, help="number of the assignment iterations")                    
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-b", "--debug", action="store_true", dest="debug",
                     default=False, help="debug the program")
                     
(options, args) = optParser.parse_args()

if not options.netfile:
    optParser.print_help()
    sys.exit()
    
def main():
    matrices = options.mtxpsfile.split(",")
    parser = make_parser()
    
    if options.verbose:
        print "Reading net"
        
    # generate the investigated network from the respective SUMO-network    
    net = Net()                                                             
    
    parser.setContentHandler(NetworkReader(net))
    parser.parse(options.netfile)
    
    parser.setContentHandler(DistrictsReader(net))
    parser.parse(options.confile)
    
    if options.sigfile:
        parser.setContentHandler(ExtraSignalInformationReader(net))
        parser.parse(options.sigfile)
    
    foutlog.write('- Reading network: done.\n')
    
    if options.verbose:
        print len(net._edges), "edges read"
    
    for edgeID in net._edges: 
        edge = net._edges[edgeID]
        net.removeUTurnEdge(edge)
        if edge.numberlane > 0.:
            edge.getCapacity()
            edge.getCRcurve()
            edge.getDefaultCapacity()
            edge.getAdjustedCapacity(net)
            edge.getActualTravelTime(options.curvefile, lammda)
    # calculate link travel time for all district connectors 
    getConnectionTravelTime(net._startVertices, net._endVertices)    
        
    foutlog.write('- Initial calculation of link parameters : done.\n')
    
    # the required time for reading the network     
    timeForInput(inputreaderstart)
                                                    
    if options.debug:
        outputNetwork(net)
       
    # initialization
    MatrixCounter = 0
    vehID = 0
    MatrixSum = 0.0
    
    # initialize the map for recording the number of the assigned vehicles
    AssignedVeh = {}
    # initialize the map for recording the number of the assigned trips                                                
    AssignedTrip = {}                                               
    
    for startVertex in net._startVertices:
        AssignedVeh[startVertex] = {}
        AssignedTrip[startVertex] = {}
    
        for endVertex in net._endVertices:
            AssignedVeh[startVertex][endVertex] = 0
            AssignedTrip[startVertex][endVertex] = 0.
            
    starttime = datetime.datetime.now() 
    foutroute = open('routes.rou.xml', 'w')                                           # initialize the file for recording the routes
    print >> foutroute, """<?xml version="1.0"?>
<!-- generated on %s by $Id$ -->
<routes>""" % starttime
    for counter, matrix in enumerate(matrices):
        # delete all vehicle information related to the last matrix for saving the disk space
        net._vehicles = []
      
        matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum, begintime = getMatrix(net, options.verbose, matrix, MatrixSum)

        departtime = (begintime + int(counter)) * 3600
        
        if options.verbose:
            print 'number of the analyzed matrices:', len(matrices)
            print 'Begin Time:', begintime, "o'Clock"
            print 'Matrix:', counter
            print 'departtime', departtime
            print 'Matrix und OD Zone already read for Interval', counter
            
        foutlog.write('Reading matrix and O-D zones: done.\n')
        
        for edgeID in net._edges:
            edge = net._edges[edgeID]
            edge.flow = 0.
            edge.actualtime = edge.freeflowtime
        
        # the number of origins, the number of destinations and the number of the OD pairs
        origins = len(startVertices)                                             
        dests = len(endVertices)
        ODpairs = origins * dests
        
    # output the origin and destination zones and the number of effective OD pairs
        if options.debug:
            outputODZone(startVertices, endVertices, Pshort_EffCells, MatrixCounter) 
  
        iter = 0
        # incremental traffic assignment
        while iter < iteration:
            foutlog.write('- Current iteration(not executed yet):%s\n' %iter)
            iter += 1
            for start, startVertex in enumerate(startVertices):
                D,P = dijkstra(startVertex)                                                                      
                vehID = doIncAssign(net, options.verbose, iteration, endVertices, start, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID)
            
            for edgeID in net._edges:                                                   
                edge = net._edges[edgeID]
                edge.getActualTravelTime(options.curvefile, lammda)

        # output the generated vehicular releasing times and routes, based on the current matrix
        sortedVehOutput(net._vehicles, departtime, foutroute)
    
    foutroute.write('</routes>\n')
    foutroute.close()

    # output the global performance indices
    assigntime= outputStatistics(net, starttime, len(matrices))

    foutlog.write('- Assignment is completed and the all vehicular information is generated. ')
    foutlog.close()
                                     
    print 'Total duration for traffic assignment:', assigntime
    print 'Total assigned vehicles:', vehID
    print 'Total number of the assigend trips:', MatrixSum

main()