"""
@file    glsMatrixEstimation.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-04-21
@version $Id: glsMatrixEstimation.py 5458 2008-04-18 20:18:21Z behrisch $

This script is to execute the generalized least squared model, proposed by Cascetta(1984).

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime, math, operator, numpy
from elements import Predecessor, Vertex, Edge, Path, Vehicle
from network import Net
from numpy import *
from numpy.linalg import inv

def doMatrixEstimation(net, verbose, Parcontrol, startVertices, endVertices, matrixPshort, linkChoiceProportions, daytimeindex, odtype):
    historicalMatrixArray = array(())
    sampleMatrixArray = array(())
    estimatedMatrix = zeros((len(startVertices), len(endVertices)))
    
    sumSampleMatrix = 0.
    sumHistoricalMatrix = 0.
    matrixElements = 0
    minimalTrips = array((0))
    finished = False
    
    start = -1
    for startVertex in startVertices:
        start += 1
        end = -1
        for endVertex in endVertices:
            end += 1
            if str(startVertex) != str(endVertex) and matrixPshort[start][end] > 0.:
                matrixElements += 1
                historicalMatrixArray = append(historicalMatrixArray, [[matrixPshort[start][end]]])
                sampleMatrixArray = append(sampleMatrixArray, [[matrixPshort[start][end]]])
    
    varSampleMatrixArray = zeros((matrixElements, matrixElements))
    estMatrixArray = zeros((matrixElements, 1))
    muArray = zeros((matrixElements, 1))
            
    if float(Parcontrol[10]) != 1.:
        sampleMatrixArray *= float(Parcontrol[10])
    
    sumSampleMatrix = sum(sampleMatrixArray)
    sumHistoricalMatrix = sum(historicalMatrixArray)
    
    print 'len(startVertices):', len(startVertices)
    print 'historicalMatrixArray-shape:', historicalMatrixArray.shape
    print 'sampleArray-shape:', sampleArray.shape
    print ' estMatrixArray-shape:',  estMatrixArray.shape
    
    # calculate the variance matrix of the sample matrix - matrix Q
    if float(Parcontrol[10]) != 1.:
        elem = ((sumSampleMatrix-sumHistoricalMatrix)*sumSampleMatrix)/((sumHistoricalMatrix-1)*pow(sumHistoricalMatrix,2.))
        for i in range (0, matrixElements):
            # equation: Var[tij]=(T-N)*T/(N-1)N2*nij*(N-nij)
            varSampleMatrixArray[i][i]= elem * historicalMatrixArray[i] * (sumHistoricalMatrix - historicalMatrixArray[i])
                    
    # calculate the variance matrices of link counts matrices
    varFlowMatrixArray, detectedFlowArray = getDetectedLinkFlowAndVariance(net, startVertices, endVertices, daytimeindex, odtype)
  
    # calculate proportional matrix A or get it from the assignment model
    linkChoiceArray = getLinkChoiceArray(net, startVertices, endVertices, matrixPshort, linkChoiceProportions, matrixElements)
    
    inv_varSampleMatrixArray = inv(varSampleMatrixArray)
    tran_linkChoiceArray = transpose(linkChoiceArray)
    inv_varFlowMatrixArray = inv(varFlowMatrixArray)

    ProductFlowMatrix = dot(tran_linkChoiceArray, inv_varFlowMatrixArray)

    # calculate dMatrix (D): D = inverse(Q)+transpose(A)*inverse(W)*A
    dMatrix = inv_varSampleMatrixArray + dot(ProductFlowMatrix, linkChoiceArray)
    inv_dMatrix = inv(dMatrix)

    tran_sampleMatrixArray = sampleMatrixArray.reshape(matrixElements, 1)
    
    sumFlowMatrix = dot(inv_varSampleMatrixArray, tran_sampleMatrixArray) + dot(ProductFlowMatrix, detectedFlowArray)
    # estimate the matrix by using t =inverse(D)(inverse(Q) * n + transpose(A) * inverse(W) * V+ mu) with mu for the condition equations t >= 0.
    while not finished:
        notRight = 0
        estMatrixArray = dot(inv_dMatrix, (sumFlowMatrix + muArray))
        for i in range (0, matrixElements):
            muArray[i] = muArray[i] + max(minimalTrips, (minimalTrips - estMatrixArray[i])/inv_dMatrix[i][i])
            if estMatrixArray[i] < 0.:
                notRight += 1
        if notRight == 0:
            finished = True

    # reconstruct the matrix format
    start = -1
    elementcounts = -1
    for startVertex in startVertices:
        start += 1
        end = -1
        for endVertex in endVertices:
            end += 1
            if str(startVertex) != str(endVertex) and matrixPshort[start][end] > 0.:
                elementcounts += 1
                estimatedMatrix[start][end] = estMatrixArray[elementcounts]    
            
    return estimatedMatrix

def getDetectedLinkFlowAndVariance(net, startVertices, endVertices, daytimeindex, odtype):
    linkcounts = net._detectedLinkCounts
    detectedFlowArray = array(())
    linkoder = -1
    varFlowMatrixArray = zeros((linkcounts, linkcounts))
    for edge in net._edges.itervalues():
        if edge.detected:
            flowlist = array(())
            linkoder += 1
            for data in edge._detecteddata.itervalues():
                if str(data.label) == daytimeindex and odtype == "passenger":
                    flowlist = append(flowlist, [[data.flowPger]])
                    
                elif str(data.label) == daytimeindex and odtype == "truck":
                    flowlist = append(flowlist, [[data.flowTruck]])
            if len(flowlist) > 1:
                detectedFlowArray = append(detectedFlowArray, [[average(flowlist)]])
                varFlowMatrixArray[linkorder][linkorder] = (var(flowlist)* float(count(flowlist)))/(float(count(flowlist)) - 1.)
            else:
                detectedFlowArray = append(detectedFlowArray, [[flowlist]])
                varFlowMatrixArray[linkoder][linkoder] = (flowlist[0]* 0.1/1.96)**2
                # according to the Traffic Appraisal Manual of the UK Department(1998), 
                # a 95% confidence intercal for an unclasified manual count is about +- 10%.
                # The assumption of an approximately normal sampling distribution is also made and the respective variance is (flow*10%/1.96)^2.
    detectedFlowArray = detectedFlowArray.reshape((linkcounts, 1))

    return varFlowMatrixArray, detectedFlowArray

def getLinkChoiceArray(net, startVertices, endVertices, matrixPshort, linkChoiceProportions, matrixElements):
    linkcounts = net._detectedLinkCounts
    linkChoiceArray = zeros((linkcounts, matrixElements))

    edgestart = -1
    for edge in net._edges.itervalues():
        start = -1
        if edge.detected:
            edgestart += 1
            odpair = -1
            for startVertex in startVertices:
                start += 1
                end = -1
                for endVertex in endVertices:
                    end += 1
                    if str(startVertex) != str(endVertex) and matrixPshort[start][end] > 0.:
                        odpair += 1
                        linkChoiceArray[edgestart][odpair] = linkChoiceProportions[edge.label][startVertex][endVertex]
                        
    return linkChoiceArray

def initialLinkChoiceProportionsMatrix(net, startVertices, endVertices, matrixPshort):
    linkChoiceProportions = {}
    for edge in edge._edges:
        if edge.detected:
            linkChoiceProportions[edge] = {}
            start = -1
            for startVertex in startVertices:
                start += 1
                end = -1
                linkChoiceProportions[edge][startVertex]={}
                for endVertex in endVertices:
                    end += 1
                    if str(startVertex) != str(endVertex) and matrixPshort[start][end] > 0.:
                        linkChoiceProportions[edge][startVertex][endVertex] = 0.
    return linkChoiceProportions
    
def getHourlyDetectedFlow(net, daytimetype, odtype):
    for edge in net._edges.itervalue():
        edge.detecteddata
        