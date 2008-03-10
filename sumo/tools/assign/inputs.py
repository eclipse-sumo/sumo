#!/usr/bin/env python
# This script is to retrieve the data about the control parameters, the names of the OD districts and the matrix. 
# Moreover the link travel time for traffic zone connectors will be estimated.

import os, random, string, sys, datetime

def getParameter(parfile):
    ODcontrol = []
    sum = 0.
    for line in open(parfile):                                                  # include: default link capacity estimation (0: no; 1: yes)
        ODcontrol = line.split()                                                        # the number of iterations, procent of matrix at each iteration
        for i in range (1, (len(ODcontrol)-2)):
            sum += float(ODcontrol[i])
    sum = sum * 100.0
    print 'percentage of the assigned matrix:', sum, "%" 
        
    return ODcontrol
     
def getMatrix(net, matrix, MatrixSum):#, mtxplfile, mtxtfile):
    matrixPshort = []
#    matrixPlong = []
#    matrixTruck = []
    startVertices = []
    endVertices = []
    Pshort_EffCells = 0
#    Plong_EffCells = 0
#    Truck_EffCells = 0

    print 'matrix:', str(matrix)                                 

    itemend = -1.0
    ODpairs = 0
    origins = 0
    dest= 0
    CurrentMatrixSum = 0.0
    skipCount = 0
    zones = 0
    for line in open(matrix):                             # read the matrix for passenger vehicles
        if line[0] != '*' and line[0] != '$':
            skipCount += 1
            print 'skipcount:',skipCount
            if skipCount > 3:
                if zones == 0:
                    for elem in line.split():
                        zones = int(elem)
                    print 'zones:', zones
                elif len(startVertices) < zones:
                    for elem in line.split():
                        haveStart = False
                        haveEnd = False
                        helperVertex = None
                        if len(elem) > 0:
                            for vertex in net._vertices:
                                if str(vertex.label) == str(elem):
                                    helperVertex = vertex
                                    if len(vertex.outEdges) > 0 and len(vertex.inEdges) == 0:
                                        startVertices.append(vertex)
                                        haveStart = True
                                    if len(vertex.inEdges) > 0 and len(vertex.outEdges) == 0:
                                        endVertices.append(vertex)
                                        haveEnd = True
                        if not haveStart:
                            startVertices.append(helperVertex)
                        if not haveEnd:
                            endVertices.append(helperVertex)
                    origins = len(startVertices)
                    dest = len(endVertices)        
                elif len(startVertices) == zones:
                    if itemend == -1.0:
                        matrixPshort.append([])
                        L = line.split('\n')
                        for item in line.split():
                            matrixPshort[-1].append(float(item))
                            ODpairs += 1
                            itemend = ODpairs%origins
                            MatrixSum += float(item)                  # calculate the sum of all matrices
                            if float(item) > 0.0:
                                Pshort_EffCells += 1
                    elif itemend == 0.:
                        matrixPshort.append([])
                        L = line.split('\n')
                        for item in line.split():
                            matrixPshort[-1].append(float(item))
                            ODpairs += 1
                            itemend = ODpairs%origins
                            MatrixSum += float(item)                  # calculate the sum of all matrices
                            if float(item) > 0.0:
                                Pshort_EffCells += 1
                    elif itemend != 0.:
                        for item in line.split():
                            matrixPshort[-1].append(float(item))
                            ODpairs += 1
                            itemend = ODpairs%origins
                            MatrixSum += float(item)                  # calculate the sum of all matrices
                            if float(item) > 0.0:
                                Pshort_EffCells += 1
    CurrentMatrixSum = MatrixSum

    print 'zones:', zones
    print 'Number of origins:', origins
#    print 'startVertices:', startVertices
    print 'Number of destinations:', dest
#    print 'endVertices:', endVertices 
#    print 'CurrentMatrixSum:', CurrentMatrixSum        
    print 'Effective O-D Cells:', Pshort_EffCells    
    
#    itemend = -1.0
#    ODpairs = 0
#    for line in open(mtxplfile):
#        if line[0] != '*' and itemend == -1.0:
#            matrixPlong.append([])
#            L = line.split('\n')
#            for item in line.split():
#                matrixPlong[-1].append(float(item))
#                ODpairs += 1
#                itemend = ODpairs%origins
#                if float(item) > 0.0:
#                    Plong_EffCells += 1
#        elif line[0] != '*' and itemend == 0.:
#            matrixPlong.append([])
#            L = line.split('\n')
#            for item in line.split():
#                matrixPlong[-1].append(float(item))
#                ODpairs += 1
#                itemend = ODpairs%origins
#                if float(item) > 0.0:
#                    Plong_EffCells += 1
#        elif line[0] != '*' and itemend != 0.:
#            for item in line.split():
#                matrixPlong[-1].append(float(item))
#                ODpairs += 1
#                itemend = ODpairs%origins
#                if float(item) > 0.0:
#                    Plong_EffCells += 1
#        elif line[0] == '*':
#            pass
    
#    itemend = -1.0
#    ODpairs = 0
#    for line in open(mtxtfile):
#        if line[0] != '*' and itemend == -1.0:
#            matrixTruck.append([])
#            L = line.split('\n')
#            for item in line.split():
#                matrixTruck[-1].append(float(item))
#                ODpairs += 1
#                itemend = ODpairs%origins
#                if float(item) > 0.0:
#                    Truck_EffCells += 1
#        elif line[0] != '*' and itemend == 0.:
#            matrixTruck.append([])
#            L = line.split('\n')
#            for item in line.split():
#                matrixTruck[-1].append(float(item))
#                ODpairs += 1
#                itemend = ODpairs%origins
#                if float(item) > 0.0:
#                    Truck_EffCells += 1
#        elif line[0] != '*' and itemend != 0.:
#            for item in line.split():
#               matrixTruck[-1].append(float(item))
#                ODpairs += 1
#                itemend = ODpairs%origins
#                if float(item) > 0.0:
#                    Truck_EffCells += 1
#        elif line[0] == '*':
#            pass   
    return matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum #, matrixPlong, matrixTruck, Plong_EffCells, Truck_EffCells  
    
def getConnectionTravelTime(startVertices, endVertices):
    sum = 0.0
    for vertex in startVertices:
        sum = 0.0
        for edge in vertex.outEdges:        
            sum += float(edge.weight)
        for edge in vertex.outEdges:
            edge.freeflowtime = (1-float(edge.weight)/sum) * 10        # assumption: all vehilces can reach the access links
                                                                       #             within 10 min from the respective traffic zone
            edge.actualtime = edge.freeflowtime
    for vertex in endVertices:
        sum = 0.0
        for edge in vertex.inEdges:
            sum += float(edge.weight)
        for edge in vertex.inEdges:
            edge.freeflowtime = (1-float(edge.weight)/sum) * 10        # assumption: all vehilces can reach the respective traffic zone 
                                                                       #             within 10 min from the access links   
            edge.actualtime = edge.freeflowtime
    return edge.actualtime, edge.freeflowtime
