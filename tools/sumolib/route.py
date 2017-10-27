#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    route.py
# @author  Michael Behrisch
# @date    2013-10-23
# @version $Id$


import os
import sys
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
from sumolib.miscutils import euclidean


def _getMinPath(paths):
    minDist = 1e400
    minPath = None
    for path, dist in paths.iteritems():
        if dist < minDist:
            minPath = path
            minDist = dist
    return minPath


def mapTrace(trace, net, delta, verbose=False):
    """
    matching a list of 2D positions to consecutive edges in a network
    """
    result = []
    paths = {}
    if verbose:
        print("mapping trace with %s points" % len(trace))
    for pos in trace:
        newPaths = {}
        candidates = net.getNeighboringEdges(pos[0], pos[1], delta)
        if len(candidates) == 0 and verbose:
            print("Found no candidate edges for %s,%s" % pos)
        for edge, d in candidates:
            if paths:
                minDist = 1e400
                minPath = None
                for path, dist in paths.iteritems():
                    if dist < minDist:
                        if edge == path[-1]:
                            minPath = path
                            minDist = dist
                        elif edge in path[-1].getOutgoing():
                            minPath = path + (edge,)
                            minDist = dist
                        else:
                            minPath = path + (edge,)
                            minDist = dist + euclidean(
                                path[-1].getToNode().getCoord(),
                                edge.getFromNode().getCoord())
                if minPath:
                    newPaths[minPath] = minDist + d * d
            else:
                newPaths[(edge,)] = d * d
        if not newPaths:
            if paths:
                result += [e.getID() for e in _getMinPath(paths)]
        paths = newPaths
    if paths:
        return result + [e.getID() for e in _getMinPath(paths)]
    return result
