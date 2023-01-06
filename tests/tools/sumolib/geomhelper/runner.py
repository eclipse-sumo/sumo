# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Matthias Schwamborn
# @date    2013-02-25

import os
import sys
import unittest
# Do not use SUMO_HOME here to ensure you are always testing the
# functions from the same tree the test is in
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools'))
import sumolib  # noqa


class TestGeomhelper(unittest.TestCase):

    def testPolygonOffsetWithMinimumDistanceToPoint(self):
        """Return the offset from the polygon start where the distance to point is minimal"""
        offsetMinDist = sumolib.geomhelper.polygonOffsetWithMinimumDistanceToPoint
        shape = ((0, 1), (0, 0), (1, 0))
        self.assertEqual(1, offsetMinDist((-1, -1), shape, False))
        self.assertEqual(1, offsetMinDist((-1, -1), shape, True))
        self.assertEqual(2, offsetMinDist((2, 1), shape, False))
        self.assertEqual(0, offsetMinDist((2, 1), shape, True))
        self.assertEqual(2, offsetMinDist((3, 2), shape, False))
        self.assertEqual(
            sumolib.geomhelper.INVALID_DISTANCE, offsetMinDist((3, 2), shape, True))

    def testDistancePointToPolygon(self):
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
        dist = sumolib.geomhelper.distancePointToPolygon(point, polygon, True)
        self.assertTrue(abs(dist - 32.288) < 0.01)

    def testPointInPoly(self):
        convex = [(0, 0), (2, 0), (2, 2), (0, 2)]
        self.assertEqual(sumolib.geomhelper.isWithin((1, 1), convex), True)
        self.assertEqual(sumolib.geomhelper.isWithin((-1, -1), convex), False)
        self.assertEqual(sumolib.geomhelper.isWithin((3, 3), convex), False)
        self.assertEqual(sumolib.geomhelper.isWithin((1.5, 1.5), convex), True)
        concave = [(0, 0), (2, 0), (2, 1), (1, 1), (1, 2), (0, 2)]
        self.assertEqual(
            sumolib.geomhelper.isWithin((0.5, 0.5), concave), True)
        self.assertEqual(
            sumolib.geomhelper.isWithin((1.5, 1.5), concave), False)

    def testSplitPolygonAtLengths2D(self):
        singlePointPolygon = [(0, 0)]
        polygon1 = [(0, 0), (3, 0)]
        lengths1 = [1]
        polygon1Slices = [[(0, 0), (1, 0)], [(1, 0), (3, 0)]]
        self.assertEqual(sumolib.geomhelper.splitPolygonAtLengths2D(polygon1, []), [polygon1])
        self.assertEqual(sumolib.geomhelper.splitPolygonAtLengths2D(singlePointPolygon, lengths1), [singlePointPolygon])
        self.assertEqual(sumolib.geomhelper.splitPolygonAtLengths2D(polygon1, lengths1), polygon1Slices)

    def testIntersectsAtLengths2D(self):
        polygon1 = [(0, 0), (3, 0)]
        polygon2 = [(1, 0), (2, 0)]
        self.assertEqual(sumolib.geomhelper.intersectsPolygon(polygon1, polygon2), True)
        self.assertEqual(sumolib.geomhelper.intersectsAtLengths2D(polygon1, polygon2), [1.0, 2.0])
        polygon3 = [(0, 0), (2, 0)]
        self.assertEqual(sumolib.geomhelper.intersectsAtLengths2D(polygon1, polygon3), [0.0, 2.0])
        polygon4 = [(1, 0), (3, 0)]
        self.assertEqual(sumolib.geomhelper.intersectsAtLengths2D(polygon1, polygon4), [1.0, 3.0])
        polygon5 = [(-1, 0), (2, 0)]
        self.assertEqual(sumolib.geomhelper.intersectsAtLengths2D(polygon1, polygon5), [0.0, 2.0])
        polygon6 = [(1, 0), (4, 0)]
        self.assertEqual(sumolib.geomhelper.intersectsAtLengths2D(polygon1, polygon6), [1.0, 3.0])
        polygon7 = [(-1, 0), (4, 0)]
        self.assertEqual(sumolib.geomhelper.intersectsAtLengths2D(polygon1, polygon7), [0.0, 3.0])

    def testIsClosedPolygon(self):
        polygon1 = [(0, 0), (1, 0), (1, 1), (0, 1), (0, 0)]
        polygon2 = [(0, 0), (1, 0), (1, 1), (0, 1)]
        self.assertEqual(sumolib.geomhelper.isClosedPolygon(polygon1), True)
        self.assertEqual(sumolib.geomhelper.isClosedPolygon(polygon2), False)


if __name__ == '__main__':
    unittest.main()
