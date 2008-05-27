"""
@file    matrixEstimator.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-04-21
@version $Id: matrixEstimator.py 5458 2008-04-18 20:18:21Z behrisch $

This script is to execute the matrix estimation with traffic counts.
The model used here is the generalized least squared model, proposed by Cascetta(1984).

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime, math, operator
from numpy import *
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Predecessor, Vertex, Edge, Path, Vehicle, DetectedFlows
from network import Net, NetworkReader, DistrictsReader, ExtraSignalInformationReader, DetectedFlowsReader                                                
from inputs import getParameter, getMatrix, getConnectionTravelTime
from outputs import timeForInput, outputODZone, outputNetwork, outputMatrix
from assign import getLinkChoiceProportions
from glsMatrixEstimation import doMatrixEstimation, initialLinkChoiceProportionsMatrix, resetLinkChoiceMatrix, getWeekday

# for measuring the required time for reading input files
inputreaderstart = datetime.datetime.now()  
# initialize the file for recording the process and the errors
foutlog = file('matrixEstimator_log.txt', 'w')
foutlog.write('The stochastic user-equilibrium traffic assignment will be executed either with the C-logit model or with the Lohse model.\n')
foutlog.write('The non-user-equilibrium traffic assignment will be executed with incremental assignment.\n')
optParser = OptionParser()

optParser.add_option("-m", "--matrix-file", dest="mtxpsfile", 
                     help="read OD matrix for passenger vehilces(short dist.) from FILE (mandatory)", metavar="FILE")
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
optParser.add_option("-l", "--loopdetector-file", dest="loopfile",
                     help="read detected flows from FILE", metavar="FILE")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-b", "--debug", action="store_true", dest="debug",
                     default=False, help="debug the program")
                                      
(options, args) = optParser.parse_args()

if not options.netfile or not options.confile or not options.mtxpsfile or not options.loopfile:
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
    
    parser.setContentHandler(DetectedFlowsReader(net))
    parser.parse(options.loopfile)
    
    if options.sigfile:
        parser.setContentHandler(ExtraSignalInformationReader(net))
        parser.parse(options.sigfile)
    
    foutlog.write('- Reading network: done.\n')
    foutlog.write('number of total startVertices:%s\n' %len(net._startVertices))
    foutlog.write('number of total endVertices:%s\n' %len(net._endVertices))
        
    if options.verbose:
        print len(net._edges), "edges read"
    foutflow = file('flowreader.txt', 'w')
    for edgeID in net._edges: 
        edge = net._edges[edgeID]
        if edge.numberlane > 0. or edge.detectorNum > 0.:
            edge.getCapacity()
            edge.getCRcurve()
            edge.getDefaultCapacity()
            edge.getAdjustedCapacity(net)
            edge.getActualTravelTime(options.curvefile)
        foutflow.write('    <edge id="%s" groupid="%s" detectors="%s">\n' %(edge, 000, edge.detectorNum))    
        for data in edge.detecteddata.itervalues():
            foutflow.write('        <flows weekday-time="%s" passengercars="%s" truckflows="%s">\n' %(data.label, data.flowPger, data.flowTruck)) 
    foutflow.write('    </edge>\n') 

    # calculate link travel time for all district connectors 
    getConnectionTravelTime(net._startVertices, net._endVertices)
            
    foutlog.write('- Initial calculation of link parameters : done.\n')
    # the required time for reading the network     
    timeForInput(inputreaderstart)                                                 
    
    if options.debug:
        outputNetwork(net)
    
    # read the control parameters for the matrix estimation:  
    #   - Parcontrol[0]: "under" in the Lohse model;  "beta" in the c-logit model; "iteration" in the incremental assignment;
    #   - Parcontrol[1]: "upper" in the Lohse model; "gamma" in the c-logit model; percent at the 1st iteration 
    #   - Parcontrol[2]: "v1" in the Lohse model; "lammda" in the c-logit model; percent at the 2nd iteration 
    #   - Parcontrol[3]: "v2" in the Lohse model; "devi" in the c-logit model; percent at the 3rd iteration 
    #   - Parcontrol[4]: "v3" in the Lohse model; "theta" in the c-logit model; percent at the 4-th iteration
    #   - Parcontrol[5]: "cvg1" in the Lohse model; "suek1" in the c-logit model; percent at the 5-th iteration
    #   - Parcontrol[6]: "cvg2" in the Lohse model; "suek2" in the c-logit model; percent at the 6-th iteration
    #   - Parcontrol[7]: "cvg3" in the Lohse model; "maxSUEIteration" in the c-logit model; percent at the 7-th iteration
    #   - Parcontrol[8]: "maxIter" in the Lohse model; "sueTolerance" in the c-logit model; percent at the 8-th iteration
    #   - Parcontrol[9]: "KPaths" in the Lohse model and the c-logit model; percent at the n-th iteration
    #   - Parcontrol[(len(Parcontrol)-5)]: matrix growth factor for adjusting the SampleMatrix
    #   - Parcontrol[(len(Parcontrol)-4)]: weekday index: 1- tuesday, 2-wednesday, 3-thursday, 4-friday, 5-saturday, 6-sunday, 7-avgWkday, 8-avgWkend
    #   - Parcontrol[(len(Parcontrol)-3)]: time index: ['-06-10', '-11-15', '-16-20', '-20-24', '-Allday']
    #   - Parcontrol[(len(Parcontrol)-2)]: odtype, pgr/truck
    #   - Parcontrol[(len(Parcontrol)-1)]: 
    #    - equals to 0, the incremental assignment will be adopted.
    #    - equals to 1, the stochastic user equilibrium assignment ( C-Logit model) will be adopted.
    #    - equals to 2, the stochastic user equilibrium assignment ( Lohse model) will be adopted.
    
    Parcontrol = getParameter(options.parfile)
    if options.verbose:
        print 'Control parameters(Parcontrol):', Parcontrol
    # initialization
    matrixCounter = 0
    MatrixSum = 0.0
    weekday = Parcontrol[len(Parcontrol)-4]
    weekday = getWeekday(weekday)

    timeindex = Parcontrol[len(Parcontrol)-3]
    odtype = Parcontrol[len(Parcontrol)-2]
    assignMethod = Parcontrol[len(Parcontrol)-1]
    net.initialPathSet()

    daytimeindex = weekday + timeindex

    if Parcontrol[(len(Parcontrol)-3)] == '-Allday':
        periods = 1.                                                # should be 24
    else:
        periods = 4.
    starttime = datetime.datetime.now()
    
    for counter in range (0, len(matrices)):
        matrix = matrices[counter]
        matrixCounter += 1
        net._detectedLinkCounts = 0
        convergence = False
        exRMSE = 99999.
        iter = 0
        iterations = 10
        net.countDetectedLinks(weekday, timeindex, odtype)
                
        # get the historical matrix
        matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum = getMatrix(net, options.verbose, matrix, MatrixSum)
        
        if options.verbose:
            print 'CurrentMatrixSum:', CurrentMatrixSum
        
        foutlog.write('Reading matrix and O-D zones: done.\n')
        foutlog.write('CurrentMatrixSum:%s\n' %CurrentMatrixSum)
        foutlog.write('number of current startVertices:%s\n' %len(startVertices))
        foutlog.write('number of current endVertices:%s\n' %len(endVertices))
               
        
        convergence = False
        linkChoiceProportions = initialLinkChoiceProportionsMatrix(net, startVertices, endVertices, matrixPshort)
        
        while not convergence:
        
            for edgeID in net._edges:
                edge = net._edges[edgeID]
                edge.flow = 0.
                edge.helpflow = 0.
                edge.estcapacity *= periods 
                edge.actualtime = edge.freeflowtime
                if assignMethod == '2':
                    edge.getActualTravelTime(options.curvefile)
                    edge.resetLohseParameter()
                    edge.helpacttime = edge.freeflowtime    
            
            # the number of origins, the umber of destinations and the number of the OD pairs
            origins = len(startVertices)                                    
            dests = len(endVertices)
            ODpairs = origins * dests
            
            linkChoiceProportions = resetLinkChoiceMatrix(net, startVertices, endVertices, matrixPshort, linkChoiceProportions)
    
            # output the origin and destination zones and the number of effective OD pairs
            if options.debug:
                outputODZone(startVertices, endVertices, Pshort_EffCells, matrixCounter)
        
            # execute the user-defined traffic assignment to get the route choioce proportions 
            linkChoiceProportions = getLinkChoiceProportions(options.curvefile, options.verbose, net, matrixPshort, Parcontrol, startVertices, endVertices, linkChoiceProportions, foutlog)
        
            # excuate the GLS model     # revised the parameter "daytimeindex" and "odtype" when the format of the detecter data file is ready.
            estimatedMatrix = doMatrixEstimation(net, options.verbose, Parcontrol, startVertices, endVertices, matrixPshort, linkChoiceProportions, weekday, timeindex, odtype)
            iter += 1
            
            # check convergence
            convergence, exRMSE = stopCheck(net, options.verbose, iter, iterations, exRMSE, startVertices, endVertices, matrixPshort, estimatedMatrix)
            
            if not convergence:
                matrixPshort = estimatedMatrix
            
        # output the estiamted matrix                         
        outputMatrix(startVertices, endVertices, estimatedMatrix, daytimeindex)
     
        print 'estimatedMatrix:'
        print estimatedMatrix
    foutlog.write('- Matrix estimation is completed and the respective matrix is generated in the file estimatedMatrix.fma.')
    foutlog.close()
      
main()