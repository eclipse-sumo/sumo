# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    geomhelper.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Matthias Schwamborn
# @date    2013-02-25

from __future__ import absolute_import
import math
import sys

INVALID_DISTANCE = -1

# back-ported from python 3 for backward compatibility
# https://www.python.org/dev/peps/pep-0485/#proposed-implementation


def isclose(a, b, rel_tol=1e-09, abs_tol=0.0):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)


def distance(p1, p2):
    dx = p1[0] - p2[0]
    dy = p1[1] - p2[1]
    return math.sqrt(dx * dx + dy * dy)


def polyLength(polygon):
    return sum([distance(a, b) for a, b in zip(polygon[:-1], polygon[1:])])


def addToBoundingBox(coordList, bbox=None):
    if bbox is None:
        minX = 1e400
        minY = 1e400
        maxX = -1e400
        maxY = -1e400
    else:
        minX, minY, maxX, maxY = bbox
    for x, y in coordList:
        minX = min(x, minX)
        minY = min(y, minY)
        maxX = max(x, maxX)
        maxY = max(y, maxY)
    return minX, minY, maxX, maxY


def isLeft(point, line_start, line_end):
    return ((line_end[0] - line_start[0]) * (point[1] - line_start[1])
            < (point[0] - line_start[0]) * (line_end[1] - line_start[1]))


def lineOffsetWithMinimumDistanceToPoint(point, line_start, line_end, perpendicular=False):
    """Return the offset from line (line_start, line_end) where the distance to
    point is minimal"""
    d = distance(line_start, line_end)
    u = ((point[0] - line_start[0]) * (line_end[0] - line_start[0])
         + (point[1] - line_start[1]) * (line_end[1] - line_start[1]))
    if d == 0. or u < 0. or u > d * d:
        if perpendicular:
            return INVALID_DISTANCE
        if u < 0.:
            return 0.
        return d
    return u / d


def polygonOffsetAndDistanceToPoint(point, polygon, perpendicular=False):
    """Return the offset and the distance from the polygon start where the distance to the point is minimal"""
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
    """Return the offset from the polygon start where the distance to the point is minimal"""
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
    if isclose(offset, 0.):  # for pathological cases with dist == 0 and offset == 0
        return p1

    dist = distance(p1, p2)

    if isclose(dist, offset):
        return p2

    if offset > dist:
        return None

    return (p1[0] + (p2[0] - p1[0]) * (offset / dist), p1[1] + (p2[1] - p1[1]) * (offset / dist))


def indexAtShapeOffset(shape, offset):
    """Returns the index of the shape segment which contains the offset and
       the cumulated length of the shape up to the start point of the segment.
       If the offset is less or equal to 0, it returns (0, 0.) If the offset is
       larger than the shape length it returns (None, length of the shape)"""
    seenLength = 0.
    curr = shape[0]
    for idx, p in enumerate(shape[1:]):
        nextLength = distance(curr, p)
        if seenLength + nextLength > offset:
            return idx, seenLength
        seenLength += nextLength
        curr = p
    return None, seenLength


def positionAtShapeOffset(shape, offset):
    idx, seen = indexAtShapeOffset(shape, offset)
    if idx is None:
        return shape[-1]
    return positionAtOffset(shape[idx], shape[idx + 1], offset - seen)


def rotationAtShapeOffset(shape, offset):
    idx, _ = indexAtShapeOffset(shape, offset)
    if idx is None:
        return None
    return math.atan2(shape[idx + 1][1] - shape[idx][1], shape[idx + 1][0] - shape[idx][0])


def angle2D(p1, p2):
    theta1 = math.atan2(p1[1], p1[0])
    theta2 = math.atan2(p2[1], p2[0])
    dtheta = theta2 - theta1
    while dtheta > math.pi:
        dtheta -= 2.0 * math.pi
    while dtheta < -math.pi:
        dtheta += 2.0 * math.pi
    return dtheta


def angleTo2D(p1, p2):
    return math.atan2(p2[1] - p1[1], p2[0] - p1[0])


def naviDegree(rad):
    return normalizeAngle(math.degrees(math.pi / 2. - rad), 0, 360, 360)


def fromNaviDegree(degrees):
    return math.pi / 2. - math.radians(degrees)


def normalizeAngle(a, lower, upper, circle):
    while a < lower:
        a = a + circle
    while a > upper:
        a = a - circle
    return a


def minAngleDegreeDiff(d1, d2):
    return min(normalizeAngle(d1 - d2, 0, 360, 360),
               normalizeAngle(d2 - d1, 0, 360, 360))


def isWithin(pos, shape):
    """Returns whether the given pos coordinate is inside the polygon shape defined in anticlockwise order."""
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


def sideOffset(fromPos, toPos, amount):
    scale = amount / distance(fromPos, toPos)
    return (scale * (fromPos[1] - toPos[1]),
            scale * (toPos[0] - fromPos[0]))


def sub(a, b):
    return (a[0] - b[0], a[1] - b[1])


def add(a, b):
    return (a[0] + b[0], a[1] + b[1])


def mul(a, x):
    return (a[0] * x, a[1] * x)


def dotProduct(a, b):
    return a[0] * b[0] + a[1] * b[1]


def orthoIntersection(a, b):
    c = add(a, b)
    quot = dotProduct(c, a)
    if quot != 0:
        return mul(mul(c, dotProduct(a, a)), 1 / quot)
    else:
        return None


def rotateAround2D(p, rad, origin):
    s = math.sin(rad)
    c = math.cos(rad)
    tmp = sub(p, origin)
    tmp2 = [tmp[0] * c - tmp[1] * s,
            tmp[0] * s + tmp[1] * c]
    return add(tmp2, origin)


def length(a):
    return math.sqrt(dotProduct(a, a))


def norm(a):
    return mul(a, 1 / length(a))


def narrow(fromPos, pos, toPos, amount):
    """detect narrow turns which cannot be shifted regularly"""
    a = sub(toPos, pos)
    b = sub(pos, fromPos)
    c = add(a, b)
    dPac = dotProduct(a, c)
    if dPac == 0:
        return True
    x = dotProduct(a, a) * length(c) / dPac
    return x < amount


def line2boundary(shape, width):
    """expand center line by width/2 on both sides to obtain a (closed) boundary shape
    (i.e. for an edge or lane)
    """
    left = move2side(shape, width / 2)
    right = move2side(shape, -width / 2)
    return left + list(reversed(right)) + [left[0]]


def move2side(shape, amount):
    shape = [s for i, s in enumerate(shape) if i == 0 or shape[i-1] != s]
    if len(shape) < 2:
        return shape
    if polyLength(shape) == 0:
        return shape
    result = []
    for i, pos in enumerate(shape):
        if i == 0:
            fromPos = pos
            toPos = shape[i + 1]
            if fromPos != toPos:
                result.append(sub(fromPos, sideOffset(fromPos, toPos, amount)))
        elif i == len(shape) - 1:
            fromPos = shape[i - 1]
            toPos = pos
            if fromPos != toPos:
                result.append(sub(toPos, sideOffset(fromPos, toPos, amount)))
        else:
            fromPos = shape[i - 1]
            toPos = shape[i + 1]
            # check for narrow turns
            if narrow(fromPos, pos, toPos, amount):
                # print("narrow at i=%s pos=%s" % (i, pos))
                pass
            else:
                a = sideOffset(fromPos, pos, -amount)
                b = sideOffset(pos, toPos, -amount)
                c = orthoIntersection(a, b)
                if orthoIntersection is not None:
                    pos2 = add(pos, c)
                else:
                    extend = norm(sub(pos, fromPos))
                    pos2 = add(pos, mul(extend, amount))
                result.append(pos2)
    # print("move2side", amount)
    # print(shape)
    # print(result)
    # print()
    return result


def isClosedPolygon(polygon):
    return (len(polygon) >= 2) and (polygon[0] == polygon[-1])


def splitPolygonAtLengths2D(polygon, lengths):
    """
    Returns the polygon segments split at the given 2D-lengths.
    """
    if (len(polygon) <= 1 or len(lengths) == 0):
        return [polygon]
    offsets = [offset for offset in sorted(lengths) if offset > 0.0 and offset < polyLength(polygon)]
    ret = []
    seenLength = 0
    curr = polygon[0]
    polygonIndex = 0
    for offset in offsets:
        currSlice = [curr]
        while polygonIndex < len(polygon) - 1:
            p = polygon[polygonIndex + 1]
            if offset < seenLength + distance(curr, p):
                splitPos = positionAtOffset(curr, p, offset - seenLength)
                if not isclose(distance(currSlice[-1], splitPos), 0):
                    currSlice.append(splitPos)
                seenLength += distance(curr, splitPos)
                curr = splitPos
                break
            else:
                if not isclose(distance(currSlice[-1], p), 0):
                    currSlice.append(p)
                seenLength += distance(curr, p)
                curr = p
                polygonIndex += 1
        ret.append(currSlice)
    if polygonIndex < len(polygon) - 1:
        finalSlice = [curr] + polygon[polygonIndex + 1:]
        ret.append(finalSlice)
    return ret


def intersectsAtLengths2D(polygon1, polygon2):
    """
    Returns the 2D-length from polygon1's start to all intersections between polygon1 and polygon2.
    """
    ret = []
    if (len(polygon1) == 0 or len(polygon2) == 0):
        return ret
    polygon1Length = polyLength(polygon1)
    for j in range(len(polygon2) - 1):
        p21 = polygon2[j]
        p22 = polygon2[j + 1]
        pos = 0.0
        for i in range(len(polygon1) - 1):
            p11 = polygon1[i]
            p12 = polygon1[i + 1]
            pIntersection = [0.0, 0.0]
            if intersectsLineSegment(p11, p12, p21, p22, 0.0, pIntersection, True):
                for k in range(0, len(pIntersection), 2):
                    length = pos + distance(p11, (pIntersection[k], pIntersection[k + 1]))
                    # special case for closed polygons
                    if isClosedPolygon(polygon1) and isclose(length, polygon1Length):
                        length = 0.0
                    # check for duplicate
                    isDuplicate = False
                    for result in ret:
                        if isclose(length, result):
                            isDuplicate = True
                            break
                    if not isDuplicate:
                        ret.append(length)
            pos += distance(p11, p12)
    return ret


def intersectsPolygon(polygon1, polygon2):
    """
    Returns whether the polygons intersect on at least one of their segments.
    """
    if (len(polygon1) < 2 or len(polygon2) < 2):
        return False
    for i in range(len(polygon1) - 1):
        p11 = polygon1[i]
        p12 = polygon1[i + 1]
        for j in range(len(polygon2) - 1):
            p21 = polygon2[j]
            p22 = polygon2[j + 1]
            if intersectsLineSegment(p11, p12, p21, p22):
                return True
    return False


# @see src/utils/geom/PositionVector::intersects(p11, p12, p21, p22 ...)
def intersectsLineSegment(p11, p12, p21, p22, withinDist=0.0, pIntersection=None, storeEndPointsIfCoincident=False):
    """
    Returns whether the line segments defined by Line p11,p12 and Line p21,p22 intersect.
    If not set to 'None', 'pIntersection' serves as a storage for the intersection point(s).
    Parameter 'storeEndPointsIfCoincident' is an option for storing the endpoints of the
    line segment defined by the intersecting set of line1 and line2 if applicable.
    """
    eps = sys.float_info.epsilon
    # dy2 * dx1 - dx2 * dy1
    denominator = (p22[1] - p21[1]) * (p12[0] - p11[0]) - (p22[0] - p21[0]) * (p12[1] - p11[1])
    # dx2 * (p11.y - p21.y) - dy2 * (p11.x - p21.x)
    numera = (p22[0] - p21[0]) * (p11[1] - p21[1]) - (p22[1] - p21[1]) * (p11[0] - p21[0])
    # dx1 * (p11.y - p21.y) - dy1 * (p11.x - p21.x)
    numerb = (p12[0] - p11[0]) * (p11[1] - p21[1]) - (p12[1] - p11[1]) * (p11[0] - p21[0])
    # Are the lines coincident?
    if (math.fabs(numera) < eps and math.fabs(numerb) < eps and math.fabs(denominator) < eps):
        a1 = 0.0
        a2 = 0.0
        a3 = 0.0
        a4 = 0.0
        a = -1e12
        isVertical = (p11[0] == p12[0])
        if not isVertical:
            # line1 and line2 are not vertical
            a1 = p11[0] if p11[0] < p12[0] else p12[0]
            a2 = p12[0] if p11[0] < p12[0] else p11[0]
            a3 = p21[0] if p21[0] < p22[0] else p22[0]
            a4 = p22[0] if p21[0] < p22[0] else p21[0]
        else:
            # line1 and line2 are vertical
            a1 = p11[1] if p11[1] < p12[1] else p12[1]
            a2 = p12[1] if p11[1] < p12[1] else p11[1]
            a3 = p21[1] if p21[1] < p22[1] else p22[1]
            a4 = p22[1] if p21[1] < p22[1] else p21[1]
        if a1 <= a3 and a3 <= a2:
            # one endpoint of line2 lies on line1
            if a4 <= a2:
                # line2 is a subset of line1
                a = (a3 + a4) / 2.0
                if storeEndPointsIfCoincident and pIntersection is not None:
                    pIntersection[0] = p21[0]
                    pIntersection[1] = p21[1]
                    pIntersection.append(p22[0])
                    pIntersection.append(p22[1])
                    return True
            else:
                # the other endpoint of line2 lies beyond line1
                a = (a3 + a2) / 2.0
                if storeEndPointsIfCoincident and pIntersection is not None:
                    if not isVertical:
                        pIntersection[0] = a3
                        pIntersection[1] = p21[1] if p21[0] < p22[0] else p22[1]
                        pIntersection.append(a2)
                        pIntersection.append(p12[1] if p11[0] < p12[0] else p11[1])
                    else:
                        pIntersection[0] = p21[0] if p21[1] < p22[1] else p22[0]
                        pIntersection[1] = a3
                        pIntersection.append(p12[0] if p11[1] < p12[1] else p11[0])
                        pIntersection.append(a2)
                    return True
        if a3 <= a1 and a1 <= a4:
            # one endpoint of line1 lies on line2
            if a2 <= a4:
                # line1 is a subset of line2
                a = (a1 + a2) / 2.0
                if storeEndPointsIfCoincident and pIntersection is not None:
                    pIntersection[0] = p11[0]
                    pIntersection[1] = p11[1]
                    pIntersection.append(p12[0])
                    pIntersection.append(p12[1])
                    return True
            else:
                # the other endpoint of line1 lies beyond line2
                a = (a1 + a4) / 2.0
                if storeEndPointsIfCoincident and pIntersection is not None:
                    if not isVertical:
                        pIntersection[0] = a1
                        pIntersection[1] = p11[1] if p11[0] < p12[0] else p12[1]
                        pIntersection.append(a4)
                        pIntersection.append(p22[1] if p21[0] < p22[0] else p21[1])
                    else:
                        pIntersection[0] = p11[0] if p11[1] < p12[1] else p12[0]
                        pIntersection[1] = a1
                        pIntersection.append(p22[0] if p21[1] < p22[1] else p21[0])
                        pIntersection.append(a4)
                    return True
        if a != -1e12:
            if pIntersection is not None:
                if not isVertical:
                    # line1 and line2 are not vertical
                    mu = (a - p11[0]) / (p12[0] - p11[0])
                    x = a
                    y = p11[1] + mu * (p12[1] - p11[1])
                else:
                    # line1 and line2 are vertical
                    x = p11[0]
                    y = a
                    if p12[1] == p11[1]:
                        mu = 0
                    else:
                        mu = (a - p11[1]) / (p12[1] - p11[1])
                pIntersection[0] = x
                pIntersection[1] = y
            return True
        return False
    # Are the lines parallel?
    if math.fabs(denominator) < eps:
        return False
    # Is the intersection along the segments?
    mua = numera / denominator
    # Reduce rounding errors for lines ending in the same point
    if math.fabs(p12[0] - p22[0]) < eps and math.fabs(p12[1] - p22[1]) < eps:
        mua = 1.0
    else:
        offseta = withinDist / distance(p11, p12)
        offsetb = withinDist / distance(p21, p22)
        mub = numerb / denominator
        if (mua < -offseta or mua > 1 + offseta or mub < -offsetb or mub > 1 + offsetb):
            return False
    if pIntersection is not None:
        x = p11[0] + mua * (p12[0] - p11[0])
        y = p11[1] + mua * (p12[1] - p11[1])
        mu = mua
        pIntersection[0] = x
        pIntersection[1] = y
    return True
