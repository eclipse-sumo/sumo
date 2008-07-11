#!/usr/bin/env python
"""
@file    clogit.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-01-25
@version $Id$

This script is for executing the traffic assignment with the C-Logit model.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime, math
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Predecessor, Vertex, Edge, Path, Vehicle, TLJunction, Signalphase
from network import Net, NetworkReader, DistrictsReader, ExtraSignalInformationReader
                                                  
from inputs import getParameter, getMatrix, getConnectionTravelTime                 
from outputs import timeForInput, outputODZone, outputNetwork, outputStatistics, sortedVehOutput, vehPoissonDistr
from assign import doSUEAssign, doSUEVehAssign # doCLogitAssign, doCLogitVehAssign

# for measuring the required time for reading input files
inputreaderstart = datetime.datetime.now()  
# initialize the file for recording the process and the errors
foutlog = file('CLogit_log.txt', 'w')
foutlog.write('The stochastic user equilibrium traffic assignment will be executed with the C-logit model.\n')
foutlog.write('All vehicular releasing times are determined randomly(uniform).\n')

optParser = OptionParser()

optParser.add_option("-m", "--matrix-file", dest="mtxpsfile", 
                     help="read OD matrix for passenger vehilces(long dist.) from FILE (mandatory)", metavar="FILE")
#optParser.add_option("-k", "--matrixpl-file", dest="mtxplfile", 
#                     help="read OD matrix for passenger vehilces(long dist.) from FILE (mandatory)", metavar="FILE")  
#optParser.add_option("-t", "--matrixt-file", dest="mtxtfile",                     
#                     help="read OD matrix for trucks from FILE (mandatory)", metavar="FILE")  
optParser.add_option("-n", "--net-file", dest="netfile",                          
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-p", "--parameter-file", dest="parfile", 
                     help="read assignment parameters from FILE (mandatory)", metavar="FILE")
optParser.add_option("-u", "--curve-file", dest="curvefile", default="CRcurve.txt",
                     help="read CRcurve from FILE", metavar="FILE")
optParser.add_option("-d", "--district-file", dest="confile",
                     help="read OD Zones from FILE (mandatory)", metavar="FILE")  
optParser.add_option("-s", "--extrasignal-file", dest="sigfile",
                     help="read extra/updated signal timing plans from FILE", metavar="FILE")  
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
            edge.getActualTravelTime(options.curvefile) 
    # calculate link travel time for all district connectors 
    getConnectionTravelTime(net._startVertices, net._endVertices)
            
    foutlog.write('- Initial calculation of link parameters : done.\n')
    # the required time for reading the network     
    timeForInput(inputreaderstart)                                                 
    
    if options.debug:
        outputNetwork(net)
    
    # read the control parameters for the SUE traffic assignment:  
    #   - beta, gamma: used in the function of the communality factor
    #   - lammda: used in the capacity contraint for calculating travel time penality
    #   - devi: Deviation of the travel time among  the effective routes
    #   - theta: perception deviation of the shortest travel time among drivers
    #   - suek1 and suek2 are two parameter for calculating the alpha, 
    #     the moving-size sequence for updating link flows, which is applied in MSA,
    #     where alpha(n) = suek1/(suek2+n), where n : the number of iterations; 
    #     suek1 a positive constant and determins the magnitude of the move; 
    #     suek2: a nonnegative constant and acts as an offset to the starting step.
    #     The suek1 and suek2 will not be used and alpha is set to be 1/n as default.
    #   - NumEffPath: maximum number (k) of effective routes
    #   - maxSUEIteration: maximum number of the SUE iterations
    #   - sueTolerance: the difference of link flows between the two consequent iterations
    #   - KPaths: the number of k shortest paths for each OD pair
    #   - vehicle releasing method: 0: uniform randomly; 1: Poisson (arcontrol[10])
    #   - Number of the periods (Parcontrol[11])
    #   - Begin time (Parcontrol[12])
    
    Parcontrol = getParameter(options.parfile)
    if options.verbose:
        print 'Control parameters(Parcontrol):', Parcontrol
    
    beta = float(Parcontrol[0])
    gamma = float(Parcontrol[1])
    lammda = float(Parcontrol[2])
    devi = float(Parcontrol[3])
    theta = float(Parcontrol[4])
    suek1 = float(Parcontrol[5])
    suek2 = float(Parcontrol[6])
    maxSUEIteration = int(Parcontrol[7])
    sueTolerance = float(Parcontrol[8])
    KPaths = int(Parcontrol[9])
    
    begintime = int(Parcontrol[(len(Parcontrol)-1)])
    
    if options.verbose:
        print 'number of the analyzed matrices:', len(matrices)
        print 'Begintime:', begintime, "O'Clock"
    
    # initialization
    matrixCounter = 0
    vehID = 0
    MatrixSum = 0.0
    lohse = False
    checkKPaths = False
    if KPaths > 1:
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
        if options.verbose:
            print 'Matrix: ', matrixCounter
        departtime = (begintime + int(counter)) * 3600
        if options.verbose:
            print 'departtime', departtime  
    
        matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum = getMatrix(net, options.verbose, matrix, MatrixSum)
       
        if options.verbose:
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
        # execute the traffic assignment based on the C-Logit Model 
        while newRoutes > 0:
            iter_inside = 1
            foutlog.write('- SUE iteration:%s\n' %iter_outside)
            # Generate the effective routes als intital path solutions, when considering k shortest paths (k is defined by the user.)
            if checkKPaths:
                checkPathStart = datetime.datetime.now() 
                newRoutes = net.calcKPaths(options.verbose, newRoutes, KPaths, startVertices, endVertices, matrixPshort)
                checkPathEnd = datetime.datetime.now() - checkPathStart
                foutlog.write('- Time for finding the k-shortest paths: %s\n' %checkPathEnd)
                foutlog.write('- Finding the k-shortest paths for each OD pair: done.\n')
                
                if options.verbose:
                    print 'KPaths:', KPaths 
                    print 'number of new routes:', newRoutes
            elif not checkKPaths and iter_outside == 1 and counter == 0:
                newRoutes = net.findNewPath(startVertices, endVertices, newRoutes, matrixPshort, lohse)
            
            checkKPaths = False
            
            if options.verbose:
                print 'number of new routes:', newRoutes
              
            stable = False            
            while not stable:
                if options.verbose:
                    print 'iteration (inside):', iter_inside
                    print 'SUE Tolerance:', sueTolerance
                        
                # The matrixPlong and the matrixTruck should be added when considering the long-distance trips and the truck trips.
                stable = doSUEAssign(options.curvefile, options.verbose, Parcontrol, net, startVertices, endVertices, matrixPshort, iter_inside, lohse, first)
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
                
            if iter_outside > maxSUEIteration:
                print 'The max. number of iterations is reached!'
                foutlog.write('The max. number of iterations is reached!\n')
                foutlog.write('The number of new routes and the parameter stable will be set to zero and True respectively.\n')
                print 'newRoutes:', newRoutes 
                stable = True
                newRoutes = 0
    
    # update the path choice probability and the path flows as well as generate vehicle data 	
#        AssignedVeh, AssignedTrip, vehID = doCLogitVehAssign(net, options.verbose, counter, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh, AssignedTrip, vehID)
        vehID = doSUEVehAssign(options.verbose, net, counter, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh, AssignedTrip, vehID, lohse)          
    # output vehicle releasing time and vehicle route 
        sortedVehOutput(net._vehicles, departtime, foutroute)
    
    foutroute.write('</routes>\n')
    foutroute.close()

    # output the global performance indices
    assigntime = outputStatistics(net, starttime, Parcontrol)
    
    # output the number of vehicles in the given time interval(10 sec) accoding to the Poisson distribution
    vehPoissonDistr(net, Parcontrol, begintime)
    
    foutlog.write('- Assignment is completed and all vehicular information is generated. ')
    foutlog.close()
                                              
    print 'Duration for traffic assignment:', assigntime
    print 'Total assigned vehicles:', vehID
    print 'Total number of the assigend trips:', MatrixSum

    
main()