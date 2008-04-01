"""
@file    assign.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-11-25
@version $Id$

This script is for executing traffic assignment according to the required assignment model.
The incremental assignment model, the C-Logit assignment model and the Lohse assignment model are included in this script. 

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, math
import elements
from elements import Vertex, Edge, Path, Vehicle
from network import Net

def doIncAssign(net, verbose, Parcontrol, iter, endVertices, start, end, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID): 
    # matrixPlong and matrixTruck should be added if available.
    for endVertex in endVertices:                                               
        end += 1
        endnode = endVertex
        if str(startVertex) != str(endVertex) and (matrixPshort[start][end] > 0.0):
        # if matrixPling and the matrixTruck exist, matrixPlong[start][end] > 0.0 or matrixTruck[start][end] > 0.0): should be added.
            Path = []
            helpPath = []
            pathtime = 0.
            pathlength = 0.
            while 1:
                Path.append(endnode)
                if endnode == startVertex: 
                    break
                endnode = P[endnode]
            Path.reverse()

            for i in range(0, len(Path)):
                if Path[i] != endVertex:
                    node = Path[i]
                    for edge in node.outEdges:
                        if str(Path[i]) != str(Path[i+1]) and str(edge.source) == str(Path[i]) and str(edge.target) == str(Path[i+1]):
                            helpPath.append(edge)
            # for generating vehicle routes used in SUMO 
            for edge in helpPath[1:-1]: 
                pathlength += edge.length
                pathtime += edge.actualtime
            # the amount of the pathflow, which will be released at this iteration
            pathflow = float(matrixPshort[start][end]*float(Parcontrol[iter]))  
            if verbose:
                print 'pathflow:', pathflow
            interval = 3600. / float(Parcontrol[0])
            
            AssignedTrip[startVertex][endVertex] += pathflow
            
            while AssignedVeh[startVertex][endVertex] < int(round(AssignedTrip[startVertex][endVertex])): 
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
    
# execute the C-Logit model with the given path set
def doCLogitAssign(curvefile, verbose, Parcontrol, net, startVertices, endVertices, matrixPshort, alpha, iter, first):
    # matrixPlong and matrixTruck should be added if available.
    if verbose:
        print 'pathNum', elements.pathNum   
    start = -1                  
    # calculate the overlapping factors between any two paths of a given OD pair
    for startVertex in startVertices: 
        start += 1
        end = -1
        for endVertex in endVertices:
            end += 1
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex):
                ODPaths = net._paths[startVertex][endVertex]
                
                for path in ODPaths:
                    path.updatePathActTime(net)
                    
                sum_exputility = calCommonality(net, ODPaths, Parcontrol)
                # calculate the path choice probabilities and the path flows for the given OD Pair
                for path in ODPaths:
                    path.choiceprob = math.exp(float(Parcontrol[4])*(-path.actpathtime - path.commfactor))/ sum_exputility  
                    path.helpflow = matrixPshort[start][end] * path.choiceprob
                    if first and iter == 1:    
                        for edge in path.Edges:
                            if str(edge.source) != str(edge.target):
                                edge.flow += path.helpflow
                    else:
                        for edge in path.Edges:
                            if str(edge.source) != str(edge.target):
                                edge.helpflow += path.helpflow
    
    # Reset the convergence index
    notstable = 0
                                                           
    # link travel timess and link flows will be updated according to the latest traffic assingment  
    for edge in net._edges.itervalues():                                       
        if str(edge.source) != str(edge.target):
            if iter > 1:
                exflow = edge.flow
                edge.flow = edge.flow*(1. - alpha) + alpha*edge.helpflow
                if edge.flow > 0.:
                    if abs((edge.flow-exflow)/edge.flow) > float(Parcontrol[8]):
                        notstable += 1
                elif edge.flow < 0.:
                    notstable += 1
                
                if edge.flow < 0.:
                    edge.flow = 0.
            # reset the edge.helpflow for the next iteration
            edge.helpflow = 0.0                                                
            edge.getActualTravelTime(curvefile) 
    
    if notstable > 0 or iter == 1:
        stable = False
    else:
        stable = True
    
    if notstable < len(net._edges)*0.05 and iter > 20:
        stable = True
        
    if iter > int(Parcontrol[7]):
        stable = True
     
    return stable

# calculate the commonality factors in the C-Logit model
def calCommonality(net, ODPaths, Parcontrol):
    # initialize the overlapping matrix
    mtxOverlap = {}
    for pathone in ODPaths:
        mtxOverlap[pathone]={}
        for pathtwo in ODPaths:
            mtxOverlap[pathone][pathtwo] = 0.
    
    for pathone in ODPaths:
        for pathtwo in ODPaths:
            for edgeone in pathone.Edges:
                for edgetwo in pathtwo.Edges:
                    if str(edgeone.label) == str(edgetwo.label):
                        mtxOverlap[pathone][pathtwo] += edgeone.actualtime
            mtxOverlap[pathtwo][pathone] = mtxOverlap[pathone][pathtwo]
   
    sum_exputility = 0.
    if len(ODPaths) > 1:
        # calculate the commonality factors (CF) for the given OD pair
        for pathone in ODPaths:   
            sum_overlap = 0.0 
            for pathtwo in ODPaths:
                sum_overlap += math.pow(mtxOverlap[pathone][pathtwo]/(math.pow(pathone.actpathtime,0.5) * math.pow(pathtwo.actpathtime,0.5)), float(Parcontrol[1]))
            pathone.commfactor = float(Parcontrol[0]) * math.log(sum_overlap)

            sum_exputility += math.exp(float(Parcontrol[4])*(-pathone.actpathtime - pathone.commfactor))
    else:    
        for path in ODPaths:
            path.commfactor = 0.
            sum_exputility += math.exp(float(Parcontrol[4])*(-path.actpathtime))

    return sum_exputility

# calculate the path choice probabilities and the path flows for each OD Pair    
def doCLogitVehAssign(net, verbose, counter, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh,  AssignedTrip, vehID):
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
                # update the path travel times
                for path in ODPaths:
                    TotalPath += 1
                    path.updatePathActTime(net)
                
                sum_exputility = calCommonality(net, ODPaths, Parcontrol)
                
                for path in ODPaths:
                    pathcount += 1
                    path.choiceprob = math.exp(float(Parcontrol[4])*(-path.actpathtime - path.commfactor))/ sum_exputility  
                    if pathcount < len(ODPaths):
                        path.pathflow = matrixPshort[start][end] * path.choiceprob
                        cumulatedFlow += path.pathflow
                    else:
                        path.pathflow = matrixPshort[start][end] - cumulatedFlow
                        if path.pathflow < 0.:
                            print '*********************** the path flow on the path:%s < 0.!!!!', path.label
                    if verbose:
                        foutpath.write('\npathID= %s, path flow=%4.4f, actpathtime=%4.4f, choiceprob=%4.4f, edges=' 
                                        %(path.label, path.pathflow, path.actpathtime, path.choiceprob))
                        for item in path.Edges:
                            foutpath.write('%s, ' %(item.label))
                    
                    AssignedTrip[startVertex][endVertex] += path.pathflow

                    while AssignedVeh[startVertex][endVertex] < int(round(AssignedTrip[startVertex][endVertex])):
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