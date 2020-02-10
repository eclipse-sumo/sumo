#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2018-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    unittests.py
# @author  Michael Behrisch
# @date    2019-01-09

from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import unittest

# Do not use SUMO_HOME here to ensure you are always testing the
# functions from the same tree the test is in
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', '..', 'tools', 'turn-defs'))

import collectinghandler  # noqa
import connections  # noqa
import turndefinitions  # noqa


class TurnDefinitionsTestCase(unittest.TestCase):
    # pylint: disable=C,R,W,E,F

    def setUp(self):
        self.turn_definitions = turndefinitions.TurnDefinitions()

    def tearDown(self):
        self.turn_definitions = None

    def test_new_turn_definitions_is_empty(self):
        self.assertEqual([], self.turn_definitions.get_sources())

    def test_get_sources(self):
        self.turn_definitions.add("source", "destination", 100)
        self.assertEqual(["source"], self.turn_definitions.get_sources())

    def test_get_sources_is_sorted(self):
        self.turn_definitions.add("source 1", "destination", 100)
        self.turn_definitions.add("source 2", "destination", 100)
        self.assertEqual(["source 1", "source 2"],
                         self.turn_definitions.get_sources())

    def test_get_destinations(self):
        self.turn_definitions.add("source", "destination", 100)
        self.assertEqual(["destination"],
                         self.turn_definitions.get_destinations("source"))

    def test_get_destinations_is_sorted(self):
        self.turn_definitions.add("source", "destination 1", 100)
        self.turn_definitions.add("source", "destination 2", 100)
        self.assertEqual(["destination 1", "destination 2"],
                         self.turn_definitions.get_destinations("source"))

    def test_get_turning_probability(self):
        self.turn_definitions.add("source", "destination", 100)
        self.assertEqual(100,
                         self.turn_definitions.get_turning_probability("source",
                                                                       "destination"))

    def test_probability_overflow_raises_warning_on_initial_add(self):
        collecting_handler = collectinghandler.CollectingHandler()
        self.turn_definitions.logger.addHandler(collecting_handler)

        self.turn_definitions.add("source", "destination", 101)

        self.assertTrue(
            "Turn probability overflow: 101.000000; lowered to 100"
            in [record.getMessage()
                for record in collecting_handler.log_records])

    def test_probability_overflow_raises_warning_after_addition(self):
        collecting_handler = collectinghandler.CollectingHandler()
        self.turn_definitions.logger.addHandler(collecting_handler)

        self.turn_definitions.add("source", "destination", 90)
        self.turn_definitions.add("source", "destination", 11)

        self.assertTrue(
            "Turn probability overflow: 101.000000; lowered to 100"
            in [record.getMessage()
                for record in collecting_handler.log_records])


class CreateTurnDefinitionsTestCase(unittest.TestCase):
    # pylint: disable=C,R,W,E,F

    def test_creation_with_0_sources(self):
        self.assertEqual(turndefinitions.TurnDefinitions(),
                         turndefinitions.from_connections(connections.Connections()))

    def test_creation_with_one_source(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")

        turn_definitions = turndefinitions.TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
                         turndefinitions.from_connections(input_connections))

    def test_creation_with_two_sources(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 2", "source lane 1", "destination 1")

        turn_definitions = turndefinitions.TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)
        turn_definitions.add("source 2", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
                         turndefinitions.from_connections(input_connections))

    def test_creation_two_destinations_from_two_lanes_overlapping(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 2", "destination 2")
        input_connections.add("source 1", "source lane 2", "destination 1")

        turn_definitions = turndefinitions.TurnDefinitions()
        turn_definitions.add(
            "source 1", "destination 1", 100.0 / 2 / 2 + 100.0 / 2)
        turn_definitions.add("source 1", "destination 2", 100.0 / 2 / 2)

        self.assertEqual(turn_definitions,
                         turndefinitions.from_connections(input_connections))

    def test_creation_one_destination_from_two_lanes(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 2", "destination 1")

        turn_definitions = turndefinitions.TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
                         turndefinitions.from_connections(input_connections))

    def test_creation_with_one_destination_from_one_lane(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")

        turn_definitions = turndefinitions.TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
                         turndefinitions.from_connections(input_connections))

    def test_creation_with_two_destinations_from_one_lane(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 1", "destination 2")

        turn_definitions = turndefinitions.TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0 / 2)
        turn_definitions.add("source 1", "destination 2", 100.0 / 2)

        self.assertEqual(turn_definitions,
                         turndefinitions.from_connections(input_connections))

    def test_creation_with_three_destinations_from_one_lane(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 1", "destination 2")
        input_connections.add("source 1", "source lane 1", "destination 3")

        turn_definitions = turndefinitions.TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0 / 3)
        turn_definitions.add("source 1", "destination 2", 100.0 / 3)
        turn_definitions.add("source 1", "destination 3", 100.0 / 3)

        self.assertEqual(turn_definitions,
                         turndefinitions.from_connections(input_connections))


class UniformDestinationWeightCalculatorTestCase(unittest.TestCase):
    # pylint: disable=C,W,R,E,F

    def setUp(self):
        self.calculator = connections.UniformDestinationWeightCalculator()

    def tearDown(self):
        self.calculator = None

    def test_uniform_weight_spread_across_lanes(self):
        # 1 lane case
        self.assertAlmostEqual(
            100.0, self.calculator.calculate_weight(0, 1, 1))

        # 2 lane case
        self.assertAlmostEqual(50.0, self.calculator.calculate_weight(0, 2, 1))
        self.assertAlmostEqual(50.0, self.calculator.calculate_weight(1, 2, 1))

        # 2 lane case
        self.assertAlmostEqual(
            100.0 / 3, self.calculator.calculate_weight(0, 3, 1))
        self.assertAlmostEqual(
            100.0 / 3, self.calculator.calculate_weight(1, 3, 1))
        self.assertAlmostEqual(
            100.0 / 3, self.calculator.calculate_weight(2, 3, 1))

    def test_uniform_weight_spread_across_destinations(self):
        # 1 destination
        self.assertAlmostEqual(
            100.0, self.calculator.calculate_weight(0, 1, 1))

        # 2 destinations
        self.assertAlmostEqual(
            100.0 / 2, self.calculator.calculate_weight(0, 1, 2))

        # 3 destinations
        self.assertAlmostEqual(
            100.0 / 3, self.calculator.calculate_weight(0, 1, 3))


class ConnectionsTestCase(unittest.TestCase):
    # pylint: disable=C,W,R,E,F

    def setUp(self):
        self.connections = connections.Connections()

    def tearDown(self):
        self.connections = None

    def assert_contains_source(self, source):
        self.assertTrue(source in
                        self.connections.get_sources())

    def assert_contains_lane(self, source, lane):
        self.assertTrue(lane in
                        self.connections.get_lanes(source))

    def assert_contains_destination(self, source, lane, destination):
        self.assertTrue(destination in
                        self.connections.get_destinations(source, lane))

    def assert_sources_no(self, sources_no):
        self.assertEqual(sources_no,
                         len(self.connections.get_sources()))

    def assert_lanes_no(self, source, lanes_no):
        self.assertEqual(lanes_no,
                         len(self.connections.get_lanes(source)))

    def assert_destinations_no(self, source, lane, destinations_no):
        self.assertEqual(destinations_no,
                         len(self.connections.get_destinations(source, lane)))

    def test_map_empty_on_init(self):
        self.assert_sources_no(0)

    def test_add(self):
        self.connections.add("source", "source lane", "destination")

        self.assert_sources_no(1)
        self.assert_contains_source("source")

        self.assert_lanes_no("source", 1)
        self.assert_contains_lane("source", "source lane")

        self.assert_destinations_no("source", "source lane", 1)
        self.assert_contains_destination(
            "source", "source lane", "destination")

    def test_add_different_lanes(self):
        self.connections.add("source", "source lane 1", "destination")
        self.connections.add("source", "source lane 2", "destination")

        self.assert_sources_no(1)
        self.assert_contains_source("source")

        self.assert_lanes_no("source", 2)
        self.assert_contains_lane("source", "source lane 1")
        self.assert_contains_lane("source", "source lane 2")

        self.assert_destinations_no("source", "source lane 1", 1)
        self.assert_contains_destination(
            "source", "source lane 1", "destination")

        self.assert_destinations_no("source", "source lane 2", 1)
        self.assert_contains_destination(
            "source", "source lane 2", "destination")

    def test_add_different_destinations(self):
        self.connections.add("source", "source lane", "destination 1")
        self.connections.add("source", "source lane", "destination 2")

        self.assert_sources_no(1)
        self.assert_contains_source("source")

        self.assert_lanes_no("source", 1)
        self.assert_contains_lane("source", "source lane")

        self.assert_destinations_no("source", "source lane", 2)
        self.assert_contains_destination(
            "source", "source lane", "destination 1")
        self.assert_contains_destination(
            "source", "source lane", "destination 2")

    def test_readd_raises_warning(self):
        collecting_handler = collectinghandler.CollectingHandler()
        self.connections.logger.addHandler(collecting_handler)

        self.connections.add("source", "source lane", "destination")
        self.connections.add("source", "source lane", "destination")

        self.assert_sources_no(1)
        self.assert_contains_source("source")

        self.assert_lanes_no("source", 1)
        self.assert_contains_lane("source", "source lane")

        self.assert_destinations_no("source", "source lane", 1)
        self.assert_contains_destination("source", "source lane", "destination")

        self.assertTrue(
            "Destination for source (lane source lane) readded: destination"
            in [record.getMessage() for record in collecting_handler.log_records])


if __name__ == "__main__":
    unittest.main()
