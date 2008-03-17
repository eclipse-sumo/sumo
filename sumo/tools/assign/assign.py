# python
# execute the incremetal traffic assignment with the given amount of OD demand
# the path information and the path travel time at each iteration will be stored in the file "path.txt"

#import math
#from network import Net
#from elements import Vertex, Edge, Vehicle

import os, random, string, sys, math
import elements
from elements import Vertex, Edge, Path, Vehicle                                    # import the characteristics of Vertices, Edges and paths
from network import Net
from commonality import CalCommonality

def DoIncAssign(net, verbose, Parcontrol, iter, endVertices, start, end, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID): # matrixPlong, matrixTruck):

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
    
# execute the SUE traffic assignment with the given path set

def DoSUEAssign(curvefile, verbose, Parcontrol, net, startVertices, endVertices, matrixPshort, alpha, iter):  #, matrixPlong, matrixTruck
    if verbose:
        print 'pathNum', elements.pathNum   
    start = -1                  
    for startVertex in startVertices:                                                # calculate the overlapping factors between any two paths of a given OD pair
        start += 1
        end = -1
        for endVertex in endVertices:
            end += 1
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex):
                ODPaths = net._paths[startVertex][endVertex]
                for path in ODPaths:                                                # update all path costs of the given OD pair
                    path.UpdatePathActTime(net)
                sum_exputility = CalCommonality(net, ODPaths, Parcontrol)
                
                for path in ODPaths:                                         # calculate the path choice probabilities and the path flows for the given OD Pair
                    path.choiceprob = math.exp(float(Parcontrol[4])*(-path.actpathtime - path.commfactor))/ sum_exputility  
                    path.helpflow = matrixPshort[start][end] * path.choiceprob   #  + matrixPlong[start][end] + matrixTruck[start][end]
                    if iter < 2:    
                        for edge in path.Edges:
                            if str(edge.source) != str(edge.target):
                                edge.flow += path.helpflow
                    else:
                        for edge in path.Edges:
                            if str(edge.source) != str(edge.target):
                                edge.helpflow += path.helpflow                 # the path flow cannot be referenced
 
    TotalTime = 0.0                                                            # Reset the total link travel times in the network
      
    for edge in net._edges.itervalues():                                       # the link travel times will be updated according to the latest traffic assingment
        if str(edge.source) != str(edge.target):
            if iter > 1:
                edge.flow = edge.flow + alpha * (edge.helpflow - edge.flow)
                
                if edge.flow < 0.:
                    edge.flow = 0.
            edge.helpflow = 0.0                                                # reset the edge.helpflow for the next iteration
            edge.getACTTT(curvefile) 
            TotalTime += edge.actualtime * edge.flow
     
    return TotalTime

# calculate the path choice probabilities and the path flows for each OD Pair    
def DoVehAssign(net, verbose, counter, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh,  AssignedTrip, vehID):
    if verbose:
        if counter == 0:
            foutpath = file('paths.txt', 'w')
        else:
            foutpath = file('paths.txt', 'a')
        foutpath.write('the analyzed matrix=%s' %counter)
    TotalPath = 0
    start = -1
    for startVertex in startVertices:
        start += 1
        end = -1
        if verbose:
            foutpath.write('\norigin=%s, ' %startVertex)
        for endVertex in endVertices:
            end += 1
            pathcount = 0
            cumulatedFlow = 0.
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex):
                if verbose:
                    foutpath.write('destination=%s' %endVertex)
                ODPaths = net._paths[startVertex][endVertex]
                for path in ODPaths:                                  # update the path travel times
                    TotalPath += 1
                    path.UpdatePathActTime(net)
                
                sum_exputility = CalCommonality(net, ODPaths, Parcontrol)
                
                for path in ODPaths:
                    pathcount += 1
                    path.choiceprob = math.exp(float(Parcontrol[4])*(-path.actpathtime - path.commfactor))/ sum_exputility  
                    if pathcount < len(ODPaths):
                        path.pathflow = matrixPshort[start][end] * path.choiceprob    # + matrixPlong[start][end] + matrixTruck[start][end]
                        cumulatedFlow += path.pathflow
                    else:
                        path.pathflow = matrixPshort[start][end] - cumulatedFlow
                    if verbose:
                        foutpath.write('\npathID= %s, path flow=%2.2f, actpathtime=%2.2f, choiceprob=%2.2f, edges=' %(path.label, path.pathflow, path.actpathtime, path.choiceprob))
                        for item in path.Edges:
                            foutpath.write('%s, ' %(item.label))
                    
                    AssignedTrip[startVertex][endVertex] += path.pathflow

                    while AssignedVeh[startVertex][endVertex] < int(round(AssignedTrip[startVertex][endVertex])): # generate vehicle list
                        vehID += 1
                        newVehicle = net.addVehicle(str(vehID))
                        newVehicle.route = path.Edges
                        AssignedVeh[startVertex][endVertex] += 1
                        if verbose:
                            print 'vehID:', vehID
                            print 'AssignedTrip[start][end]', AssignedTrip[startVertex][endVertex]
                            print 'AssignedVeh[start][end]', AssignedVeh[startVertex][endVertex]
                   
    if verbose:
        print 'total Number of the used paths for the current matrix:', TotalPath 
        foutpath.write('\ntotal Number of the used paths for the current matrix:%s' %TotalPath)
        foutpath.close()
    return AssignedVeh, AssignedTrip, vehID