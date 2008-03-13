# this script is to calculate the commonality factors in SUE

import os, random, string, sys, math
import elements
from elements import Vertex, Edge, Path                                             # import the characteristics of Vertices, Edges and paths
from network import Net

def CalCommonality(net, ODPaths, Parcontrol):
    mtxOverlap = {}                                                                  
    for pathone in ODPaths:                                            # initialize the overlapping matrix
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
        for pathone in ODPaths:                                        # calculate the commonality factors (CF) for the given OD pair 
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
    
