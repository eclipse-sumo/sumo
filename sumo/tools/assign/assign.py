# python
# execute the incremetal traffic assignment with the given amount of OD demand
# the path information and the path travel time at each iteration will be stored in the file "path.txt"

import math
from network import Net
from elements import Vertex, Edge, Vehicle

def DoAssign(net, ODcontrol, iter, endVertices, start, end, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID): # matrixPlong, matrixTruck):

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

            pathflow = float(matrixPshort[start][end]*float(ODcontrol[iter]))  # the amount of the pathflow, which will be released at this iteration
            print 'pathflow:', pathflow
            interval = 3600. / float(ODcontrol[0])
            
            AssignedTrip[start][end] += pathflow
            
            while AssignedVeh[start][end] < AssignedTrip[start][end] and (AssignedTrip[start][end]-float(AssignedVeh[start][end]) >= 1.):  # VehCounter < pathflow and 
                vehID += 1
                newVehicle = net.addVehicle(str(vehID))
                newVehicle.route = helpPath
                newVehicle.traveltime = pathtime
                newVehicle.travellength = pathlength
                newVehicle.speed = pathlength / pathtime
                AssignedVeh[start][end] += 1
            print 'AssignedTrip[start][end]', AssignedTrip[start][end]
            print 'AssignedVeh[start][end]', AssignedVeh[start][end]

            while iter == int(ODcontrol[0]) and (AssignedVeh[start][end] < AssignedTrip[start][end]): # matrixPshort[start][end]):
                vehID += 1
                AssignedVeh[start][end] += 1
                newVehicle = net.addVehicle(str(vehID))
                newVehicle.route = helpPath
                newVehicle.traveltime = pathtime
                newVehicle.travellength = pathlength
                newVehicle.speed = pathlength / pathtime
            for edge in helpPath:
                edge.flow += matrixPshort[start][end]* float(ODcontrol[iter])                 
    
    return vehID, AssignedVeh, AssignedTrip