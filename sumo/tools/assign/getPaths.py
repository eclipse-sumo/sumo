"""
@file    getPaths.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-02-01
@version $Id$

This script is find the new paths for all OD pairs.
The Dijkstra algorithm is applied for searching the shortest paths.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys
from dijkstra import dijkstra
from elements import Vertex, Edge, Path, pathNum

def findNewPath(startVertices, endVertices, net, newRoutes, matrixPshort, lohse):
    newRoutes = 0
    for start, startVertex in enumerate(startVertices):
        D,P = dijkstra(startVertex, lohse)            
        for end, endVertex in enumerate(endVertices):
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex):
                tempPath = []
                helpPath = []
                pathcost = D[endVertex]/3600.
                ODPaths = net._paths[startVertex][endVertex]
                for path in ODPaths:
                    path.currentshortest = False
                    
                vertex = endVertex
                while vertex != startVertex:
                    if P[vertex].kind == "real":
                        helpPath.append(P[vertex])
                    vertex = P[vertex].source
                helpPath.reverse()

                newPath = True
                notpath = False                    
                if len(ODPaths) > 0:
                    for path in ODPaths:
                        if path.Edges != helpPath:
                            sameEdgeCount = 0
                            sameTravelTime = 0.0
                            for pos, edge in enumerate(helpPath):
                                if pos < len(path.Edges) and path.Edges[pos] == edge:
                                    sameEdgeCount += 1 
                                    sameTravelTime += edge.actualtime
                            if abs(sameEdgeCount - len(path.Edges))/len(path.Edges) <= 0.1 and abs(sametraveltime/3600. - pathcost) <= 0.05:
                                notpath = True
                                newPath = False
                                break
                if newPath:
                    newpath = Path()
                    ODPaths.append(newpath)
                    newpath.source = startVertex
                    newpath.target = endVertex
                    newpath.currentshortest = True
                    if lohse:
                        newpath.pathhelpacttime = pathcost
                    else:    
                        newpath.actpathtime = pathcost
                    newpath.Edges = helpPath
                    newpath.usedcounts += 1
                    for edge in newpath.Edges:
                        newpath.freepathtime += edge.freeflowtime
                    newRoutes += 1
                elif not newPath and not notpath:
                    if lohse:
                        samePath.pathhelpacttime = pathcost
                    else:
                        samePath.actpathtime = pathcost
                    samePath.usedcounts += 1
                    samePath.currentshortest = True
    
    return newRoutes