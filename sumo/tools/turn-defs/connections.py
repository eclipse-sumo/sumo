#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    turndefinitions.py
@author  Karol Stosiek
@date    2011-10-26
@version $Id$

Operations and classes necessary to operate on SUMO connections.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import logging
import xml.dom.minidom

LOGGER = logging.getLogger(__name__)

class UniformDestinationWeightCalculator():
    """Calculates weight for each destination from given lane,
       redistributing weights uniformly: first redistributes the total
       traffic among incoming lanes uniformly and then divides the total
       traffic for a lane among all of the destination lanes.
       
       For example, assume we  have a junction with one incoming road In1
       with 3 incoming lanes and 2 outgoing roads Out1 and Out2,
       having 2 and 3 outgoing lanes, respectively.
  
                   ___________________
            lane 0 _______     _______ lane 0
       Out2 lane 1 _______     _______ lane 1  In1
            lane 2 _______     _______ lane 2
                          | | |
                          | | |
                          | | |
                      lane 0 1
                           Out1
  
      Assume that lanes are connected as follows:
      - In1, lane 0 is connected to Out2, lane 0
      - In1, lane 1 is connected to Out2, lane 1
      - In1, lane 2 is connected to Out2, lane 2
      
      - In1, lane 1 is connected to Out1, lane 0
      - In1, lane 2 is connected to Out1, lane 1
  
       This weight calculator will redistribute weights as follows:
  
       1. Distribute the incoming traffic on In1 uniformly on lanes 0, 1 and 2:
          as a result, each lane has been assigned 33,(3)% of the traffic.
       2. Since In1, lane 0 is connected to Out2, lane 0, whole traffic
          from In1, lane 0 is redirected to Out2, lane 0, which makes 33,(3)%
          of the total incoming traffic from In1.
       3. Since In1, lane 1 is connected both to Out2, lane 1 and Out1, lane 0,
          the traffic on that lane (that is, 33,(3)% of the total traffic) is
          spread uniformly among these two lanes, resulting in 16,(6)% of the total
          traffic for each destination lane.
       4. Similarly, In2, lane 2's traffic is spread uniformly among Out2, lane 2
          and Out 1, lane 1, resulting in 16,(6)% of the total traffic for each
          destination lane.
  
       This, if we ask the calculator for weight assigned to In1, lane 0, we get
       33,(3) as a result; if we ask for weight assigned to In1, lane 1, we get
       16,(6) as a result.
  
       Note: If you want to provide your own weight calculator (based on Gaussian
       distribution, for example), simply provide a class with calculate_weight
       method. The method's signature may need to be changed in that case. """
  
    # pylint: disable=R0903

    logger = logging.getLogger(__name__)

    def __init__(self):
        pass
  
    def calculate_weight(self,
                         source_lane_no,
                         source_total_lanes,
                         destination_total_lanes):
  
        """ Calculates the weight assigned to a single destination 
            from given lane in an uniform way. See class docs
            for explanation. """
  
        lane_weight = 100.0 / source_total_lanes
        destination_weight = lane_weight / destination_total_lanes
  
        self.logger.debug("Destination weight for lane %s/%s connected to %s "
            "destinations: %f" %
                (str(source_lane_no), str(source_total_lanes),
                    str(destination_total_lanes), destination_weight))
  
        return destination_weight
  
class Connections:
    """ Represents all of the connections in the network we have read. """
  
    logger = logging.getLogger(__name__)
  
    def __init__(self,
                 calculator = UniformDestinationWeightCalculator()):
        """ Constructor. Allows providing own destination weight calculator,
            which defaults to UniformDestinationWeightCalculator if not "
            provided. """
  
        self.connections_map = { }
        self.destination_weight_calculator = calculator
  
    def add(self,
            source,
            source_lane,
            destination):
  
        """ Adds a connection. If a connection is readded, 
            a warning is issued. """
  
        self.logger.debug("Adding connection %s (%s) -> %s" %
            (str(source), str(source_lane), str(destination)))
  
        if source not in self.connections_map:
            self.logger.debug("Created new mapping for %s" %
                (str(source)))
            self.connections_map[source] = {}
  
        if source_lane not in self.connections_map[source]:
            self.logger.debug("Created new mapping for %s / %s" %
                (str(source), str(source_lane)))
            self.connections_map[source][source_lane] = set()
  
        if destination in self.connections_map[source][source_lane]:
            self.logger.warn("Destination for %s (lane %s) readded: %s"
                % (str(source), str(source_lane), str(destination)))
  
        self.connections_map[source][source_lane].add(destination)
  
    def get_sources(self):
        """ Returns all of the incoming edges that this connections collection
            contains. Incoming edges are sorted alphabetically
            in ascending order. """
  
        sources = self.connections_map.keys()
        sources.sort()
        return sources
  
    def get_lanes(self, source):
        """ Returns all lanes that have connections for the given edge.
            The connection_source must have been added before. """
  
        return self.connections_map[source].keys()
  
    def get_destinations(self, source, source_lane):
        """ Returns all possible destinations that are achievable from given
            lane on given edge. The connection_source
            and connection_source_lane must have been added before. """
  
        return self.connections_map[source][source_lane]
  
    def calculate_destination_weight(self,
                                     source,
                                     source_lane,
                                     destination):
        """ Calculates weight assigned to the given destination using
            weight calculator provided in class' constructor.
            The connection_source, connection_source_lane
            and connection_destination must have been added before."""
  
        weight = self.destination_weight_calculator.calculate_weight(
            source_lane,
            len(self.get_lanes(source)),
            len(self.get_destinations(source, source_lane)))
  
        self.logger.debug("Destination weight for connection "
            "%s (%s) -> %s: %f" %
                (str(source), str(source_lane), str(destination), weight))
  
        return weight
  
class ConnectionXML():
    """ Represents a raw, xml-defined connection. """
  
    def __init__(self, connection_xml):
        """ Constructor. The connection_xml argument must be a well-formed
            connection XML entity string. See SUMO docs for information on
            connection XML format. """
  
        self.connection_xml = connection_xml
        
    def get_source(self):
        """ Returns the connection's incoming edge. Raises AttributeError
            if the incoming edge is missing. """
  
        source = self.connection_xml.getAttribute("from")
        if source is '':
            raise AttributeError
        return source
  
    def get_source_lane(self):
        """ Returns the connection's incoming edge's lane.
            Raises AttributeError if the incoming edge's lane is missing. """
  
        source_lane = self.connection_xml.getAttribute("fromLane")
        if source_lane is '':
            raise AttributeError
        return source_lane
  
    def get_destination(self):
        """ Returns the connection's outgoing edge. Raises AttributeError
            if the outgoing edge is missing. """
  
        destination = self.connection_xml.getAttribute("to")
        if destination is '':
            raise AttributeError
        return destination
      
  
class ConnectionsXMLReader():
    """ Reads the XML connection definitions. """
    # pylint: disable=R0903

    logger = logging.getLogger("ConnectionsXMLReader")
  
    def __init__(self, connections_xml_stream):
        """ Constructor. The connections_xml_stream argument may be
            a filename or an opened file. """
  
        self.connections_xml = xml.dom.minidom.parse(connections_xml_stream)
  
    def get_connections(self):
        """ Returns connections defined in the XML stream provided in
            the constructor. """
  
        return [ ConnectionXML(connection_xml) for connection_xml in
            self.connections_xml.getElementsByTagName("connection") ]
  
  
  
def from_stream(input_connections):
    """ Constructs Connections object from connections defined in the given
        stream. The input_connections argument may be a filename or an opened
        file. """
  
    LOGGER.info("Reading connections from input stream")

    connections = Connections()
    connections_xml_reader = ConnectionsXMLReader(input_connections)
    for xml_connection in connections_xml_reader.get_connections():
        connections.add(xml_connection.get_source(),
                        xml_connection.get_source_lane(),
                        xml_connection.get_destination())
   
    return connections
  
import collectinghandler
import unittest
  
  
class UniformDestinationWeightCalculatorTestCase(unittest.TestCase):
    # pylint: disable=C,W,R,E,F 

    def setUp(self):
      self.calculator = UniformDestinationWeightCalculator()
  
    def tearDown(self):
      self.calculator = None
  
    def test_uniform_weight_spread_across_lanes(self):
      # 1 lane case 
      self.assertAlmostEqual(100.0, self.calculator.calculate_weight(0, 1, 1))
  
      # 2 lane case 
      self.assertAlmostEqual(50.0, self.calculator.calculate_weight(0, 2, 1))
      self.assertAlmostEqual(50.0, self.calculator.calculate_weight(1, 2, 1))
  
      # 2 lane case 
      self.assertAlmostEqual(100.0/3, self.calculator.calculate_weight(0, 3, 1))
      self.assertAlmostEqual(100.0/3, self.calculator.calculate_weight(1, 3, 1))
      self.assertAlmostEqual(100.0/3, self.calculator.calculate_weight(2, 3, 1))
  
    def test_uniform_weight_spread_across_destinations(self):
      # 1 destination
      self.assertAlmostEqual(100.0, self.calculator.calculate_weight(0, 1, 1))
  
      # 2 destinations
      self.assertAlmostEqual(100.0/2, self.calculator.calculate_weight(0, 1, 2))
  
      # 3 destinations
      self.assertAlmostEqual(100.0/3, self.calculator.calculate_weight(0, 1, 3))
  
  
class ConnectionsTestCase(unittest.TestCase):
    # pylint: disable=C,W,R,E,F 

    def setUp(self):
      self.connections = Connections()
  
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
      self.assert_contains_destination("source", "source lane", "destination") 
  
    def test_add_different_lanes(self):
      self.connections.add("source", "source lane 1", "destination")
      self.connections.add("source", "source lane 2", "destination")
  
      self.assert_sources_no(1)
      self.assert_contains_source("source")
      
      self.assert_lanes_no("source", 2)
      self.assert_contains_lane("source", "source lane 1")
      self.assert_contains_lane("source", "source lane 2")
  
      self.assert_destinations_no("source", "source lane 1", 1)
      self.assert_contains_destination("source", "source lane 1", "destination")
  
      self.assert_destinations_no("source", "source lane 2", 1)
      self.assert_contains_destination("source", "source lane 2", "destination")
   
    def test_add_different_destinations(self):
      self.connections.add("source", "source lane", "destination 1")
      self.connections.add("source", "source lane", "destination 2")
  
      self.assert_sources_no(1)
      self.assert_contains_source("source")
      
      self.assert_lanes_no("source", 1)
      self.assert_contains_lane("source", "source lane")
  
      self.assert_destinations_no("source", "source lane", 2)
      self.assert_contains_destination("source", "source lane", "destination 1")
      self.assert_contains_destination("source", "source lane", "destination 2")
   
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
  
  
class ConnectionXMLTestCase(unittest.TestCase):
    # pylint: disable=C,W,R,E,F 

    SOURCE_ATTRIBUTE = "from"
    SOURCE_LANE_ATTRIBUTE= "fromLane"
    DESTINATION_ATTRIBUTE = "to"
    DESTINATION_LANE_ATTRIBUTE = "toLane"
   
    def setUp(self):
      # Shared resources.
      self.from_value = "source"
      self.to_value = "destination"
      self.from_lane_value = "source lane"
      self.to_lane_value = "destination lane"
      
    def set_up_new_connection_element(self):
      connection_element = xml.dom.minidom.Element("connection")
      connection_element.setAttribute(self.SOURCE_ATTRIBUTE,
          self.from_value)
      connection_element.setAttribute(self.SOURCE_LANE_ATTRIBUTE,
          self.from_lane_value)
      connection_element.setAttribute(self.DESTINATION_ATTRIBUTE,
          self.to_value)
      connection_element.setAttribute(self.DESTINATION_LANE_ATTRIBUTE,
          self.to_lane_value)
      return connection_element
  
    def set_up_connection_xml_without(self, attribute):
      connection_element = self.set_up_new_connection_element()
      connection_element.removeAttribute(attribute)
      self.set_up_connection_xml(connection_element)
  
    def set_up_connection_xml(self, connection_element):
      self.connection_xml = ConnectionXML(connection_element)
  
    def tearDown(self):
      self.connection_xml = None
  
    def test_get_source(self):
      self.set_up_connection_xml(self.set_up_new_connection_element())
      self.assertEqual(self.from_value, self.connection_xml.get_source())
  
    def test_get_source_with_missing_source(self):
      self.set_up_connection_xml_without(self.SOURCE_ATTRIBUTE)
      self.assertRaises(AttributeError, self.connection_xml.get_source)
  
    def test_get_source_lane(self):
      self.set_up_connection_xml(self.set_up_new_connection_element())
      self.assertEqual(self.from_lane_value, self.connection_xml.get_source_lane())
  
    def test_get_source_lane_with_missing_source_lane(self):
      self.set_up_connection_xml_without(self.SOURCE_LANE_ATTRIBUTE)
      self.assertRaises(AttributeError, self.connection_xml.get_source_lane)
  
    def test_get_destination(self):
      self.set_up_connection_xml(self.set_up_new_connection_element())
      self.assertEqual(self.to_value, self.connection_xml.get_destination())
  
    def test_get_destination_with_missing_destination(self):
      self.set_up_connection_xml_without(self.DESTINATION_ATTRIBUTE)
      self.assertRaises(AttributeError, self.connection_xml.get_destination)
  
if __name__ == "__main__":
    unittest.main()
  
