"""
@file    LohseAssignment.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-03-28
@version $Id: cLogit.py 5249 2008-03-27 14:08:44Z yunpangwang $

This script is for executing the traffic assignment with the assignment model proposed by Professor Dieter Lohse.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime, math
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Predecessor, Vertex, Edge, Path, Vehicle
from network import Net, NetworkReader, DistrictsReader
                                                  
from inputs import getParameter, getMatrix, getConnectionTravelTime                 
from outputs import timeForInput, outputODZone, outputNetwork, outputStatistics, sortedVehOutput, vehPoissonDistr
from assign import doSUEAssign, doLohseStopCheck, doSUEVehAssign
from getPaths import findNewPath

# for measuring the required time for reading input files
inputreaderstart = datetime.datetime.now()  
# initialize the file for recording the process and the errors
foutlog = file('Lohse_log.txt', 'w')
foutlog.write('The stochastic user equilibrium traffic assignment will be executed with the Lohse model.\n')
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
    
    foutlog.write('- Reading network: done.\n')
    foutlog.write('number of total startVertices:%s\n' %len(net._startVertices))
    foutlog.write('number of total endVertices:%s\n' %len(net._endVertices))
        
    if options.verbose:
        print len(net._edges), "edges read"
    
    for edgeID in net._edges:                                                        
        edge = net._edges[edgeID]
        edge.getCapacity(options.parfile)
        edge.getCRcurve()
        edge.getActualTravelTime(options.curvefile)
        edge.helpacttime = edge.freeflowtime
    # calculate link travel time for all district connectors 
    getConnectionTravelTime(net._startVertices, net._endVertices)
            
    foutlog.write('- Initial calculation of link parameters : done.\n')
    # the required time for reading the network     
    timeForInput(inputreaderstart)                                                 
    
    if options.debug:
        outputNetwork(net)
    
    # read the control parameters for the Lohse traffic assignment:  
    #   - under: used to alculate the parameter fTT, related to the link travel time.
    #   - upper: used to alculate the parameter fTT, related to the link travel time.
    #   - v1: used to calculate the parameter fTT, related to the link travel time.
    #   - v2: used with v1 for calculating the parameter fTT
    #   - v3: used with v1 and v2 for calculating the parameter fTT
    #   - cvg1: convergence parameter used in the Lohse model
    #   - cvg2: maximum number of effective routes
    #   - cvg3: maximum number of the SUE iterations
    #   - maxIter: the difference of link flows between the two consequent iterations
    #   - KPaths: the number of k shortest paths for each OD pair
    #   - vehicle releasing method: 0: uniform randomly; 1: Poisson (arcontrol[10])
    #   - Number of the periods (Parcontrol[11])
    #   - Begin time (Parcontrol[12])
    
    Parcontrol = getParameter(options.parfile)
    if options.verbose:
        print 'Control parameters(Parcontrol):', Parcontrol
    
    under = float(Parcontrol[0])
    upper = float(Parcontrol[1])
    v1 = float(Parcontrol[2])
    v2 = float(Parcontrol[3])
    v3 = float(Parcontrol[4])
    cvg1 = float(Parcontrol[5])                              
    cvg2 = float(Parcontrol[6])
    cvg3 = float(Parcontrol[7])
    maxIter = int(Parcontrol[8])
    KPaths = int(Parcontrol[9])
    
    begintime = int(Parcontrol[(len(Parcontrol)-1)])
    
    if options.verbose:
        print 'number of the analyzed matrices:', len(matrices)
        print 'Begintime:', begintime, "O'Clock"
    
    # initialization
    matrixCounter = 0
    vehID = 0
    MatrixSum = 0.0
    lohse = True
    first = True
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
    foutroute = open('routes.rou.xml', 'w')                                           # initialize the file for recording the routes
    print >> foutroute, """<?xml version="1.0"?>
<!-- generated on %s by $Id: cLogit.py 5249 2008-03-27 14:08:44Z yunpangwang $ -->
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
        newRoutes = 0
        stable = False
        # traffic Assignment based on the Lohse Model
        while iter_outside == 1 or newRoutes > 0:
            iter_inside = 1
            # Generate the effective routes als intital path solutions, when considering k shortest paths (k is defined by the user.)
            if first and KPaths > 1:
                newRoutes = net.calcPaths(newRoutes, options.verbose, KPaths, startVertices, endVertices, matrixPshort)
                foutlog.write('- Finding the k-shortest paths for each OD pair: done.\n')
                if options.verbose:
                    print 'iter_outside:', iter_outside
                    print 'KPaths:', KPaths 
                    print 'number of new routes:', newRoutes                
            else:
                print 'search for the new path'
                newRoutes = findNewPath(startVertices, endVertices, net, newRoutes, matrixPshort, lohse)
            if options.verbose:
                print 'iter_outside:', iter_outside
                print 'number of new routes:', newRoutes
            
            stable = False
            while not stable:
                print 'iter_inside:', iter_inside
                doSUEAssign(options.curvefile, options.verbose, Parcontrol, net, startVertices, endVertices, matrixPshort, iter_inside, lohse, first)
                stable = doLohseStopCheck(net, options.verbose, stable, iter_inside, maxIter, Parcontrol, foutlog)
                iter_inside += 1
            
            first = False    
            iter_outside += 1
            
            if iter_outside > maxIter:
                newRoutes = 0
                print 'Number of the max. iterations is reached!'
                foutlog.write('- Number of the max. iterations is reached!.\n')
        
        # generate vehicle data
#        AssignedVeh, AssignedTrip, vehID = doLohseVehAssign(options.verbose, net, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh, AssignedTrip,vehID, lohse)
        AssignedVeh, AssignedTrip, vehID = doSUEVehAssign(options.verbose, net, counter, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh, AssignedTrip, vehID, lohse)                
        # generate vehicle releasing time
        net.vehRelease(options.verbose, Parcontrol, departtime, CurrentMatrixSum)
    
        # output vehicle releasing time and vehicle route 
        sortedVehOutput(net._vehicles, foutroute)
    
    foutroute.write('</routes>\n')
    foutroute.close()
    
    foutassign = file('assign.txt', 'a')
    foutassign.write('\n')
    for edge in net._edges.itervalues():
        foutassign.write('\nedge:%s, flow:%s, traveltime:%s\n ' %(edge.label, edge.flow, edge.actualtime))
    foutassign.close()
    
    # output the global performance indices
    assigntime = outputStatistics(net, starttime, Parcontrol)
    
    # output the number of vehicles in the given time interval(10 sec) accoding to the Poisson distribution
    if options.debug:
        vehPoissonDistr(net, Parcontrol, begintime)
    
    foutlog.write('- Assignment is completed and all vehicular information is generated. ')
    foutlog.close()
                                              
    print 'Duration for traffic assignment:', assigntime
    print 'Total assigned vehicles:', vehID
    print 'Total number of the assigend trips:', MatrixSum

    
main()