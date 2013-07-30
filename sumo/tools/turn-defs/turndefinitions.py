#!/usr/bin/python
#-*- encoding: utf8 -*-

""" Operations and classes necessary to work on SUMO turn definitions. """

import connections
import logging
import xml.dom.minidom

LOGGER = logging.getLogger(__name__)


class TurnDefinitions():
    """ Represents a connection of turn definitions. """

    logger = logging.getLogger(__name__)

    def __init__(self):
        """ Constructor. """
        self.turn_definitions = {}

    def add(self, source, destination, probability):
        """ Adds a turn definition. If the given turn definition is already
            defined (in regard to source and destination), issues
            a warning. """

        self.logger.debug("Adding turn definition for %s -> %s "
            "with probability %f" % (source, destination, probability))

        if source not in self.turn_definitions:
            self.turn_definitions[source] = {}

        if destination not in self.turn_definitions[source]:
            self.turn_definitions[source][destination] = 0

        self.turn_definitions[source][destination] += probability

        if self.turn_definitions[source][destination] > 100:
            self.logger.warn("Turn probability overflow: %f; lowered to 100" %
                (self.turn_definitions[source][destination]))
            self.turn_definitions[source][destination] = 100

    def get_sources(self):
        """ Returns all of the turn definitions incoming edges. The result
            will be sorted in alphabetical. """

        sources = self.turn_definitions.keys()
        sources.sort()
        return sources

    def get_destinations(self, source):
        """ Returns all of the turn definition's outgoing edges achievable
            from given source. The turn_definition_source must
            have been added before. The result will be sorted in alphabetical
            order."""

        destinations = self.turn_definitions[source].keys()
        destinations.sort()
        return destinations

    def get_turning_probability(self,
                                source,
                                destination):
        """ Returns the turning probability related to the given
            turn definition. The source and destination must have
            been added before. """

        return self.turn_definitions[source][destination]

    def __eq__(self, other):
        """ Compares this and given object for equality.  """

        if other is None or other.__class__ is not TurnDefinitions:
            self.logger.debug("Checking for equality with "
                              "non-TurnDefinitions object")
            return False

        return self.turn_definitions == other.turn_definitions

def from_connections(input_connections):
    """ Creates a TurnDefinitions object from given Connections' object. """

    LOGGER.info("Creating turn definitions")

    turn_definitions = TurnDefinitions()
    for source in input_connections.get_sources():
        for source_lane in input_connections.get_lanes(source):
            for destination in input_connections.get_destinations(source,
                                                                  source_lane):
                weight = input_connections.calculate_destination_weight(source,
                    source_lane, destination)

                LOGGER.debug("Adding connection %s -> %s (%f)" %
                    (source, destination, weight))

                turn_definitions.add(source,
                                     destination,
                                     weight)

    return turn_definitions


def to_xml(turn_definitions):
    """ Transforms the given TurnDefinitions object into a string
        containing a valid SUMO turn-definitions file. """

    LOGGER.info("Converting turn definitions to XML")
    LOGGER.debug("Turn definitions sources number: %i" %
        (len(turn_definitions.get_sources())))

    turn_definitions_xml = xml.dom.minidom.Element("turn-defs")
    for source in turn_definitions.get_sources():
        LOGGER.debug("Converting turn definition with source %s" % (source))

        from_edge_element = xml.dom.minidom.Element("fromEdge")
        from_edge_element.setAttribute("id", source)

        for destination in turn_definitions.get_destinations(source):
            probability = turn_definitions.get_turning_probability(source,
                                                                   destination)

            LOGGER.debug("Converting turn definition destination %s "
                         "with probability %f" % (destination, probability))

            to_edge_element = xml.dom.minidom.Element("toEdge")
            to_edge_element.setAttribute("id", destination)
            to_edge_element.setAttribute("probability", str(probability))
            from_edge_element.appendChild(to_edge_element)

        turn_definitions_xml.appendChild(from_edge_element)

    return turn_definitions_xml.toprettyxml()


import collectinghandler
import unittest


class TurnDefinitionsTestCase(unittest.TestCase):
    # pylint: disable=C,R,W,E,F  

    def setUp(self):
        self.turn_definitions = TurnDefinitions()

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
               in [ record.getMessage()
                    for record in collecting_handler.log_records ])

    def test_probability_overflow_raises_warning_after_addition(self):
        collecting_handler = collectinghandler.CollectingHandler()
        self.turn_definitions.logger.addHandler(collecting_handler)

        self.turn_definitions.add("source", "destination", 90)
        self.turn_definitions.add("source", "destination", 11)

        self.assertTrue(
            "Turn probability overflow: 101.000000; lowered to 100" 
                in [ record.getMessage()
                     for record in collecting_handler.log_records ])

          
class CreateTurnDefinitionsTestCase(unittest.TestCase):
    # pylint: disable=C,R,W,E,F  

    def test_creation_with_0_sources(self):
        self.assertEqual(TurnDefinitions(),
            from_connections(connections.Connections()))

    def test_creation_with_one_source(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")

        turn_definitions = TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
            from_connections(input_connections))

    def test_creation_with_two_sources(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 2", "source lane 1", "destination 1")

        turn_definitions = TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)
        turn_definitions.add("source 2", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
            from_connections(input_connections))


    def test_creation_two_destinations_from_two_lanes_overlapping(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 2", "destination 2")
        input_connections.add("source 1", "source lane 2", "destination 1")

        turn_definitions = TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0/2/2+100.0/2)
        turn_definitions.add("source 1", "destination 2", 100.0/2/2)

        self.assertEqual(turn_definitions,
            from_connections(input_connections))

    def test_creation_one_destination_from_two_lanes(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 2", "destination 1")

        turn_definitions = TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
            from_connections(input_connections))

    def test_creation_with_one_destination_from_one_lane(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")

        turn_definitions = TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0)

        self.assertEqual(turn_definitions,
           from_connections(input_connections))

    def test_creation_with_two_destinations_from_one_lane(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 1", "destination 2")

        turn_definitions = TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0/2)
        turn_definitions.add("source 1", "destination 2", 100.0/2)

        self.assertEqual(turn_definitions,
            from_connections(input_connections))

    def test_creation_with_three_destinations_from_one_lane(self):
        input_connections = connections.Connections()
        input_connections.add("source 1", "source lane 1", "destination 1")
        input_connections.add("source 1", "source lane 1", "destination 2")
        input_connections.add("source 1", "source lane 1", "destination 3")
    
        turn_definitions = TurnDefinitions()
        turn_definitions.add("source 1", "destination 1", 100.0/3)
        turn_definitions.add("source 1", "destination 2", 100.0/3)
        turn_definitions.add("source 1", "destination 3", 100.0/3)

        self.assertEqual(turn_definitions,
            from_connections(input_connections))

if __name__ == "__main__":
    unittest.main()

