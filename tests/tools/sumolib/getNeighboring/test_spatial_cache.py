#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

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

    @unittest.skipUnless(HAVE_RTREE, 'optional rtree dependency is unavailable')
    def test_move_updates_warm_indices_and_extended_shapes(self):
        net = sumolib.net.readNet(self.fixture)
        expected = {}
        for include_junctions in (False, True):
            for query in ('getNeighboringEdges', 'getNeighboringLanes'):
                expected[query, include_junctions] = {
                    obj.getID(): distance for obj, distance in
                    getattr(net, query)(100, 100, 2, include_junctions)}
        lane = net.getEdges()[0].getLane(0)
        before = list(lane.getShape3D(includeJunctions=True))
        node = net.getEdges()[0].getFromNode()
        before_node = node.getCoord3D()
        net.move(10000, 20000, 7)
        for (query, include_junctions), results in expected.items():
            with self.subTest(query=query, include_junctions=include_junctions):
                after = {obj.getID(): distance for obj, distance in
                         getattr(net, query)(10100, 20100, 2, include_junctions)}
                self.assertEqual(set(after), set(results))
                for obj_id in after:
                    self.assertAlmostEqual(after[obj_id], results[obj_id])
                self.assertEqual(getattr(net, query)(100, 100, 2, include_junctions), [])
        self.assertEqual(lane.getShape3D(includeJunctions=True),
                         [(x + 10000, y + 20000, z + 7) for x, y, z in before])
        self.assertEqual(node.getCoord3D(),
                         (before_node[0] + 10000, before_node[1] + 20000, before_node[2] + 7))

    @unittest.skipUnless(HAVE_RTREE, 'optional rtree dependency is unavailable')
    def test_empty_network_can_grow_after_queries(self):
        net = sumolib.net.Net()
        for include_junctions in (False, True):
            self.assertEqual(net.getNeighboringLanes(50, 0, 1, include_junctions), [])
            self.assertEqual(net.getNeighboringEdges(50, 0, 1, include_junctions), [])
        net.addNode('start', coord=(0., 0., 0.))
        net.addNode('end', coord=(100., 0., 0.))
        edge = net.addEdge('road', 'start', 'end', 1, '', '')
        edge.setRawShape([(0., 0., 0.), (100., 0., 0.)])
        lane = net.addLane(edge, 10., 100., 3.)
        lane.setShape([(0., 0., 0.), (100., 0., 0.)])
        for include_junctions in (False, True):
            self.assertEqual(net.getNeighboringLanes(50, 0, 1, include_junctions), [(lane, 0.)])
            self.assertEqual(net.getNeighboringEdges(50, 0, 1, include_junctions), [(edge, 0.)])

    @unittest.skipUnless(HAVE_RTREE, 'optional rtree dependency is unavailable')
    def test_adding_lane_rebuilds_lane_and_average_edge_shapes(self):
        net = sumolib.net.Net()
        net.addNode('start', coord=(0., 0., 0.))
        net.addNode('end', coord=(100., 0., 0.))
        edge = net.addEdge('road', 'start', 'end', 1, '', '')
        edge.setRawShape([(0., 0., 0.), (100., 0., 0.)])
        original = net.addLane(edge, 10., 100., 3.)
        original.setShape([(0., 0., 0.), (100., 0., 0.)])
        for include_junctions in (False, True):
            net.getNeighboringLanes(50, 0, 1, include_junctions)
            net.getNeighboringEdges(50, 0, 1, include_junctions)
        added = net.addLane(edge, 10., 100., 3.)
        added.setShape([(0., 10., 0.), (100., 10., 0.)])
        for include_junctions in (False, True):
            self.assertEqual(net.getNeighboringLanes(50, 10, 1, include_junctions), [(added, 0.)])
            self.assertEqual(net.getNeighboringEdges(50, 5, 1, include_junctions), [(edge, 0.)])


def run_checks(fixture):
    SpatialCacheTest.fixture = fixture
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(
        unittest.defaultTestLoader.loadTestsFromTestCase(SpatialCacheTest))
    if not result.wasSuccessful():
        raise AssertionError(output.getvalue())


if __name__ == '__main__':
    unittest.main()
