# python
# execute the incremetal traffic assignment with the given amount of OD demand
# the path information and the path travel time at each iteration will be stored in the file "path.txt"

import math
from network import Net
from elements import Vertex, Edge, Vehicle

def DoAssign(net, verbose, Parcontrol, iter, endVertices, start, end, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID): # matrixPlong, matrixTruck):

    for endVertex in endVertices:                                               
        end += 1
        endnode = endVertex
        if str(startVertex) != str(endVertex) and (matrixPshort[start][end] > 0.0): # or matrixPlong[start][end] > 0.0 or matrixTruck[start][end] > 0.0):
            Path = []
            helpPath = []
            pathtime = 0.
            pathlength = 0.
            while 1:
                Path.append(endnode)
                if endnode == startVertex: 
                    break
                endnode = P[endnode]
            Path.reverse()                                                    # the path set (node list) will be generated regarding to the given destination "endVertex"

            for i in range(0, len(Path)):                                     # generate link list and save it in "helpPath"
                if Path[i] != endVertex:
                    node = Path[i]
                    for edge in node.outEdges:
                        if str(Path[i]) != str(Path[i+1]) and str(edge.source) == str(Path[i]) and str(edge.target) == str(Path[i+1]):
                            helpPath.append(edge)                             # link list of the shortest route
            
            for edge in helpPath[1:-1]:                    # for generating vehicle routes used in SUMO  
                pathlength += edge.length
                pathtime += edge.actualtime

            pathflow = float(matrixPshort[start][end]*float(Parcontrol[iter]))  # the amount of the pathflow, which will be released at this iteration
            if verbose:
                print 'pathflow:', pathflow
            interval = 3600. / float(Parcontrol[0])
            
            AssignedTrip[startVertex][endVertex] += pathflow
            
            while AssignedVeh[startVertex][endVertex] < int(round(AssignedTrip[startVertex][endVertex])):# and (AssignedTrip[startVertex][endVertex]-float(AssignedVeh[startVertex][endVertex]) >= 1.):  # VehCounter < pathflow and 
                vehID += 1
                newVehicle = net.addVehicle(str(vehID))
                newVehicle.route = helpPath
                AssignedVeh[startVertex][endVertex] += 1
            if verbose:
                print 'AssignedTrip[start][end]', AssignedTrip[startVertex][endVertex]
                print 'AssignedVeh[start][end]', AssignedVeh[startVertex][endVertex]

            for edge in helpPath:
                edge.flow += matrixPshort[start][end]* float(Parcontrol[iter])                 
    
    return vehID, AssignedVeh, AssignedTrip