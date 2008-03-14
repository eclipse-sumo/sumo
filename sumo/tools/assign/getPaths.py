# python
# execute the incremetal traffic assignment with the given amount of OD demand
# the path information and the path travel time at each iteration will be stored in the file "path.txt"

import os, random, string, sys
from dijkstra import Dijkstra                                                 # import the Dijkstra algorithm for searching shortest paths

from elements import Vertex, Edge, Path, pathNum                              # import the characteristics of Vertices, Edges and paths

def findNewPath(startVertices, endVertices, net, iter, NewRoutes, matrixPshort):
    NewRoutes = 0
    start = -1
    for startVertex in startVertices:
        start += 1
        end = -1
        D,P = Dijkstra(startVertex)
        for endVertex in endVertices:
            end += 1
            endnode = endVertex
            if matrixPshort[start][end] > 0. and str(startVertex) != str(endVertex): 
                tempPath = []
                helpPath = []
                pathcost = 0.0
                while 1:
                    tempPath.append(endnode)
                    if endnode == startVertex: 
                        break
                    endnode = P[endnode]
                tempPath.reverse()                                              # the path set will be generated regarding to the given destination "endVertex"
                for i in range(0, len(tempPath)):   
                    if tempPath[i] != endVertex:
                        node = tempPath[i]
                        for edge in node.outEdges:
                            if str(tempPath[i]) != str(tempPath[i+1]) and str(edge.source) == str(tempPath[i]) and str(edge.target) == str(tempPath[i+1]):
                                helpPath.append(edge)
                    else:
                        pathcost = D[endVertex]

                ODPaths = net._paths[startVertex][endVertex]
                NewPath = True                    
                for path in ODPaths:
                    sameEdgeCount = 0                                           
                    if len(path.Edges) == len(helpPath):
                        for i in range (0, len(helpPath)):
                            if str(helpPath[i]) == str(path.Edges[i]):
                                sameEdgeCount += 1
                    if sameEdgeCount == len(path.Edges):
                        samePath = path
                        NewPath = False
                        break
                if NewPath:           # add the new path in the path set und update the path flows, path travel times and so on.
                    newpath = Path()
                    ODPaths.append(newpath)
                    newpath.source = startVertex
                    newpath.target = endVertex
                    newpath.actpathtime = pathcost
                    newpath.Edges = helpPath
                    for edge in newpath.Edges:
                        newpath.freepathtime += edge.freeflowtime
                    NewRoutes += 1
                else:
                    samePath.actpathtime = pathcost
    return NewRoutes