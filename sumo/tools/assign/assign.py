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

import os, random, string, sys, math, operator
import elements
from elements import Vertex, Edge, Path, Vehicle
from network import Net
from getPaths import findNewPath

def doIncAssign(net, verbose, Parcontrol, iter, endVertices, start, startVertex, matrixPshort, D, P, AssignedVeh, AssignedTrip, vehID): 
    # matrixPlong and matrixTruck should be added if available.
    for end, endVertex in enumerate(endVertices): 
        if str(startVertex) != str(endVertex) and (matrixPshort[start][end] > 0.0):
        # if matrixPling and the matrixTruck exist, matrixPlong[start][end] > 0.0 or matrixTruck[start][end] > 0.0): should be added.
            Path = []
            helpPath = []
            pathtime = 0.
            pathlength = 0.
        
            vertex = endVertex
            while vertex != startVertex:
                if P[vertex].kind == "real":
                    helpPath.append(P[vertex])
                vertex = P[vertex].source
            helpPath.reverse()
            
            # for generating vehicle routes used in SUMO 
            for edge in helpPath[1:-1]: 
                pathlength += edge.length
                pathtime += edge.actualtime
            # the amount of the pathflow, which will be released at this iteration
            pathflow = float(matrixPshort[start][end]*float(Parcontrol[iter]))  
            if verbose:
                print 'pathflow:', pathflow
            
            AssignedTrip[startVertex][endVertex] += pathflow
            
            AssignedVeh, AssignedTrip, vehID = assignVeh(verbose, net, startVertex, endVertex, helpPath, AssignedVeh, AssignedTrip, vehID)

            for edge in helpPath:
                edge.flow += matrixPshort[start][end]* float(Parcontrol[iter])                 
    
    return AssignedVeh, AssignedTrip, vehID
  
# execute the SUE model with the given path set
def doSUEAssign(curvefile, verbose, Parcontrol, net, startVertices, endVertices, matrixPshort, iter, lohse, first):
    if lohse:
        if verbose:
            foutassign = file('assign.txt', 'a')
            foutassign.write('\niter:%s\n' %iter)
        under  = float(Parcontrol[0])
        upper = float(Parcontrol[1])
        v1 = float(Parcontrol[2])
        v2 = float(Parcontrol[3])
        v3 = float(Parcontrol[4])

    # matrixPlong and matrixTruck should be added if available.
    if verbose:
        print 'pathNum', elements.pathNum           
    # calculate the overlapping factors between any two paths of a given OD pair
    for start, startVertex in enumerate(startVertices): 
        for end, endVertex in enumerate(endVertices):
            cumulatedflow = 0.
            pathcount = 0
                        
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex):
                ODPaths = net._paths[startVertex][endVertex]
                
                for path in ODPaths:
                    path.getPathTimeUpdate(net)
                calCommonalityAndChoiceProb(net, ODPaths, Parcontrol, lohse)
                
                # calculate the path choice probabilities and the path flows for the given OD Pair
                for path in ODPaths:
                    pathcount += 1
                    if pathcount < len(ODPaths):
                        path.helpflow = matrixPshort[start][end] * path.choiceprob
                        cumulatedflow += path.helpflow
                        if lohse and verbose:
                            foutassign.write('    path:%s\n' % path.label)
                            foutassign.write('    path.choiceprob:%s\n' % path.choiceprob)
                            foutassign.write('    path.helpflow:%s\n' % path.helpflow)
                            foutassign.write('    cumulatedflow:%s\n' % cumulatedflow)
                    else:
                        path.helpflow = matrixPshort[start][end] - cumulatedflow 
                        if lohse and verbose:
                            foutassign.write('    last_path.helpflow:%s\n' % path.helpflow)
                    if first and iter == 1:
                        for edge in path.Edges:
                            if str(edge.source) != str(edge.target):
                                edge.flow += path.helpflow
                    else:
                        for edge in path.Edges:
                            if str(edge.source) != str(edge.target):
                                edge.helpflow += path.helpflow
    
    # Reset the convergence index for the C-Logit model
    notstable = 0
    stable =False
    # link travel timess and link flows will be updated according to the latest traffic assingment  
    for edge in net._edges.itervalues():                                       
        if str(edge.source) != str(edge.target):
            if (first and iter > 1) or (not first):
                exflow = edge.flow
                edge.flow = edge.flow + (1./iter)*(edge.helpflow - edge.flow)
                
                if not lohse:
                    if edge.flow > 0.:
                        if abs(edge.flow-exflow)/edge.flow > float(Parcontrol[8]):
                            notstable += 1
                    elif edge.flow == 0.:
                        if exflow != 0. and abs(edge.flow-exflow)/exflow > float(Parcontrol[8]):
                            notstable += 1
                    elif edge.flow < 0.:
                        notstable += 1
                        edge.flow = 0.
                else:
                    if edge.flow < 0.:
                        edge.flow = 0.
            
            # reset the edge.helpflow for the next iteration
            edge.helpflow = 0.0                                                
            edge.getActualTravelTime(curvefile)
 #           if edge.queuetime > 0.:
 #               notstable += 1
            if lohse:
                edge.getLohseParUpdate(under, upper, v1, v2, v3)
    if lohse and verbose:
        foutassign.close()
                                                               
    if not lohse:
        if notstable > 0 or iter == 1:
            stable = False
        else:
            stable = True
        
        if notstable < len(net._edges)*0.05:
            stable = True
            
        if iter > int(Parcontrol[7]):
            stable = True
         
    return stable

# calculate the commonality factors in the C-Logit model
def calCommonalityAndChoiceProb(net, ODPaths, Parcontrol, lohse):
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
                        if lohse:
                            mtxOverlap[pathone][pathtwo] += edgeone.helpacttime
                        else:
                            mtxOverlap[pathone][pathtwo] += edgeone.actualtime
            mtxOverlap[pathtwo][pathone] = mtxOverlap[pathone][pathtwo]

    if len(ODPaths) > 1:
        for pathone in ODPaths:
            sum_overlap = 0.0 
            for pathtwo in ODPaths:
                if lohse:
                    sum_overlap += math.pow(mtxOverlap[pathone][pathtwo]/(math.pow(pathone.pathhelpacttime,0.5) * math.pow(pathtwo.pathhelpacttime,0.5)), float(Parcontrol[1]))
                else:
                    sum_overlap += math.pow(mtxOverlap[pathone][pathtwo]/(math.pow(pathone.actpathtime,0.5) * math.pow(pathtwo.actpathtime,0.5)), float(Parcontrol[1]))
            
            pathone.commfactor = float(Parcontrol[0]) * math.log(sum_overlap)
        
        if lohse:
            minpath = min(ODPaths, key=operator.attrgetter('pathhelpacttime'))
            minpathcost = minpath.pathhelpacttime + minpath.commfactor
            beta = 12./(1.+ math.exp(0.7 - 0.015 * minpath.pathhelpacttime))
        else:
            theta = getThetaForCLogit(ODPaths)

        for pathone in ODPaths:
            sum_exputility = 0.
            for pathtwo in ODPaths:
                if str(pathone.label) != str(pathtwo.label):
                    if not lohse:
                        sum_exputility += math.exp(theta*(-pathtwo.actpathtime + pathone.actpathtime + pathone.commfactor - pathtwo.commfactor))
                    else:
                        pathonecost = pathone.pathhelpacttime + pathone.commfactor
                        pathtwocost = pathtwo.pathhelpacttime + pathtwo.commfactor
                        sum_exputility += math.exp(-(beta*(pathtwocost/minpathcost -1.))**2.+(beta*(pathonecost/minpathcost -1.))**2.)
            pathone.choiceprob = 1./(1. + sum_exputility)
    else:
        for path in ODPaths:
            path.commfactor = 0.
            path.choiceprob = 1.
            
# calculate the path choice probabilities and the path flows and generate the vehicular data for each OD Pair    
def doSUEVehAssign(verbose, net, counter, matrixPshort, Parcontrol, startVertices, endVertices, AssignedVeh, AssignedTrip, vehID, lohse):
    if verbose:
        if counter == 0:
            foutpath = file('paths.txt', 'w')
            fouterror = file('errors.txt', 'w')
        else:
            foutpath = file('paths.txt', 'a')
            fouterror = file('errors.txt', 'a')
        if lohse:
            foutpath.write('begin the doSUEVehAssign based on the lohse assignment model!')
        else:
            foutpath.write('begin the doSUEVehAssign based on the c-logit model!')
        foutpath.write('the analyzed matrix=%s' %counter)
        
    TotalPath = 0

    for start, startVertex in enumerate(startVertices):
        if verbose:
            foutpath.write('\norigin=%s, ' %startVertex)
        for end, endVertex in enumerate(endVertices):
            pathcount = 0
            cumulatedflow = 0.
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex):
                if verbose:
                    foutpath.write('destination=%s' %endVertex)
                ODPaths = net._paths[startVertex][endVertex]
                
                for path in ODPaths:
                    TotalPath += 1
                    path.getPathTimeUpdate(net)
                    if lohse:                      
                        path.helpacttime = path.actpathtime
      
                calCommonalityAndChoiceProb(net, ODPaths, Parcontrol, lohse)
        
                for path in ODPaths:
                    pathcount += 1
                    if pathcount < len(ODPaths):
                        path.pathflow = matrixPshort[start][end] * path.choiceprob
                        cumulatedflow += path.pathflow
                    else:
                        path.pathflow = matrixPshort[start][end] - cumulatedflow
                        if path.pathflow < 0.:
                            fouterror.write('*********************** the path flow on the path:%s < 0.!!' %path.label)
                            print '*********************** the path flow on the path:%s < 0.!!!!', path.label
                    if verbose:
                        foutpath.write('\npathID= %s, path flow=%4.4f, actpathtime=%4.4f, choiceprob=%4.4f, edges=' 
                                        %(path.label, path.pathflow, path.actpathtime, path.choiceprob))
                        for item in path.Edges:
                            foutpath.write('%s, ' %(item.label))
                        
                    AssignedTrip[startVertex][endVertex] += path.pathflow
                    edges = path.Edges
                    AssignedVeh, AssignedTrip, vehID = assignVeh(verbose, net, startVertex, endVertex, edges, AssignedVeh,  AssignedTrip, vehID)
                if verbose:
                    foutpath.write('\n')
    if verbose:
        print 'total Number of the used paths for the current matrix:', TotalPath 
        foutpath.write('\ntotal Number of the used paths for the current matrix:%s' %TotalPath)
        foutpath.close()
        fouterror.close()
    return AssignedVeh, AssignedTrip, vehID

           
def assignVeh(verbose, net, startVertex, endVertex, path, AssignedVeh, AssignedTrip, vehID):
    edges = path
    while AssignedVeh[startVertex][endVertex] < int(round(AssignedTrip[startVertex][endVertex])):
        vehID += 1
        newVehicle = net.addVehicle(str(vehID))
        newVehicle.route = edges
        AssignedVeh[startVertex][endVertex] += 1
    if verbose:
        print 'vehID:', vehID
        print 'AssignedTrip[start][end]', AssignedTrip[startVertex][endVertex]
        print 'AssignedVeh[start][end]', AssignedVeh[startVertex][endVertex]
    
    return AssignedVeh, AssignedTrip, vehID

def getThetaForCLogit(ODPaths):
    sum = 0.
    diff = 0.
    minpath = min(ODPaths, key=operator.attrgetter('actpathtime'))
    
    for path in ODPaths:
        sum += path.actpathtime
    
    meanpathtime = sum / float(len(ODPaths))
    
    for path in ODPaths:
        diff += (path.actpathtime - meanpathtime)**2.

    sdpathtime = (diff/float(len(ODPaths)))**0.5

    if sdpathtime > 0.04:
        theta = math.pi / (pow(6.,0.5) * sdpathtime * minpath.actpathtime)
    else:
        theta = 1.

    return theta
    
def doLohseStopCheck(net, verbose, stable, iter, maxIter, Parcontrol, foutlog):
    cvg1 = float(Parcontrol[5])                              # parameter for determining the modification degree of link flows at each iteration in SUE
    cvg2 = float(Parcontrol[6])                              # parameter for determining the modification degree of link flows at each iteration in SUE
    cvg3 = float(Parcontrol[7])
    stable = False
    if iter > 1 :                                        # Check if the convergence reaches.
        counts = 0    
        for edge in net._edges.itervalues():
            stop = edge.stopCheck(verbose, cvg1, cvg2, cvg3)
            if stop: 
                counts += 1
        if counts == len(net._edges):
            stable = True
            foutlog.write('The defined convergence is reached. The number of the required iterations:%s\n' %iter)
        elif counts < int(len(net._edges)*0.05) and iter > 50:
            stable = True
            foutlog.write('The number of the links with convergence is 95% of the total links. The number of executed iterations:%s\n' %iter)

    if iter >= maxIter:
        print 'The max. number of iterations is reached!'
        foutlog.write('The max. number(%s) of iterations is reached!\n' %iter)
        foutlog.write('The number of new routes will be set to 0, since the max. number of iterations is reached.')
        stable = True
        print 'stop?:', stable
        print 'iter_inside:', iter
    return stable

def getLinkChoiceProportions(curvefile, verbose, net, matrixPshort, Parcontrol, startVertices, endVertices, linkChoiceProportions, foutlog):
    if verbose:
        print 'begin the "getLinkChoiceProportions - SUE"!'
    # initialization for the traffic assignment
    iter_outside = 1
    newRoutes = 1
    stable = False
    first =True
    
    lohse = False
    incremental = False
    clogit = False
    checkKPaths = False
    
    if Parcontrol[(len(Parcontrol)-1)] == "0":
        incremental = True
    elif Parcontrol[(len(Parcontrol)-1)] == "1":
        lohse = True
    elif Parcontrol[(len(Parcontrol)-1)] == "2":
        clogit = True
    if not incremental:
        if int(Parcontrol[9]) > 1:
            checkKPaths = True
            KPaths = int(Parcontrol[9])
        
    if clogit:
        maxIteration = int(Parcontrol[7])
    elif lohse:
        maxIteration = int(Parcontrol[8])
    elif incremental:
        iterations = int(Parcontrol[0])
        
    if clogit or lohse:
        foutlog.write('- SUE assignment is adopted.\n')
        while newRoutes > 0:
            iter_inside = 1
            # Generate the effective routes als intital path solutions, when considering k shortest paths (k is defined by the user.)
            if checkKPaths:
                newRoutes = net.calcKPaths(verbose, newRoutes, KPaths, startVertices, endVertices, matrixPshort)
                foutlog.write('- Finding the k-shortest paths for each OD pair: done.\n')

            elif not checkKPaths and iter_outside == 1 and counter == 0:
                newRoutes = findNewPath(startVertices, endVertices, net, newRoutes, matrixPshort, lohse)
            
            checkKPaths = False
              
            stable = False            
            while not stable:
                if verbose:
                    print 'iteration (inside):', iter_inside
                        
                # The matrixPlong and the matrixTruck should be added when considering the long-distance trips and the truck trips.
                stable = doSUEAssign(curvefile, verbose, Parcontrol, net, startVertices, endVertices, matrixPshort, iter_inside, lohse, first)
                if lohse:
                    stable = doLohseStopCheck(net, verbose, stable, iter_inside, maxIteration, Parcontrol, foutlog)
                iter_inside += 1
                
                newRoutes = findNewPath(startVertices, endVertices, net, newRoutes, matrixPshort, lohse)
                
                if verbose:
                    print 'stable:', stable
            
            first = False
            iter_outside += 1
            
            if newRoutes < 5 and iter_outside > 10:
                newRoutes = 0
                
            if iter_outside > maxIteration:
                print 'The max. number of iterations is reached!'
                foutlog.write('The max. number of iterations is reached!\n')
                foutlog.write('The number of new routes and the parameter stable will be set to zero and True respectively.\n')
                print 'newRoutes:', newRoutes 
                stable = True
                newRoutes = 0
        
        linkChoiceProportions = calLinkChoiceProportion(self, net, Parcontrol, startVertices, endVertices, linkChoiceProportions, lohse)
        if verbose:
            print 'calLinkChoiceProportion - SUE is done!'
    else:
        if verbose:
            print 'begin the "getLinkChoiceProportions - incremental"!'
        foutlog.write('- incremetal assignment is adopted.\n')
        iter = 0
        print 'iterations for the incremental assignment:', iterations
        while iter < iterations:
            foutlog.write('- Current iteration(not executed yet):%s\n' %iter)
            iter += 1
            
            findNewPath(startVertices, endVertices, net, newRoutes, matrixPshort, lohse)
            for start, startVertex in enumerate(startVertices):
                for end, endVertex in enumerate(endVertices):
                    if str(startVertex) != str(endVertex) and (matrixPshort[start][end] > 0.0):
                        ODPaths = net._paths[startVertex][endVertex]
                        for path in ODPaths:
                            if path.currentshortest:
                                shortestpath = path
                                
                        pathflow = float(matrixPshort[start][end]*float(Parcontrol[iter]))
                        shortestpath.pathflow += pathflow
                        
                        for edge in shortestpath.Edges:
                            edge.flow += pathflow
                            if edge.detected:
                                linkChoiceProportions[edge.label][startVertex][endVertex] += pathflow/matrixPshort[start][end]

            for edgeID in net._edges:
                edge = net._edges[edgeID]
                edge.getActualTravelTime(curvefile)
    foutlink = file('linkproportion.txt', 'a')
    
    for edge in net._edges.itervalues():
        if edge.detected:
            foutlink.write('flow on Edge %s is detected.\n' %edge.label)
            for start, startVertex in enumerate(startVertices):
                for end, endVertex in enumerate(endVertices):
                    if str(startVertex) != str(endVertex) and (matrixPshort[start][end] > 0.0):
                        foutlink.write('linkChoiceProportions[%s][%s][%s]=%s\n' %(edge.label, startVertex, endVertex, linkChoiceProportions[edge.label][startVertex][endVertex]))
    foutlink.close()            
    return linkChoiceProportions
                
def calLinkChoiceProportion(verbose, net, Parcontrol, startVertices, endVertices, linkChoiceProportions, lohse):
    if verbose:
        print 'calLinkChoiceProportion - SUE is done!'

    for start, startVertex in enumerate(startVertices):
        for end, endVertex in enumerate(endVertices):
            pathcount = 0
            cumulatedflow = 0.
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex):
                ODPaths = net._paths[startVertex][endVertex]
                
                for path in ODPaths:
                    path.getPathTimeUpdate(net)
                    if lohse:
                        path.helpacttime = path.actpathtime
      
                calCommonalityAndChoiceProb(net, ODPaths, Parcontrol, lohse)
        
                for path in ODPaths:
                    pathcount += 1
                    if pathcount < len(ODPaths):
                        path.pathflow = matrixPshort[start][end] * path.choiceprob
                        cumulatedflow += path.pathflow
                    else:
                        path.pathflow = matrixPshort[start][end] - cumulatedflow
                    
                    for edge in path.Edges:
                        edge.flow += path.pathflow
                        if edge.detected:
                            linkChoiceProportions[edge.label][startVertex][endVertex] += path.pathflow/matrixPshort[start][end]
    
    return linkChoiceProportions
