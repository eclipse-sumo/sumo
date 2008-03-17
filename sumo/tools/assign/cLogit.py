# python
# This script is for the stochastic user equilibrium traffic assignment with a sumo-based traffic network
# The C-Logit model is applied here.
# THe capacity constraint is applied here in order to avoid the overflow situation. 
# The Dijkstra algorithm is applied for searching the shortest paths.
# The necessary inputs include:
# SUMO net (.net.xml), OD-matrix file, zone connectors file, parameter file, OD-zone file and CR-curve file (for defining link cost functions)

import os, random, string, sys, datetime, math

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from elements import Predecessor, Vertex, Edge, Path, Vehicle                                             # import the characteristics of Vertices, Edges and paths
from network import Net, NetDetectorFlowReader, ZoneConnectionReader                # import characteristices of network and read xml files for retriving netork and zone connectors data
                                                     
from inputs import getParameter, getMatrix, getConnectionTravelTime                 # read control parameters and matrix; calculate the link travel time of zone connectors
from outputs import TimeforInput, OutputODZone, OutputNetwork, OutputMOE, SortedVehOutput, VehPoissonDistr # output the related results
from assign import DoSUEAssign, DoVehAssign                                      # import the algorithm of the incremental traffic assignment 
from getPaths import findNewPath
from VehRelease import VehRelease

## Program execution
inputreaderstart = datetime.datetime.now()                                        # for measuring the required time for reading input files
# initialize the file for recording the process and the errors
foutlog = file('SUE_log.txt', 'w')
foutlog.write('The stochastic user equilibrium traffic assignment will be executed with the C-logit model.\n')
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

# read the control parameters for the SUE traffic assignment:  
#   - beta, gamma: used in the function of the communality factor
#   - lammda: used in the capacity contraint for calculating travel time penality
#   - devi: Deviation of the travel time among  the effective routes
#   - theta: perception deviation of the shortest travel time among drivers
#   - alpha: moving-size sequence for updating link flows, which is applied in MSA. 
#     alpha(n) = k1/(k2+n), where n : the number of iterations; 
#     k1: a positive constant and determins the magnitude of the move; 
#     k2: a nonnegative constant and acts as an offset to the starting step.
#   - NumEffPath: maximum number of effective routes
#   - MaxSUEIteration: maximum number of the SUE iterations
#   - SUETolerance: the difference of link flows between the two consequent iterations

Parcontrol = getParameter(options.parfile)
if options.verbose:
    print 'Control parameters(Parcontrol):', Parcontrol

Beta = float(Parcontrol[0])
Gamma = float(Parcontrol[1])
Lammda = float(Parcontrol[2])
Devi = float(Parcontrol[3])
Theta = float(Parcontrol[4])
SUEk1 = float(Parcontrol[5])                              # parameter for determining the modification degree of link flows at each iteration in SUE
SUEk2 = float(Parcontrol[6])                              # parameter for determining the modification degree of link flows at each iteration in SUE
MaxSUEIteration = int(Parcontrol[7])
SUETolerance = float(Parcontrol[8])
KPaths = int(Parcontrol[9])
# Poisson = Parcontrol[10]                                  # on : 1; off: 0
# number of the periods = Parcontrol[11]
# begin time = Parcontrol[12]

MatrixCounter = 0
begintime = int(Parcontrol[(len(Parcontrol)-1)])
starttime = datetime.datetime.now()
if options.verbose:
    print 'number of the analyzed matrices:', len(matrices)
    print 'Begintime:', begintime, "O'Clock"

# initialization
MatrixCounter = 0
vehID = 0
MatrixSum = 0.0

AssignedVeh = {}                                                    # initialize the map for recording the number of the assigned vehicles
AssignedTrip = {}                                                   # initialize the map for recording the number of the assigned trips
for startVertex in net._startVertices:
    AssignedVeh[startVertex]={}                                                              
    AssignedTrip[startVertex]={} 
    for endVertex in net._endVertices:
        AssignedVeh[startVertex][endVertex] = 0
        AssignedTrip[startVertex][endVertex] = 0.

starttime = datetime.datetime.now()

# read in the matrix related information: 
# matrixPshort: Matrix of passenger traffic for short distance; 
# matrixPlong: Matrix of passenger traffic for long distance; matrixTruck: Matrix of truck traffic
# startVertices: set of origins; endVertices: set of destinations
# Pshort_EffCells: number of OD Pairs, which traffic demand is not zero, in the Matrix of passenger traffic for short distance
# Plong_EffCells: number of OD Pairs, which traffic demand is not zero, in the Matrix of passenger traffic for long distance
# Truck_EffCells: number of OD Pairs, which traffic demand is not zero, in the Matrix of truck traffic

for counter in range (0, len(matrices)):                            # matrix ist im 1. Durchlauf="matrix05-08.fma", im 2.="matrix06-07.fma"
    net._vehicles = []                                              # delete all vehicle information related to the last matrix for saving the disk space
    matrix = matrices[counter]
    MatrixCounter += 1
    if options.verbose:
        print 'Matrix: ', MatrixCounter
    departtime = (begintime + int(counter)) * 3600
    if options.verbose:
        print 'departtime', departtime                                  # in second
#	print 'Begintime:', begintime

    matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum = getMatrix(net, options.verbose, matrix, MatrixSum)
    if options.verbose:
        print 'Matrix und OD Zone already read for Interval', counter
        print 'CurrentMatrixSum:', CurrentMatrixSum
        print 'startVertices:', startVertices
        print 'endvertices:', endVertices          
#    getConnectionTravelTime(startVertices, endVertices) # calculate link travel time for all zone connectors
    foutlog.write('Reading matrix and O-D zones: done.\n')

    origins = len(startVertices)                                    # number of origins
    dests = len(endVertices)                                        # number of destinations
    ODpairs = origins * dests                                       # number of the OD pairs

#OutputODZone(startVertices, endVertices, Pshort_EffCells, Plong_EffCells, Truck_EffCells)# output the origin and destination zones and the number of effective OD pairs
#    OutputODZone(startVertices, endVertices, Pshort_EffCells, MatrixCounter)        # output the origin and destination zones and the number of effective OD pairs
#    OutputNetwork(net)                                                              # output the converted network data
    
# execute the SUE traffic assignment based on the C-Logit Model    
    iter = 1                                                                        # iterartion counter for the SUE assignment
    StopIndex = 9999999.
    preTotalTime = 9999999.0
    TotalTime = 0.0
    NewRoutes = 0
    
# Generate the effective routes als intital path solutions, when considering k shortest paths (k is defined by the user.)
    if counter == 0:
        NewRoutes = net.calcPaths(NewRoutes, options.verbose, KPaths, startVertices, endVertices, matrixPshort)
        foutlog.write('- Finding the k-shortest paths for each OD pair: done.\n')   
    if options.verbose:
        print 'KPaths:', KPaths 
        print 'number of new routes:', NewRoutes
    
# SUE traffic Assignment based on the C-Logit Model
    while StopIndex > SUETolerance or NewRoutes > 0:      
        foutlog.write('- SUE iteration:%s\n' %iter)
             
        alpha = SUEk1/float(SUEk2 + iter)                                         # the parameter in the MSA algorithm
        if options.verbose:
            print 'iteration:', iter
            print 'alpha:', alpha
            print 'SUETolerance:', SUETolerance
        TotalTime = DoSUEAssign(options.curvefile, options.verbose, Parcontrol, net, startVertices, endVertices, matrixPshort, alpha, iter)  # , matrixPlong, matrixTruck, 

        if iter > 1:
            StopIndex = math.fabs((preTotalTime-TotalTime)/TotalTime)                    # Check if the convergence reaches.
        if options.verbose:
            print 'preTotalTime:', preTotalTime
            print 'TotalTime:', TotalTime
            print 'StopIndex:', StopIndex
        
        NewRoutes = findNewPath(startVertices, endVertices, net, iter, NewRoutes, matrixPshort)
        if options.verbose:
            print 'number of new routes:', NewRoutes 
        preTotalTime = TotalTime
        iter += 1
        if iter > MaxSUEIteration:
            print 'The max. number of iterations is reached!'
            foutlog.write('The max. number of iterations is reached!\n')
            print 'StopIndex:', StopIndex 
            StopIndex = 0.

# update the path choice probability and the path flows as well as generate vehicle data 	
    AssignedVeh, AssignedTrip, vehID = DoVehAssign(net, options.verbose, counter, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh, AssignedTrip, vehID)
                
# generate vehicle releasing time            
    VehRelease(net, options.verbose, Parcontrol, departtime, CurrentMatrixSum)

# output vehicle releasing time and vehicle route 
    SortedVehOutput(net, counter, Parcontrol)

    # output the global performance indices
assigntime = OutputMOE(net, starttime, Parcontrol)

# output the number of vehicles in the given time interval(10 sec) accoding to the Poisson distribution
VehPoissonDistr(net, Parcontrol, begintime)

foutlog.write('- Assignment is completed and the all vehicular information is generated. ')
foutlog.close()
                                          
print 'Duration for traffic assignment:', assigntime
print 'Total number of the assigend trips:', MatrixSum
