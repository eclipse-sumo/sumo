"""
@file    inputs.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-10-25
@version $Id$

This script is to retrieve the assignment parameters, the OD districts and the matrix from the input files. 
Moreover, the link travel time for district connectors will be estimated.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

# read the analyzed matrix         
def getMatrix(net, verbose, matrix, MatrixSum):#, mtxplfile, mtxtfile):
    matrixPshort = []
    startVertices = []
    endVertices = []
    Pshort_EffCells = 0
    periodList = []
    if verbose:
        print 'matrix:', str(matrix)                                 

    ODpairs = 0
    origins = 0
    dest= 0
    CurrentMatrixSum = 0.0
    skipCount = 0
    zones = 0
    for line in open(matrix):
        if line[0] == '$':
            visumCode = line[1:4]
            if visumCode != 'VMR':
                skipCount += 1
        elif line[0] != '*' and line[0] != '$':
            skipCount += 1
            if skipCount == 2:
                for elem in line.split():
                    periodList.append(float(elem))
            elif skipCount > 3:
                if zones == 0:
                    for elem in line.split():
                        zones = int(elem)
                elif len(startVertices) < zones:
                    for elem in line.split():
                        if len(elem) > 0:
                            for startVertex in net._startVertices:
                                if str(startVertex.label) == str(elem):
                                    startVertices.append(startVertex)
                            for endVertex in net._endVertices:
                                if str(endVertex.label) == str(elem):
                                    endVertices.append(endVertex)
                    origins = len(startVertices)
                    dest = len(endVertices)        
                elif len(startVertices) == zones:
                    if ODpairs % origins == 0:
                        matrixPshort.append([])
                    for item in line.split():
                        matrixPshort[-1].append(float(item))
                        ODpairs += 1
                        MatrixSum += float(item)
                        CurrentMatrixSum += float(item) 
                        if float(item) > 0.0:
                            Pshort_EffCells += 1
                            
    begintime = int(periodList[0])

    if verbose:
        print 'Number of zones:', zones
        print 'Number of origins:', origins
        print 'Number of destinations:', dest
        print 'begintime:', begintime
        print 'CurrentMatrixSum:', CurrentMatrixSum        
        print 'Effective O-D Cells:', Pshort_EffCells
        print 'len(net._startVertices):', len(net._startVertices)
        print 'len(net._endVertices):', len(net._endVertices)
    
    return matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum, begintime #, matrixPlong, matrixTruck, Plong_EffCells, Truck_EffCells  

# estimate the travel times on the district connectors
# assumption: all vehilces can reach the access links within 10 min from the respective traffic zone
def getConnectionTravelTime(startVertices, endVertices):
    for vertex in startVertices:
        sum = 0.0
        for edge in vertex.outEdges:     
            sum += float(edge.weight)
        for edge in vertex.outEdges:
            edge.freeflowtime = (1-float(edge.weight)/sum) * 10        
                                          
            edge.actualtime = edge.freeflowtime
    for vertex in endVertices:
        sum = 0.0
        for edge in vertex.inEdges:  
            sum += float(edge.weight)
        for edge in vertex.inEdges:
            edge.freeflowtime = (1-float(edge.weight)/sum) * 10 
                                              
            edge.actualtime = edge.freeflowtime
