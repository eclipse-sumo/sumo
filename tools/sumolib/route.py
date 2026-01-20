# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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
import sys
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


def _getMinPath(paths, detoursOut=None):
    minDist = 1e400
    minPath = None
    minDetours = None
    for path, (dist, _, detours) in paths.items():
        if dist < minDist:
            minPath = path
            minDist = dist
            minDetours = detours
    if detoursOut is not None:
        detoursOut += minDetours
    return minPath


def mapTrace(trace, net, delta, verbose=False, airDistFactor=2, fillGaps=0, gapPenalty=-1,
             debug=False, direction=False, vClass=None, vias=None, reversalPenalty=0.,
             fastest=False, detourWarnFactor=1e400):
    """
    matching a list of 2D positions to consecutive edges in a network.
    The positions are assumed to be dense (i.e. covering each edge of the route) and in the correct order.
    """
    result = ()
    resultDetours = []
    paths = {}  # maps a path stub to a tuple (currentCost, posOnLastEdge, detours)
    lastPos = None
    nPathCalls = 0
    nNoCandidates = 0
    if verbose:
        print("mapping trace with %s points ... " % len(trace), end="", flush=True)
    for idx, pos in enumerate(trace):
        x, y = pos
        newPaths = {}
        if vias and idx in vias:
            candidates = []
            for edgeID in vias[idx]:
                if net.hasEdge(edgeID):
                    candidates.append((net.getEdge(edgeID), 0.))
                else:
                    print("Unknown via edge %s for %s,%s" % (edgeID, x, y))
            # print("idx %s: vias=%s, candidates=%s (%s)" % (idx, len(vias[idx]),
            #    len(candidates), [ed[0].getID() for ed in candidates]))
        else:
            candidates = net.getNeighboringEdges(x, y, delta, False)
        if debug:
            print("\n\npos:%s, %s" % (x, y))
            print("candidates:%s\n" % [(e.getID(), c) for e, c in candidates])
        if verbose and not candidates:
            if nNoCandidates == 0:
                print()
            print("   Found no candidate edges for %s,%s (index %s) " % (x, y, idx))
            nNoCandidates += 1

        for edge, d in candidates:
            if vClass is not None and not edge.allows(vClass):
                continue
            base = polygonOffsetWithMinimumDistanceToPoint(pos, edge.getShape())
            base *= edge.getLengthGeometryFactor()
            if paths:
                advance = euclidean(lastPos, pos)  # should become a vector
                bestLength = 1e400 # length of the best path (not necessarily the shortest)
                minDist = 1e400
                minPath = None
                for path, (dist, lastBase, detours) in paths.items():
                    pathLength = None
                    if debug:
                        print("*** extending path %s by edge '%s' (d=%s)" %
                              ([e.getID() for e in path], edge.getID(), d))
                        print("              lastBase: %.2f, base: %.2f, advance: %.2f, old dist: %.2f, minDist: %.2f" %
                              (lastBase, base, advance, dist, minDist))
                    if dist < minDist:
                        if edge == path[-1] and base > lastBase:
                            pathLength = base - lastBase
                            pathCost = pathLength
                            if fastest:
                                pathCost /= edge.getSpeed()
                            baseDiff = advance - pathCost
                            extension = ()
                            if debug:
                                print("---------- same edge")
                        else:
                            penalty = airDistFactor * advance if gapPenalty < 0 else gapPenalty
                            maxGap = min(penalty + edge.getLength() + path[-1].getLength(), fillGaps)
                            extension, cost = net.getOptimalPath(path[-1], edge, maxCost=maxGap,
                                                                 fastest=fastest,
                                                                 reversalPenalty=reversalPenalty,
                                                                 fromPos=lastBase, toPos=base, vClass=vClass)
                            nPathCalls += 1
                            if extension is None:
                                airLineDist = euclidean(
                                    path[-1].getToNode().getCoord(),
                                    edge.getFromNode().getCoord())
                                pathCost = path[-1].getLength() - lastBase + base + airLineDist + penalty
                                pathLength = pathCost
                                baseDiff = abs(lastBase + advance -
                                               path[-1].getLength() - base - airLineDist) + penalty
                                extension = (edge,)
                            else:
                                pathCost = cost
                                baseDiff = advance - pathCost
                                extension = extension[1:]
                                pathLength = sum([e.getLength() for e in extension[:-1]]) - lastBase + base
                            if debug:
                                print("---------- extension path: %s, cost: %.2f, pathCost: %.2f" %
                                      (" ".join([e.getID() for e in extension]),
                                          cost, pathCost))
                        dist += d * d + pathCost
                        if direction:
                            dist += baseDiff * baseDiff
                        if dist < minDist:
                            minDist = dist
                            minPath = path + extension
                            bestLength = pathLength
                        if debug:
                            print("*** new dist: %.2f baseDiff: %.2f minDist: %.2f" % (dist, baseDiff, minDist))
                if minPath:
                    newPaths[minPath] = (minDist, base, detours + [bestLength / advance if advance > 0 else 0])
            else:
                #  the penality for picking a departure edge that is further away from pos
                #  must outweigh the distance that is saved by picking an edge
                #  that is closer to the subsequent pos
                if debug:
                    print("*** origin %s d=%s base=%s" % (edge.getID(), d, base))
                newPaths[(edge,)] = (d * 2, base, [0])
        if not newPaths:
            # no mapping for the current pos, the route may be disconnected or the radius is too small
            if paths:
                minPath = _getMinPath(paths, resultDetours)
                if len(result) > 0 and minPath[0] in result:
                    cropIndex = max([i for i in range(len(minPath)) if minPath[i] in result])
                    minPath = minPath[cropIndex+1:]
                result += minPath
                resultDetours.append(0)
        paths = newPaths
        lastPos = pos
    if verbose:
        if nNoCandidates > 0:
            print("%s Points had no candidates. " % nNoCandidates, end="")
        print("(%s router calls)" % nPathCalls)
    if paths:
        result += _getMinPath(paths, resultDetours)
        assert(len(resultDetours) == len(trace))
        if detourWarnFactor is not None:
            for i in range(1, len(trace)):
                detour = resultDetours[i]
                if detour > detourWarnFactor:
                    airLine = euclidean(trace[i - 1], trace[i])
                    print("Large detour (%s) to trace index %s, fromPos=%s, toPos=%s (airLine=%s path=%s)" %
                            (detour, i, trace[i - i], trace[i], airLine, detour * airLine), file=sys.stderr)

        if debug:
            print("**************** paths:")
            for edges, (cost, base, _) in paths.items():
                print(cost, base, " ".join([e.getID() for e in edges]))
            print("**************** result:")
            for i in result:
                print("path:%s" % i.getID())
    return result
