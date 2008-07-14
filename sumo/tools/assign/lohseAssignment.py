#!/usr/bin/env python
"""
@file    lohseAssignment.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-03-28
@version $Id$

This script is for executing the traffic assignment with the assignment model proposed by Professor Dieter Lohse.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime, math
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Predecessor, Vertex, Edge, Path, Vehicle
from network import Net, NetworkReader, DistrictsReader, ExtraSignalInformationReader
                                                  
from inputs import getMatrix, getConnectionTravelTime                 
from outputs import timeForInput, outputODZone, outputNetwork, outputStatistics, sortedVehOutput
from assign import doSUEAssign, doLohseStopCheck, doSUEVehAssign

# for measuring the required time for reading input files
inputreaderstart = datetime.datetime.now()  
# initialize the file for recording the process and the errors
foutlog = file('Lohse_log.txt', 'w')
foutlog.write('The stochastic user equilibrium traffic assignment will be executed with the Lohse model.\n')
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
optParser.add_option("-k", "--k-shortest-paths", dest="kPaths", type="int",
                     default=4, help="number of the paths should be found at the first iteration")
optParser.add_option("-i", "--max-sue-iteration", dest="maxsueiteration", type="int",
                     default=20, help="maximum number of the assignment iterations")
optParser.add_option("-t", "--sue-tolerance", dest="sueTolerance", type="float",
                     default=0.01, help="difference tolerance for the convergence in the c-logit model")
optParser.add_option("-a", "--alpha-value", dest="alpha", type="float",
                     default=0.15, help="alpha value to determine the commonality factor")
optParser.add_option("-g", "--gamma-value", dest="gamma", type="float",
                     default=1., help="gamma value to determine the commonality factor")
optParser.add_option("-U", "--under-value", dest="under", type="float",
                     default=0.15, help="parameter 'under' to determine auxiliary link cost"
optParser.add_option("-p", "--upper-value", dest="upper", type="float",
                     default=0.5, help="parameter 'upper' to determine auxiliary link cost")
optParser.add_option("-x", "--parameter-1-value", dest="v1", type="float",
                     default=2.5, help="parameter 'v1' to determine auxiliary link cost")
optParser.add_option("-y", "--parameter-2-value", dest="v2", type="float",
                     default=4., help="parameter 'v2' to determine auxiliary link cost")
optParser.add_option("-z", "--parameter-3-value", dest="v3", type="float",
                     default=0.002, help="parameter 'v3' to determine auxiliary link cost")
optParser.add_option("-c", "--convergence-parameter-1", dest="cvg1", type="float",
                     default=1., help="parameter 'cvg1' to calculate the convergence value")
optParser.add_option("-o", "--convergence-parameter-2", dest="cvg2", type="float",
                     default=1., help="parameter 'cvg2' to calculate the convergence value")
optParser.add_option("-q", "--convergence-parameter-3", dest="cvg3", type="float",
                     default=10., help="parameter 'cvg3' to calculate the convergence value")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-b", "--debug", action="store_true", dest="debug",
                     default=False, help="debug the program")
                                      
(options, args) = optParser.parse_args()

if not options.netfile or not options.confile or not options.mtxpsfile:
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
    foutlog.write('number of total startVertices:%s\n' %len(net._startVertices))
    foutlog.write('number of total endVertices:%s\n' %len(net._endVertices))
        
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
            edge.helpacttime = edge.freeflowtime
    # calculate link travel time for all district connectors 
    getConnectionTravelTime(net._startVertices, net._endVertices)
            
    foutlog.write('- Initial calculation of link parameters : done.\n')
    # the required time for reading the network     
    timeForInput(inputreaderstart)                                                 
    
    if options.debug:
        outputNetwork(net)
    
    # initialization
    matrixCounter = 0
    vehID = 0
    MatrixSum = 0.0
    lohse = True
    checkKPaths = False
    if kPaths > 1:
        checkKPaths = True

    net.initialPathSet()
    # initialize the map for recording the number of the assigned vehicles
    AssignedVeh = {}
    # initialize the map for recording the number of the assigned trips                                                  
    AssignedTrip = {}                                                   
    for startVertex in net._startVertices:
        AssignedVeh[startVertex]={}                                                              
        AssignedTrip[startVertex]={} 
        for endVertex in net._endVertices:
            AssignedVeh[startVertex][endVertex] = 0
            AssignedTrip[startVertex][endVertex] = 0.
    
    starttime = datetime.datetime.now()
    # initialize the file for recording the routes
    foutroute = open('routes.rou.xml', 'w')
    print >> foutroute, """<?xml version="1.0"?>
<!-- generated on %s by $Id$ -->
<routes>""" % starttime
    
    for counter in range (0, len(matrices)):
        # delete all vehicle information related to the last matrix for saving the disk space
        net._vehicles = []
        matrix = matrices[counter]
        matrixCounter += 1

    
        matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum, begintime = getMatrix(net, options.verbose, matrix, MatrixSum)
        
        departtime = (begintime + int(counter)) * 3600
        
        if options.verbose:
            print 'number of the analyzed matrices:', len(matrices)
            print 'Begintime:', begintime, "O'Clock"
            print 'departtime', departtime
            print 'Matrix und OD Zone already read for Interval', counter
            print 'CurrentMatrixSum:', CurrentMatrixSum
        
        foutlog.write('Reading matrix and O-D zones: done.\n')
        foutlog.write('Matrix und OD Zone already read for Interval:%s\n' %counter)
        foutlog.write('CurrentMatrixSum:%s\n' %CurrentMatrixSum)
        foutlog.write('number of current startVertices:%s\n' %len(startVertices))
        foutlog.write('number of current endVertices:%s\n' %len(endVertices))
        
        for edgeID in net._edges:
            edge = net._edges[edgeID]
            edge.flow = 0.
            edge.helpflow = 0.
            edge.getActualTravelTime(options.curvefile, lammda)
            edge.resetLohseParameter()
            edge.helpacttime = edge.freeflowtime
            
        # the number of origins, the umber of destinations and the number of the OD pairs
        origins = len(startVertices)                                    
        dests = len(endVertices)
        ODpairs = origins * dests
        
        # output the origin and destination zones and the number of effective OD pairs
        if options.debug:
#            outputODZone(startVertices, endVertices, Pshort_EffCells, Plong_EffCells, Truck_EffCells)
            outputODZone(startVertices, endVertices, Pshort_EffCells, matrixCounter)  
    
        # initialization    
        iter_outside = 1
        newRoutes = 1
        stable = False
        first =True
        # traffic Assignment based on the Lohse Model
        while newRoutes > 0:
            iter_inside = 1
            foutlog.write('- SUE iteration:%s\n' %iter_outside)
            # Generate the effective routes als intital path solutions, when considering k shortest paths (k is defined by the user.)
            if checkKPaths:
                checkPathStart = datetime.datetime.now() 
                newRoutes = net.calcKPaths(options.verbose, newRoutes, kPaths, startVertices, endVertices, matrixPshort)
                checkPathEnd = datetime.datetime.now() - checkPathStart
                foutlog.write('- Time for finding the k-shortest paths: %s\n' %checkPathEnd)
                foutlog.write('- Finding the k-shortest paths for each OD pair: done.\n')
                
                if options.verbose:
                    print 'iter_outside:', iter_outside
                    print 'KPaths:', kPaths 
                    print 'number of new routes:', newRoutes
            elif not checkKPaths and iter_outside == 1 and counter == 0:
                print 'search for the new path'
                newRoutes = net.findNewPath(startVertices, endVertices, newRoutes, matrixPshort, lohse)
            
            checkKPaths = False
            
            if options.verbose:
                print 'iter_outside:', iter_outside
                print 'number of new routes:', newRoutes
            
            stable = False
            while not stable:
                if options.verbose:
                    print 'iter_inside:', iter_inside
                doSUEAssign(options.curvefile, options.verbose, net, alpha, gamma, lammda, under, uppper, v1, v2, v3, sueTolerance, maxsueiteration, startVertices, endVertices, matrixPshort, iter_inside, lohse, first)
                stable = doLohseStopCheck(net, options.verbose, stable, iter_inside, maxsueiteration, cvg1, cvg2, cvg3, foutlog)
                iter_inside += 1

                if (float(departtime)/3600.) < 10. or counter < 5:
                    newRoutes = net.findNewPath(startVertices, endVertices, newRoutes, matrixPshort, lohse)
                else:
                    newRoutes = 0

                if options.verbose:
                    print 'stable:', stable
                
            first = False    
            iter_outside += 1

            if newRoutes < 5 and iter_outside > 10:
                newRoutes = 0
                
            if iter_outside > maxsueiteration:
                print 'The max. number of iterations is reached!'
                foutlog.write('The max. number of iterations is reached!\n')
                foutlog.write('The number of new routes and the parameter stable will be set to zero and True respectively.\n')
                print 'newRoutes:', newRoutes 
                stable = True
                newRoutes = 0

        # update the path choice probability and the path flows as well as generate vehicle data 	
        vehID = doSUEVehAssign(options.verbose, net, counter, matrixPshort, startVertices, endVertices, AssignedVeh, AssignedTrip, vehID, lohse)
        # output vehicle releasing time and vehicle route 
        sortedVehOutput(net._vehicles, departtime, foutroute)
    
    foutroute.write('</routes>\n')
    foutroute.close()

    # output the global performance indices
    assigntime = outputStatistics(net, starttime, len(matrices))
    
    foutlog.write('- Assignment is completed and all vehicular information is generated. ')
    foutlog.close()
                                              
    print 'Duration for traffic assignment:', assigntime
    print 'Total assigned vehicles:', vehID
    print 'Total number of the assigend trips:', MatrixSum

    
main()
