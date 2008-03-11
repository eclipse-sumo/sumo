# python
# This script is for the incremental traffic assignment with a sumo-based traffic network
# The Dijkstra algorithm is applied for searching the shortest paths.
# The necessary inputs include:
# SUMO net (.net.xml), OD-matrix file, zone connectors file, parameter file, OD-zone file and CR-curve file (for defining link cost functions)

import os, string, sys, datetime, random, math

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Vertex, Edge, Vehicle                                          # import the characteristics of Vertices, Edges and paths
from network import Net, NetDetectorFlowReader, ZoneConnectionReader                # import characteristices of network and read xml files for retriving netork and zone connectors data
from dijkstra import Dijkstra                                                       # import the Dijkstra algorithm for searching shortest paths
from inputs import getParameter, getMatrix, getConnectionTravelTime                 # read control parameters and matrix; calculate the link travel time of zone connectors
from outputs import TimeforInput, OutputODZone, OutputNetwork, TimeforAssign, SortedVehOutput, OutputMOE, VehPoissonDistr # output the related results
from assign import DoAssign                                                           # import the algorithm of the incremental traffic assignment 
from VehRelease import VehRelease

## Program execution
inputreaderstart = datetime.datetime.now()                                        # for measuring the required time for reading input files
# initialize the file for recording the process and the errors
foutlog = file('SUK_log.txt', 'w')
foutlog.write('The traffic assignment will be executed with the incremental assignment.\n')
foutlog.write('All vehicular releasing times are determined randomly(uniform).\n')

optParser = OptionParser()

optParser.add_option("-c", "--zonalconnection-file", dest="confile",              #  XML file: containing the link information connectings to the respective traffic zone
                     help="read OD Zones from FILE (mandatory)", metavar="FILE")
optParser.add_option("-m", "--matrix-file", dest="mtxpsfile",                     # txt file: containing the matrix information for passenger vehicles 
                     help="read OD matrix for passenger vehilces(long dist.) from FILE (mandatory)", metavar="FILE")# from the respective VISUM file
#optParser.add_option("-k", "--matrixpl-file", dest="mtxplfile",                     # txt file: containing the matrix information for trucks from the respective VISUM file
#                     help="read OD matrix for passenger vehilces(long dist.) from FILE (mandatory)", metavar="FILE")  
#optParser.add_option("-t", "--matrixt-file", dest="mtxtfile",                     # txt file: containing the matrix information for trucks from the respective VISUM file
#                     help="read OD matrix for trucks from FILE (mandatory)", metavar="FILE")  
optParser.add_option("-n", "--net-file", dest="netfile",                          # XML file: containing the network geometric (link-node) information
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-p", "--parameter-file", dest="parfile",                    # txt file: containing the control paramenter for incremental traffic assignment
                     help="read assignment parameters from FILE (mandatory)", metavar="FILE")
optParser.add_option("-u", "--curve-file", dest="curvefile", default="CRcurve.txt",
                     help="read CRcurve from FILE", metavar="FILE")
#optParser.add_option("-z", "--district-file", dest="zonefile",                    # txt file: containing the OD zones based on the respective VISUM file
#                     help="read OD Zones from FILE (mandatory)", metavar="FILE")  
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
                     
(options, args) = optParser.parse_args()

matrices = options.mtxpsfile.split(",")                                # jetzt ist matrices = [ "matrix05-08.fma", "matrix06-07.fma", ...
#odzones = options.zonefile.split(",")                                  # jetzt ist matrices = [ "matrix05-08.fma", "matrix06-07.fma", ...

if not options.netfile:
    optParser.print_help()
    sys.exit()
parser = make_parser()

if options.verbose:
    print "Reading net"
    
net = Net()                                                             # generate the investigated network from the respective SUMO-network

reader = NetDetectorFlowReader(net)
parser.setContentHandler(reader)
parser.parse(options.netfile)

reader2 = ZoneConnectionReader(net)
parser.setContentHandler(reader2)
parser.parse(options.confile)

foutlog.write('- Reading network: done.\n')

if options.verbose:
    print len(net._edges), "edges read"

for edgeID in net._edges:                                                        
    edge = net._edges[edgeID]
    edge.getFFTT()                                                       # calculate link trave time at free flow speed
    edge.getAppCapacity(options.parfile)
    edge.getCRcurve()                                                    # identify the respective cost function curve based on the max. speed and the number of lanes
    edge.getACTTT(options.curvefile)                                     # calculate actual link trave time

getConnectionTravelTime(net._startVertices, net._endVertices) # calculate link travel time for all zone connectors    
    
foutlog.write('- Initial calculation of link parameters : done.\n')
    
TimeforInput(inputreaderstart)                                           # the required time for reading the network       
    
# read the control parameters(the number of iterations,  
# and the assigned percentages of the matrix at each iteration) and the traffic demand 
Parcontrol = getParameter(options.parfile)
print 'Parcontrol[number of iterations, n.%, Uni-/PoissonRandomRelease, number of periods, beginning time]:', Parcontrol    # 0: UniRandomRelease; 1: PoissonRandomRelease

foutlog.write('Reading control parameters: done.\n')

begintime = int(Parcontrol[(len(Parcontrol)-1)])
iteration = int(Parcontrol[0])                                            # number of iterations in the incremental traffic assignment

print 'number of the analyzed matrices:', len(matrices)
print 'Begin Time:', begintime, "o'Clock"

# initialization
MatrixCounter = 0
vehID = 0
MatrixSum = 0.0
starttime = datetime.datetime.now()

AssignedVeh = {}                                                # initialize the map for recording the number of the assigned vehicles
AssignedTrip = {}                                               # initialize the map for recording the number of the assigned trips

for startVertex in net._startVertices:
    AssignedVeh[startVertex] ={}
    AssignedTrip[startVertex] ={}

    for endVertex in net._endVertices:
        AssignedVeh[startVertex][endVertex] = 0
        AssignedTrip[startVertex][endVertex] = 0.
    
for counter in range (0, len(matrices)):                                         # matrix ist im 1. Durchlauf="matrix05-08.fma", im 2.="matrix06-07.fma"
    matrix = matrices[counter]
    MatrixCounter += 1
    print 'Matrix: ', MatrixCounter
    departtime = (begintime + int(counter)) * 3600
    print 'departtime', departtime                                                # in second
#	foutlog.write('the current analyzed matrix: %s \n' % counter)
    
# matrixPshort, matrixPlong, matrixTruck, startVertices, endVertices, Pshort_EffCells, Plong_EffCells, Truck_EffCells = getMatrix(net, options.zonefile, options.mtxpsfile, options.mtxplfile, options.mtxtfile)
    matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum = getMatrix(net, matrix, MatrixSum)
    print 'Matrix und OD Zone already read for Interval', counter
    foutlog.write('Reading matrix and O-D zones: done.\n')
    
    origins = len(startVertices)                                                    # number of origins
    dests = len(endVertices)                                                        # number of destinations
    ODpairs = origins * dests                                                       # number of the OD pairs

#	 OutputODZone(startVertices, endVertices, Pshort_EffCells, Plong_EffCells, Truck_EffCells)# output the origin and destination zones and the number of effective OD pairs
#	 OutputODZone(startVertices, endVertices, Pshort_EffCells, MatrixCounter)        # output the origin and destination zones and the number of effective OD pairs
#    OutputNetwork(net)                                                             # output the converted network data

    iter = 0
    while iter < iteration:
        foutlog.write('- Current iteration(not executed yet):%s\n' %iter)
        iter += 1                                                                   # number of iterations 
        print 'iter:', iter
        start = -1                                                                  # reset the origin index used in the matrix
        end = -1                                                                    # reset the destination index used in the matrix

        for startVertex in startVertices:
            start += 1
            end = -1                                                                # reset the destination index used in the matrix
            print 'begin the route searching from ', startVertex
            D,P = Dijkstra(startVertex)                                             # the information about the shortest paths and    
                                                                                    # the respective travel times from the given "startVertex" 
            prevehID = vehID                                                        # to all the other nodes will be identified.
            print 'prevehID', prevehID
# incremental traffic assignment
            vehID, AssignedVeh, AssignedTrip = DoAssign(net, Parcontrol, iter, endVertices, start, end, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID)     
            print 'vehID:', vehID
        
        for edgeID in net._edges:                                                   # the link travel times will be updated according to the latest traffic assingment
            edge = net._edges[edgeID]
            edge.getACTTT(options.curvefile)

    VehRelease(net, Parcontrol, departtime, CurrentMatrixSum)                        # generate vehicular releasing times

# output the generated releasing times and routes of the vehicles based on the current matrix
    SortedVehOutput(net, counter, Parcontrol)
	
# the required time for executing the incremental traffic assignemnt
assigntime = TimeforAssign(starttime)

# output the average vehicular travel time
OutputMOE(net, Parcontrol)

# output the number of vehicles in the given time interval(10 sec) accoding to the Poisson distribution
VehPoissonDistr(net, Parcontrol, begintime)

foutlog.write('- Assignment is completed and the all vehicular information is generated. ')
foutlog.close()
                                 
print 'Total duration for traffic assignment:', assigntime
print 'Total number of the assigend trips:', MatrixSum
