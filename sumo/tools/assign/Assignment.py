#!/usr/bin/env python
"""
@file    Assignment.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-03-28
@version $Id: Assignment.py 5774 2008-07-14 13:10:31Z behrisch $

This script is for executing the traffic assignment.
Three assignment models are available:
- incremental
- c-logit
- lohse

The c-logit model are set as default.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime, math
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Predecessor, Vertex, Edge, Path, Vehicle
from network import Net, NetworkReader, DistrictsReader, ExtraSignalInformationReader
from dijkstra import dijkstra 
from inputs import getMatrix, getConnectionTravelTime                 
from outputs import timeForInput, outputODZone, outputNetwork, outputStatistics, sortedVehOutput
from assign import doSUEAssign, doLohseStopCheck, doSUEVehAssign, doIncAssign
from tables import updateCurveTable

# for measuring the required time for reading input files
inputreaderstart = datetime.datetime.now()  

optParser = OptionParser()
optParser.add_option("-m", "--matrix-file", dest="mtxpsfile", 
                     help="read OD matrix for passenger vehicles(long dist.) from FILE (mandatory)", metavar="FILE")
optParser.add_option("-n", "--net-file", dest="netfile",                          
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-d", "--district-file", dest="confile",
                     help="read OD Zones from FILE (mandatory)", metavar="FILE")  
optParser.add_option("-s", "--extrasignal-file", dest="sigfile",
                     help="read extra/updated signal timing plans from FILE", metavar="FILE")
optParser.add_option("-u", "--crCurve-file", dest="curvefile",
                     help="read parameters used in cost functions from FILE", metavar="FILE")  
optParser.add_option("-k", "--k-shortest-paths", dest="kPaths", type="int",
                     default=4, help="number of the paths should be found at the first iteration")
optParser.add_option("-i", "--max-sue-iteration", dest="maxiteration", type="int",
                     default=20, help="maximum number of the assignment iterations")
optParser.add_option("-t", "--sue-tolerance", dest="sueTolerance", type="float",
                     default=0.001, help="difference tolerance for the convergence in the c-logit model")
optParser.add_option("-a", "--alpha", dest="alpha", type="float",
                     default=0.15, help="alpha value to determine the commonality factor")
optParser.add_option("-g", "--gamma", dest="gamma", type="float",
                     default=1., help="gamma value to determine the commonality factor")
optParser.add_option("-l", "--lambda", dest="lamda", type="float",
                     default=0.3, help="lambda value to determine the penalty time due to queue")
optParser.add_option("-U", "--under-value", dest="under", type="float",
                     default=0.15, help="parameter 'under' to determine auxiliary link cost")
optParser.add_option("-p", "--upper-value", dest="upper", type="float",
                     default=0.5, help="parameter 'upper' to determine auxiliary link cost")
optParser.add_option("-x", "--parameter-1", dest="v1", type="float",
                     default=2.5, help="parameter 'v1' to determine auxiliary link cost in the lohse model")
optParser.add_option("-y", "--parameter-2", dest="v2", type="float",
                     default=4., help="parameter 'v2' to determine auxiliary link cost in the lohse model")
optParser.add_option("-z", "--parameter-3", dest="v3", type="float",
                     default=0.002, help="parameter 'v3' to determine auxiliary link cost in the lohse model")
optParser.add_option("-c", "--convergence-parameter-1", dest="cvg1", type="float",
                     default=1., help="parameter 'cvg1' to calculate the convergence value in the lohse model")
optParser.add_option("-o", "--convergence-parameter-2", dest="cvg2", type="float",
                     default=1., help="parameter 'cvg2' to calculate the convergence value in the lohse model")
optParser.add_option("-q", "--convergence-parameter-3", dest="cvg3", type="float",
                     default=10., help="parameter 'cvg3' to calculate the convergence value in the lohse model")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-b", "--debug", action="store_true", dest="debug",
                     default=False, help="debug the program")
optParser.add_option("-e", "--type", dest="type", type="choice",
                     choices=('clogit', 'lohse', 'incremental'),
                     default="clogit", help="type of assignment [default: %default]")
                                      
(options, args) = optParser.parse_args()

if not options.netfile or not options.confile or not options.mtxpsfile:
    optParser.print_help()
    sys.exit()

foutlog = file('%s_log.txt' % options.type, 'w')
foutlog.write('The stochastic user equilibrium traffic assignment will be executed with the %s model.\n' % options.type)
foutlog.write('All vehicular releasing times are determined randomly(uniform).\n')

def main():   
    matrices = options.mtxpsfile.split(",")    
    parser = make_parser()
    
    if options.verbose:
        print "Reading net"
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

    if options.curvefile:
        updateCurveTable(options.curvefile)

    for edgeID in net._edges: 
        edge = net._edges[edgeID]
        net.removeUTurnEdge(edge)
        if edge.numberlane > 0.:
            edge.getCapacity()
            edge.getAdjustedCapacity(net)
            edge.getActualTravelTime(options.lamda) 
            edge.helpacttime = edge.freeflowtime

    # calculate link travel time for all district connectors 
    getConnectionTravelTime(net._startVertices, net._endVertices)
            
    foutlog.write('- Initial calculation of link parameters : done.\n')
    # the required time for reading the network     
    timeForInput(inputreaderstart)                                                 
    
    if options.debug:
        outputNetwork(net)
        
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
        
    # initialization
    vehID = 0
    matrixSum = 0.0
    lohse = (options.type == "lohse")
    incremental = (options.type == "incremental")
    checkKPaths = False

    if not incremental and options.kPaths > 1:
        checkKPaths = True
    if not incremental:
        net.initialPathSet()

    starttime = datetime.datetime.now()
    # initialize the file for recording the routes
    foutroute = open('routes.rou.xml', 'w')
    print >> foutroute, """<?xml version="1.0"?>
<!-- generated on %s by $Id: SUEAssignment.py 5774 2008-07-14 13:10:31Z behrisch $ -->
<routes>""" % starttime
    
    for counter, matrix in enumerate(matrices):  #for counter in range (0, len(matrices)):
        # delete all vehicle information related to the last matrix for saving the disk space
        vehicles = []
    
        matrixPshort, startVertices, endVertices, Pshort_EffCells, matrixSum, CurrentMatrixSum, begintime = getMatrix(net, options.verbose, matrix, matrixSum)
        departtime = begintime * 3600
        
        if options.verbose:
            print 'the analyzed matrices:', counter
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
            edge.actualtime = edge.freeflowtime
            
            if lohse:
                edge.resetLohseParameter()
                edge.helpacttime = edge.freeflowtime
                
        # the number of origins, the umber of destinations and the number of the OD pairs
        origins = len(startVertices)                                    
        dests = len(endVertices)
        ODpairs = origins * dests
        
        # output the origin and destination zones and the number of effective OD pairs
        if options.debug:
            outputODZone(startVertices, endVertices, Pshort_EffCells, counter) # matrixCounter)  
        
        if incremental:
            print 'begin the incremental assignment!'
            iter = 0
            options.lamda = 0.
            while iter < options.maxiteration:
                foutlog.write('- Current iteration(not executed yet):%s\n' %iter)
                iter += 1
                for start, startVertex in enumerate(startVertices):
                    D,P = dijkstra(net, startVertex)                                                                      
                    vehID = doIncAssign(vehicles, options.verbose, options.maxiteration, endVertices, start, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID)
                
                for edgeID in net._edges:                                                   
                    edge = net._edges[edgeID]
                    edge.getActualTravelTime(options.lamda)
                    
        else:
            print 'begin the', options.type, " assignment!"   
            # initialization for the clogit and the lohse assignment model
            iter_outside = 1
            newRoutes = 1
            stable = False
            first = True
            # begin the traffic Assignment
            while newRoutes > 0:
                foutlog.write('- SUE iteration:%s\n' %iter_outside)
                # Generate the effective routes als intital path solutions, when considering k shortest paths (k is defined by the user.)
                if checkKPaths:
                    checkPathStart = datetime.datetime.now() 
                    newRoutes = net.calcKPaths(options.verbose, options.kPaths, newRoutes, startVertices, endVertices, matrixPshort)
                    checkPathEnd = datetime.datetime.now() - checkPathStart
                    foutlog.write('- Time for finding the k-shortest paths: %s\n' %checkPathEnd)
                    foutlog.write('- Finding the k-shortest paths for each OD pair: done.\n')
                    if options.verbose:
                        print 'iter_outside:', iter_outside
                        print 'number of k shortest paths:', options.kPaths 
                        print 'number of new routes:', newRoutes
                
                elif not checkKPaths and iter_outside == 1 and counter == 0:
                    print 'search for the new path'
                    newRoutes = net.findNewPath(startVertices, endVertices, newRoutes, matrixPshort, lohse)
                
                checkKPaths = False
                
                if options.verbose:
                    print 'iter_outside:', iter_outside
                    print 'number of new routes:', newRoutes
                
                stable = False
                iter_inside = 1
                while not stable:
                    if options.verbose:
                        print 'iter_inside:', iter_inside
                    stable = doSUEAssign(net, options, startVertices, endVertices, matrixPshort, iter_inside, lohse, first)
                    # The matrixPlong and the matrixTruck should be added when considering the long-distance trips and the truck trips.
                    if lohse:
                        stable = doLohseStopCheck(net, options, stable, iter_inside, options.maxiteration, foutlog)

                    iter_inside += 1
    
                    if options.verbose:
                        print 'stable:', stable
                    
    #            if (float(departtime)/3600.) < 10. or counter < 5:
                newRoutes = net.findNewPath(startVertices, endVertices, newRoutes, matrixPshort, lohse)
    #            else:
    #                newRoutes = 0
                first = False    
                iter_outside += 1
    
                if newRoutes < 2 and iter_outside > int((options.maxiteration)/2):
                    newRoutes = 0
                    
                if iter_outside > options.maxiteration:
                    print 'The max. number of iterations is reached!'
                    foutlog.write('The max. number of iterations is reached!\n')
                    foutlog.write('The number of new routes and the parameter stable will be set to zero and True respectively.\n')
                    print 'newRoutes:', newRoutes 
                    stable = True
                    newRoutes = 0
    
            # update the path choice probability and the path flows as well as generate vehicle data 	
            vehID = doSUEVehAssign(net, vehicles, options, counter, matrixPshort, startVertices, endVertices, AssignedVeh, AssignedTrip, vehID, lohse)

       # output the generated vehicular releasing times and routes, based on the current matrix
        sortedVehOutput(vehicles, departtime, foutroute)
    
    foutroute.write('</routes>\n')
    foutroute.close()

    # output the global performance indices
    assigntime = outputStatistics(net, starttime, len(matrices))
    
    foutlog.write('- Assignment is completed and all vehicular information is generated. ')
    foutlog.close()
                                              
    print 'Duration for traffic assignment:', assigntime
    print 'Total assigned vehicles:', vehID
    print 'Total number of the assigend trips:', matrixSum
    
main()
