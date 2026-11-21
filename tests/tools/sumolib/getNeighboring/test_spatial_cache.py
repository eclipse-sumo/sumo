#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test_spatial_cache.py
# @author  Mark Colley
# @date    2026-11-05

import io
import os
import sys
import unittest
import warnings
from unittest import mock

sys.path.insert(0, os.path.join(os.environ.get(
    'SUMO_HOME', os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))), 'tools'))
import sumolib.net  # noqa

try:
    import rtree  # noqa
    HAVE_RTREE = True
except ImportError:
    HAVE_RTREE = False


class SpatialCacheTest(unittest.TestCase):
    fixture = os.path.join(os.path.dirname(__file__), 'input_net.net.xml')

    def test_missing_rtree_does_not_accumulate_lane_references(self):
        net = sumolib.net.readNet(self.fixture)
        lane_count = sum(len(edge.getLanes()) for edge in net.getEdges())
        with mock.patch.dict(sys.modules, {'rtree': None}), warnings.catch_warnings():
            warnings.filterwarnings('ignore', message="Module 'rtree' not available.*")
            for index in range(100):
                result = net.getNeighboringLanes(100, 100, 2, bool(index % 2))
                self.assertEqual(len(result), len({lane.getID() for lane, _ in result}))
        self.assertLessEqual(len(net._allLanes), lane_count)
        self.assertTrue(all(index is None for index in net._rtreeLanes.values()))

    @unittest.skipUnless(HAVE_RTREE, 'optional rtree dependency is unavailable')
    def test_both_index_orders_and_repeated_queries_return_unique_lanes(self):
        for order in ((False, True), (True, False)):
            net = sumolib.net.readNet(self.fixture)
            lane_count = sum(len(edge.getLanes()) for edge in net.getEdges())
            for include_junctions in order * 3:
                with self.subTest(order=order, include_junctions=include_junctions):
                    result = net.getNeighboringLanes(100, 100, 2, include_junctions)
                    self.assertEqual(len(result), len({lane.getID() for lane, _ in result}))
                    self.assertEqual(len(net._allLanes), lane_count)


def run_checks(fixture):
    SpatialCacheTest.fixture = fixture
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(
        unittest.defaultTestLoader.loadTestsFromTestCase(SpatialCacheTest))
    if not result.wasSuccessful():
        raise AssertionError(output.getvalue())


if __name__ == '__main__':
    unittest.main()
