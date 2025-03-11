# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    route.py
# @author  Michael Behrisch
# @author  Mirko Barthauer
# @date    2013-10-23

from __future__ import print_function
from .miscutils import euclidean
from .geomhelper import polygonOffsetWithMinimumDistanceToPoint

try:
    basestring
    # Allows isinstance(foo, basestring) to work in Python 3
except NameError:
    basestring = str


def getLength(net, edges):
    """
    Calculates the length of a route including internal edges.
    The input network has to contain internal edges (withInternal needs to be set when parsing).
    The list of edges can either contain edge objects or edge ids as strings.
    If there is no connection between two consecutive edges, length 0 is assumed (no error is thrown).
    If there are multiple connections of different length, the shortest is used.
    """
    if len(edges) == 0:
        return 0
    if isinstance(edges[0], basestring):
        edges = [net.getEdge(e) for e in edges]
    last = edges[0]
    length = last.getLength()
    for e in edges[1:]:
        if net.hasInternal:
            viaPath, minInternalCost = net.getInternalPath(last.getConnections(e))
            if viaPath is not None:
                length += minInternalCost
        length += e.getLength()
        last = e
    return length


def addInternal(net, edges):
    """
    Returns a list of edges of a route including internal edges.
    The input network has to contain internal edges (withInternal needs to be set when parsing).
    The list of input edges can either contain edge objects or edge ids as strings.
    The return value will always contain edge objects.
    If there is no connection between two consecutive edges no internal edge is added.
    If there are multiple connections between two edges, the shortest one is used.
    """
    if len(edges) == 0:
        return []
    if isinstance(edges[0], basestring):
        edges = [net.getEdge(e) for e in edges]
    last = edges[0]
    result = [last]
    for e in edges[1:]:
        if net.hasInternal:
            viaPath, _ = net.getInternalPath(last.getConnections(e))
            if viaPath is not None:
                result += viaPath
        result.append(e)
        last = e
    return result


def _getMinPath(paths):
    minDist = 1e400
    minPath = None
    for path, (dist, _) in paths.items():
        if dist < minDist:
            minPath = path
            minDist = dist
    return minPath


def mapTrace(trace, net, delta, verbose=False, airDistFactor=2, fillGaps=0, gapPenalty=-1,
             debug=False, direction=False, vClass=None, vias=None, reversalPenalty=0.):
    """
    matching a list of 2D positions to consecutive edges in a network.
    The positions are assumed to be dense (i.e. covering each edge of the route) and in the correct order.
    """
    result = ()
    paths = {}  # maps a path stub to a pair of current cost and the last mapping position on the last edge
    lastPos = None
    if verbose:
        print("mapping trace with %s points" % len(trace))
    for idx, pos in enumerate(trace):
        newPaths = {}
        if vias and idx in vias:
            if net.hasEdge(vias[idx]):
                candidates = [(net.getEdge(vias[idx]), 0.)]
            else:
                print("Unknown via edge %s for %s,%s" % (vias[idx], pos[0], pos[1]))
                candidates = []
        else:
            candidates = net.getNeighboringEdges(pos[0], pos[1], delta, not net.hasInternal)
        if debug:
            print("\n\npos:%s, %s" % (pos[0], pos[1]))
            print("candidates:%s\n" % [(e.getID(), c) for e, c in candidates])
        if verbose and not candidates:
            print("Found no candidate edges for %s,%s" % pos)

        for edge, d in candidates:
            if vClass is not None and not edge.allows(vClass):
                continue
            base = polygonOffsetWithMinimumDistanceToPoint(pos, edge.getShape())
            if paths:
                advance = euclidean(lastPos, pos)  # should become a vector
                minDist = 1e400
                minPath = None
                for path, (dist, lastBase) in paths.items():
                    if debug:
                        print("*** extending path %s by edge '%s'" % ([e.getID() for e in path], edge.getID()))
                        print("              lastBase: %.2f, base: %.2f, advance: %.2f, old dist: %.2f, minDist: %.2f" %
                              (lastBase, base, advance, dist, minDist))
                    if dist < minDist:
                        if edge == path[-1] and base > lastBase:
                            pathLength = base - lastBase
                            baseDiff = advance - pathLength
                            extension = ()
                            if debug:
                                print("---------- same edge")
                        else:
                            penalty = airDistFactor * advance if gapPenalty < 0 else gapPenalty
                            maxGap = min(penalty + edge.getLength() + path[-1].getLength(), fillGaps)
                            extension, cost = net.getOptimalPath(path[-1], edge, maxCost=maxGap,
                                                                 reversalPenalty=reversalPenalty,
                                                                 fromPos=lastBase, toPos=base)
                            if extension is None:
                                airLineDist = euclidean(
                                    path[-1].getToNode().getCoord(),
                                    edge.getFromNode().getCoord())
                                pathLength = path[-1].getLength() - lastBase + base + airLineDist + penalty
                                baseDiff = abs(lastBase + advance -
                                               path[-1].getLength() - base - airLineDist) + penalty
                                extension = (edge,)
                            else:
                                pathLength = cost
                                baseDiff = advance - pathLength
                                extension = extension[1:]
                            if debug:
                                print("---------- extension path: %s, cost: %.2f, pathLength: %.2f" %
                                      (extension, cost, pathLength))
                        dist += d * d + pathLength
                        if direction:
                            dist += baseDiff * baseDiff
                        if dist < minDist:
                            minDist = dist
                            minPath = path + extension
                        if debug:
                            print("*** new dist: %.2f baseDiff: %.2f minDist: %.2f" % (dist, baseDiff, minDist))
                if minPath:
                    newPaths[minPath] = (minDist, base)
            else:
                newPaths[(edge,)] = (d * d, base)
        if not newPaths:
            # no mapping for the current pos, the route may be disconnected or the radius is too small
            if paths:
                minPath = _getMinPath(paths)
                if len(result) > 0 and minPath[0] in result:
                    cropIndex = max([i for i in range(len(minPath)) if minPath[i] in result])
                    minPath = minPath[cropIndex+1:]
                result += minPath
        paths = newPaths
        lastPos = pos
    if paths:
        if debug:
            print("**************** result:")
            for i in result + _getMinPath(paths):
                print("path:%s" % i.getID())
        return result + _getMinPath(paths)
    return result
