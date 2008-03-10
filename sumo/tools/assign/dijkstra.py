#!/usr/bin/env python
# This script is to find the shortest path from the given origin 'start' to the other nodes in the investigated network. 
# The Dijkstra algorithm is used for searching the respective shortest paths. 
# the link information about the shortest paths and the corresponding travel times   
# will be stored in the lists P and D respectively.

import os, random, string, sys, datetime

from priodict import priorityDictionary

def Dijkstra(start):
    D = {}	# dictionary of final distances
    P = {}	# dictionary of predecessors
    Q = priorityDictionary()   # est.dist. of non-final vert.
    Q[start] = 0
        
    for v in Q:
        D[v] = Q[v]
          
        for edge in v.outEdges:
            w = edge.target
            vwLength = D[v] + edge.actualtime
            if w in D:
                if vwLength < D[w]:
                        raise ValueError, \
    "Dijkstra: found better path to already-final vertex"
            elif w not in Q or vwLength < Q[w]:
                Q[w] = vwLength
                P[w] = v
    return (D,P)