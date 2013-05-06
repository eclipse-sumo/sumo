"""
@file    geomhelper.py
@author  Daniel Krajzewicz
@date    2013-02-25
@version $Id$

Some helper functions for geometrical computations.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
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
    u = (((p[0] - p1[0]) * (p2[0] - p1[0])) + ((p[1] - p1[1]) * (p2[1] - p1[1]))) / (l * l)
    if u < 0.0 or u > 1:
        if perpendicular:
            return INVALID_DISTANCE
        if u < 0:
            return 0
        return l
    return u * l


def polygonOffsetWithMinimumDistanceToPoint(point, polygon):
    """Return the offset from the polygon start where the distance to point is minimal"""
    p = point
    s = polygon
    o = 0
    for i in range(0, len(s)-1):
        q = lineOffsetWithMinimumDistanceToPoint(p, s[i], s[i+1], True)
        if q!=-1:
            return o+q
        o = o + distance(s[i], s[i+1])
    return -1


def distancePointToLine(point, line_start, line_end, perpendicular=False):
    """Return the minimum distance between point and the line (line_start, line_end)"""
    p1 = line_start
    p2 = line_end
    u = lineOffsetWithMinimumDistanceToPoint(point, line_start, line_end, perpendicular)
    if u == INVALID_DISTANCE: 
        return INVALID_DISTANCE
    intersection = (p1[0] + u*(p2[0]-p1[0]), p1[1] + u*(p2[1]-p1[1]))
    return distance(point, intersection)


def distancePointToPolygon(point, polygon, perpendicular=True):
    """Return the minimum distance between point and polygon"""
    p = point
    s = polygon
    minDist = None
    for i in range(0, len(s)-1):
        dist = distancePointToLine(p, s[i], s[i+1], perpendicular)
        if dist != INVALID_DISTANCE:
            if minDist is None or dist < minDist:
                minDist = dist
    if minDist is not None:
        return minDist
    else:
        return INVALID_DISTANCE


