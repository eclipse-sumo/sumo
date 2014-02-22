"""
@file    geomhelper.py
@author  Daniel Krajzewicz
@date    2013-02-25
@version $Id$

Some helper functions for geometrical computations.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import math

INVALID_DISTANCE = -1

def distance(p1, p2):
  dx = p1[0]-p2[0]
  dy = p1[1]-p2[1]
  return math.sqrt(dx*dx + dy*dy)


def lineOffsetWithMinimumDistanceToPoint(point, line_start, line_end, perpendicular=False):
    """Return the offset from line (line_start, line_end) where the distance to
    point is minimal"""
    p = point
    p1 = line_start
    p2 = line_end
    l = distance(p1, p2)
    u = ((p[0] - p1[0]) * (p2[0] - p1[0])) + ((p[1] - p1[1]) * (p2[1] - p1[1]))
    if l == 0 or u < 0.0 or u > l * l:
        if perpendicular:
            return INVALID_DISTANCE
        if u < 0:
            return 0
        return l
    return u / l


def polygonOffsetWithMinimumDistanceToPoint(point, polygon, perpendicular=False):
    """Return the offset from the polygon start where the distance to point is minimal"""
    p = point
    s = polygon
    o = 0
    for i in range(0, len(s)-1):
        q = lineOffsetWithMinimumDistanceToPoint(p, s[i], s[i+1], True)
        if q != INVALID_DISTANCE:
            return o + q
        o += distance(s[i], s[i+1])
    if perpendicular:
        return INVALID_DISTANCE 
    else:
        return min([distance(point, p) for p in polygon])


def distancePointToLine(point, line_start, line_end, perpendicular=False):
    """Return the minimum distance between point and the line (line_start, line_end)"""
    p1 = line_start
    p2 = line_end
    offset = lineOffsetWithMinimumDistanceToPoint(point, line_start, line_end, perpendicular)
    if offset == INVALID_DISTANCE: 
        return INVALID_DISTANCE
    if offset == 0:
        return distance(point, p1)
    u = offset / distance(line_start, line_end)
    intersection = (p1[0] + u*(p2[0]-p1[0]), p1[1] + u*(p2[1]-p1[1]))
    return distance(point, intersection)


def distancePointToPolygon(point, polygon, perpendicular=False):
    """Return the minimum distance between point and polygon"""
    p = point
    s = polygon
    minDist = None
    for i in range(0, len(s)-1):
        dist = distancePointToLine(p, s[i], s[i+1], perpendicular)
        if dist == INVALID_DISTANCE and perpendicular and i != 0: 
            # distance to inner corner
            dist = distance(point, s[i])
        if dist != INVALID_DISTANCE:
            if minDist is None or dist < minDist:
                minDist = dist
    if minDist is not None:
        return minDist
    else:
        return INVALID_DISTANCE


def positionAtOffset(p1, p2, offset):
    dist = distance(p1, p2)
    if dist < offset:
        return None
    return (p1[0] + (p2[0] - p1[0]) * (offset / dist), p1[1] + (p2[1] - p1[1]) * (offset / dist))


def positionAtShapeOffset(shape, offset):
    seenLength = 0
    curr = shape[0]
    for next in shape[1:]:
        nextLength = distance(curr, next)
        if seenLength + nextLength > offset:
            return positionAtOffset(curr, next, offset - seenLength)
        seenLength += nextLength
        curr = next
    return shape[-1]


if __name__ == "__main__":
    # run some tests
    point = (81365.994719034992, 9326.8304398041219)
    polygon = [
            (81639.699999999997, 9196.8400000000001),
            (81554.910000000003, 9246.7600000000002),
            (81488.800000000003, 9288.2999999999993),
            (81376.100000000006, 9358.5799999999999),
            (81305.089999999997, 9404.4400000000005),
            (81230.610000000001, 9452.4200000000001),
            (81154.699999999997, 9502.6000000000004),
            (81063.419999999998, 9564.5799999999999),
            (80969.389999999999, 9627.6100000000006),
            (80882.990000000005, 9686.3899999999994),
            (80772.160000000003, 9763.4200000000001),
            (80682.259999999995, 9825.4500000000007),
            (80617.509999999995, 9868.1499999999996),
            (80552.660000000003, 9914.1900000000005)]
    dist = distancePointToPolygon(point, polygon, True)
    assert(abs(dist - 32.288) < 0.01)
    print "Test successful"

    assert(polygonOffsetWithMinimumDistanceToPoint((-1,-1), [(0, 1), (0,0), (1,0)], True) == INVALID_DISTANCE)
    assert(polygonOffsetWithMinimumDistanceToPoint((-1,-1), [(0, 1), (0,0), (1,0)], False) == math.sqrt(2))

