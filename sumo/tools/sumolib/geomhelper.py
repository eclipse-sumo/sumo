"""
@file    geomhelper.py
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2013-02-25
@version $Id$

Some helper functions for geometrical computations.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
import math

INVALID_DISTANCE = -1


def distance(p1, p2):
    dx = p1[0] - p2[0]
    dy = p1[1] - p2[1]
    return math.sqrt(dx * dx + dy * dy)


def polyLength(polygon):
    return sum([distance(a, b) for a, b in zip(polygon[:-1], polygon[1:])])


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


def polygonOffsetAndDistanceToPoint(point, polygon, perpendicular=False):
    """Return the offset and the distancefrom the polygon start where the distance to point is minimal"""
    p = point
    s = polygon
    seen = 0
    minDist = 1e400
    minOffset = INVALID_DISTANCE
    for i in range(len(s) - 1):
        pos = lineOffsetWithMinimumDistanceToPoint(
            p, s[i], s[i + 1], perpendicular)
        dist = minDist if pos == INVALID_DISTANCE else distance(
            p, positionAtOffset(s[i], s[i + 1], pos))
        if dist < minDist:
            minDist = dist
            minOffset = pos + seen
        if perpendicular and i != 0 and pos == INVALID_DISTANCE:
            # even if perpendicular is set we still need to check the distance
            # to the inner points
            cornerDist = distance(p, s[i])
            if cornerDist < minDist:
                pos1 = lineOffsetWithMinimumDistanceToPoint(
                    p, s[i - 1], s[i], False)
                pos2 = lineOffsetWithMinimumDistanceToPoint(
                    p, s[i], s[i + 1], False)
                if pos1 == distance(s[i - 1], s[i]) and pos2 == 0.:
                    minOffset = seen
                    minDist = cornerDist
        seen += distance(s[i], s[i + 1])
    return minOffset, minDist


def polygonOffsetWithMinimumDistanceToPoint(point, polygon, perpendicular=False):
    """Return the offset from the polygon start where the distance to point is minimal"""
    return polygonOffsetAndDistanceToPoint(point, polygon, perpendicular)[0]


def distancePointToLine(point, line_start, line_end, perpendicular=False):
    """Return the minimum distance between point and the line (line_start, line_end)"""
    p1 = line_start
    p2 = line_end
    offset = lineOffsetWithMinimumDistanceToPoint(
        point, line_start, line_end, perpendicular)
    if offset == INVALID_DISTANCE:
        return INVALID_DISTANCE
    if offset == 0:
        return distance(point, p1)
    u = offset / distance(line_start, line_end)
    intersection = (p1[0] + u * (p2[0] - p1[0]), p1[1] + u * (p2[1] - p1[1]))
    return distance(point, intersection)


def distancePointToPolygon(point, polygon, perpendicular=False):
    """Return the minimum distance between point and polygon"""
    p = point
    s = polygon
    minDist = None
    for i in range(0, len(s) - 1):
        dist = distancePointToLine(p, s[i], s[i + 1], perpendicular)
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


def angle2D(p1, p2):
    theta1 = math.atan2(p1[1], p1[0])
    theta2 = math.atan2(p2[1], p2[0])
    dtheta = theta2 - theta1
    while dtheta > math.pi:
        dtheta -= 2.0 * math.pi
    while dtheta < -math.pi:
        dtheta += 2.0 * math.pi
    return dtheta


def isWithin(pos, shape):
    angle = 0.
    for i in range(0, len(shape) - 1):
        p1 = ((shape[i][0] - pos[0]), (shape[i][1] - pos[1]))
        p2 = ((shape[i + 1][0] - pos[0]), (shape[i + 1][1] - pos[1]))
        angle = angle + angle2D(p1, p2)
    i = len(shape) - 1
    p1 = ((shape[i][0] - pos[0]), (shape[i][1] - pos[1]))
    p2 = ((shape[0][0] - pos[0]), (shape[0][1] - pos[1]))
    angle = angle + angle2D(p1, p2)
    return math.fabs(angle) >= math.pi
