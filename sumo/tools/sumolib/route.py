#!/usr/bin/env python
"""
@file    route.py
@author  Michael Behrisch
@date    2013-10-23
@version $Id$

Route helper functions.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

def _getMinPath(paths):
    minDist = 1e400
    minPath = None
    for path, dist in paths.iteritems():
        if dist < minDist:
            minPath = path
            minDist = dist
    return minPath

def mapTrace(trace, net, delta):
    """
    matching a list of 2D positions to consecutive edges in a network
    """
    result = []
    paths = {}
    for pos in trace:
        newPaths = {}
        for edge, d in net.getNeighboringEdges(pos[0], pos[1], delta):
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
                if minPath:
                    newPaths[minPath] = minDist + d * d
            else:
                newPaths[(edge,)] = d * d
        if not newPaths:
            if paths:
                result += [e.getID() for e in _getMinPath(paths)]
                result.append("*")
            if not result:
                result.append("*")
        paths = newPaths
    if paths:
        return result + [e.getID() for e in _getMinPath(paths)]
    return result
