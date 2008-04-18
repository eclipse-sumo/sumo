"""
@file    dijkstra.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-10-25
@version $Id$

This script is based on the script from David Eppstein, UC Irvine.
This script is to find the shortest path from the given origin 'start' to the other nodes in the investigated network. 
The Dijkstra algorithm is used for searching the respective shortest paths. 
the link information about the shortest paths and the corresponding travel times   
will be stored in the lists P and D respectively.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys

from priodict import priorityDictionary

def dijkstra(start):
    # dictionary of final distances
    D = {}
    # dictionary of predecessors
    P = {}
    # est.dist. of non-final vert.
    Q = priorityDictionary()
    Q[start] = 0
  
    for v in Q:
        D[v] = Q[v]
           
        for edge in v.outEdges:
            w = edge.target
            vwLength = D[v] + edge.actualtime + edge.queuetime
            if w in D:
                if vwLength < D[w]:
                        raise ValueError, \
    "Dijkstra: found better path to already-final vertex"
            elif w not in Q or vwLength < Q[w]:
                Q[w] = vwLength
                P[w] = v
    return (D, P)
    
def dijkstraForLohse(start):
    D = {}	# dictionary of final distances
    P = {}	# dictionary of predecessors
    Q = priorityDictionary()   # est.dist. of non-final vert.
    Q[start] = 0
#    print 'start=', start   
    for v in Q:
        D[v] = Q[v]
#        print 'v=', v           
        for edge in v.outEdges:
            w = edge.target
            vwLength = D[v] + edge.helpacttime
            if w in D:
                if vwLength < D[w]:
                        raise ValueError, \
    "Dijkstra: found better path to already-final vertex"
            elif w not in Q or vwLength < Q[w]:
                Q[w] = vwLength
                P[w] = v
    return (D, P)