#!/usr/bin/env python
"""
@file    unittest_sumolib_3d.py
@author  Marek Heinrich
@version $Id$

This script tests sumolib 3D functions

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2016-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import subprocess

import unittest

# Do not use SUMO_HOME here to ensure you are always testing the
# functions from the same tree the test is in
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools'))

import sumolib

NODEFILE_2D = 'input_nodes_2d.nod.xml'
NODEFILE_3D = 'input_nodes.nod.xml'
EDGEFILE = 'input_edges.edg.xml'
NETFILE_2D = 'input_net_2d.net.xml'
NETFILE_3D = 'input_net_3d.net.xml'


class Test_Shapes(unittest.TestCase):

    """ Tests to check inport of sumo elements with/without z coords. """

    @classmethod
    def setUpClass(cls):
        """ setup generates all sumo files - once. """

        netcon_bin = sumolib.checkBinary('netconvert')
#        print ('xxxxxxxxxxxxxxxxxxx', netcon_bin)

        for node_file, net_file in [
                #(NODEFILE_2D, NETFILE_2D),
                (NODEFILE_3D, NETFILE_3D)
        ]:

            command = [netcon_bin,
                       "-n", node_file,
                       "-e", EDGEFILE,
                       "-o", net_file,
                       "--offset.disable-normalization"]

            netconvertProcess = subprocess.call(
                command,
                stdout=sys.stdout,
                stderr=sys.stderr)

#        cls.sumo_net_2d = sumolib.net.readNet(
#            NETFILE_2D,
#            withInternal=True)

        cls.sumo_net = sumolib.net.readNet(
            NETFILE_3D,
            withInternal=True)

    @classmethod
    def tearDownClass(cls):
        """ remove the generated net file, once all tests ran """

        if os.path.exists(NETFILE_2D):
            os.remove(NETFILE_2D)
        if os.path.exists(NETFILE_3D):
            os.remove(NETFILE_3D)

    #### check node's coords ############################################
    @unittest.skipIf(False, '')
    def test_check_node_coords_2d_for_3d_input_node_no_z(self):
        """ test to retrive the coords from a node with no z value

            - should be a 2d coords tuple"""

        self.assertEqual(
            self.sumo_net.getNode('a0').getCoord(),
            (100.0, 0.0))

    @unittest.skipIf(False, '')
    def test_check_node_coords_3d_for_3d_input_node_z_no_z(self):
        """ test to retrive the coords from a node with no z value

            - should be a 3d coords tuple"""

        self.assertEqual(
            self.sumo_net.getNode('a0').getCoord3D(),
            (100.0, 0.0, 0.0))

    @unittest.skipIf(False, '')
    def test_check_node_coords_2d_for_3d_input_node_z_not_zero(self):
        """ test to retrive the coords from a node with z!=0

            - should be a 3d coords tuple"""

        self.assertEqual(
            self.sumo_net.getNode('a1').getCoord(),
            (200.0, 0.0))

    @unittest.skipIf(False, '')
    def test_check_node_coords_3d_for_3d_input_node_z_not_zero(self):
        """ test to retrive the coords from a node with z!=0

            - should be a 3d coords tuple"""

        self.assertEqual(
            self.sumo_net.getNode('a1').getCoord3D(),
            (200.0, 0.0, 10.0))

    #### check node's shape #############################################

    @unittest.skipIf(False, '')
    def test_check_node_shape_2d_on_a_node_with_no_z(self):
        """ test to retrive the shape from a node with no z value

            - should be a 2d coords tuple"""

        result = self.sumo_net.getNode('a0').getShape()
        self.assertTrue(len(result) > 0)
        for shape_point in result:
            self.assertTrue(len(shape_point) == 2)

    @unittest.skipIf(False, '')
    def test_check_node_shape_3d_on_a_node_with_no_z(self):
        """ test to retrive the shape from a node with no z value

            - should be a 3d coords tuple"""
        result = self.sumo_net.getNode('a0').getShape3D()
        self.assertTrue(len(result) > 0)
        for shape_point in result:
            self.assertTrue(len(shape_point) == 3)

    @unittest.skipIf(False, '')
    def test_check_node_shape_2d_on_a_node_with_z(self):
        """ test to retrive the shape from a node with z value

            - should be a 2d coords tuple"""

        result = self.sumo_net.getNode('a1').getShape3D()
        self.assertTrue(len(result) > 0)
        for shape_point in result:
            self.assertTrue(len(shape_point) == 3)

    @unittest.skipIf(False, '')
    def test_check_node_shape_3d_on_a_node_with_z(self):
        """ test to retrive the shape from a node with z value

            - should be a 3d coords tuple"""

        result = self.sumo_net.getNode('a1').getShape3D()
        self.assertTrue(len(result) > 0)
        for shape_point in result:
            self.assertTrue(len(shape_point) == 3)

    #### check edge's shape #############################################
    @unittest.skipIf(False, '')
    def test_h001_edge_shape_not_stored(self):
        """ 

        The edge is the center line of an H (both directions,
        one lane per edge).

        Junction shapes are engaged so the lane of the edge 
        is somewhat shorter at the start and at the end.

        Shape of the edge _is_not_ stored in sumo's net xml

        """

        edge_id = 'center_we'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'center_we_0'

        #### check edge shape ################################
        expected_result_raw_edge_shape_2D = \
            [(1000, 100), (1200, 100)]

        expected_result_raw_edge_shape_3D = \
            [(1000, 100, 10), (1200, 100, 10)]

        result_raw_edge_shape_2D = the_edge.getRawShape()
        result_raw_edge_shape_3D = the_edge.getRawShape3D()

        self.assertEqual(result_raw_edge_shape_2D,
                         expected_result_raw_edge_shape_2D)

        self.assertEqual(result_raw_edge_shape_3D,
                         expected_result_raw_edge_shape_3D)

    @unittest.skipIf(False, '')
    def test_h001_edge_shape_not_stored(self):
        """ 

        The edge is the center line of an H (both directions,
        one lane per edge) with one extra shape point.

        Junction shapes are engaged so the lane of the edge 
        is somewhat shorter at the start and at the end.

        Shape of the edge _is_ stored in sumo's net xml

        """

        edge_id = 'center_ew'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'center_we_0'

        #### check edge shape ################################
        expected_result_raw_edge_shape_2D = \
            [(1200, 100), (1100, 125), (1000, 100)]

        expected_result_raw_edge_shape_3D = \
            [(1200, 100, 10), (1100, 125, 10), (1000, 100, 10)]

        result_raw_edge_shape_2D = the_edge.getRawShape()
        result_raw_edge_shape_3D = the_edge.getRawShape3D()

        self.assertEqual(result_raw_edge_shape_2D,
                         expected_result_raw_edge_shape_2D)

        self.assertEqual(result_raw_edge_shape_3D,
                         expected_result_raw_edge_shape_3D)

    #### check lane's shape #############################################
    @unittest.skipIf(False, '')
    def test_h001_lane_shape_2d(self):
        """ 

        The edge is the center line of an H (both directions,
        one lane per edge).

        Junction shapes are engaged so the lane of the edge 
        is somewhat shorter at the start and at the end.

        2d version
        """

        edge_id = 'center_we'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'center_we_0'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 2)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_end_point_wo = result_lane_shape_without_junc[1]

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wo[0] < 1200)   # x
        self.assertTrue(90 < result_start_point_wo[1] < 100)   # y

        # check second shape point coords
        self.assertTrue(1000 < result_end_point_wo[0] < 1200)     # x
        self.assertTrue(90 < result_end_point_wo[1] < 100)     # y

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc = \
            the_lane.getShape(includeJunctions=True)

        self.assertTrue(len(result_lane_shape_with_junc) == 4)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_end_point_wi = result_lane_shape_with_junc[2]
        result_to_point_wi = result_lane_shape_with_junc[3]

        # check fromNode coords
        self.assertEqual(result_from_point_wi, (1000, 100))

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wi[0] < 1200)   # x
        self.assertTrue(90 < result_start_point_wi[1] < 100)   # y

        # check second shape point coords
        self.assertTrue(1000 < result_end_point_wi[0] < 1200)     # x
        self.assertTrue(90 < result_end_point_wi[1] < 100)     # y

        # check toNode coords
        self.assertEqual(result_to_point_wi, (1200, 100))

    @unittest.skipIf(False, '')
    def test_h001_lane_shape_3d(self):
        """ 

        The edge is the center line of an H (both directions,
        one lane per edge).

        Junction shapes are engaged so the lane of the edge 
        is somewhat shorter at the start and at the end.

        3d version
        """

        edge_id = 'center_we'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'center_we_0'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape3D(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 2)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_end_point_wo = result_lane_shape_without_junc[1]

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wo[0] < 1200)   # x
        self.assertTrue(90 < result_start_point_wo[1] < 100)   # y
        self.assertTrue(result_start_point_wo[2] == 10)   # z

        # check second shape point coords
        self.assertTrue(1000 < result_end_point_wo[0] < 1200)     # x
        self.assertTrue(90 < result_end_point_wo[1] < 100)     # y
        self.assertTrue(result_end_point_wo[2] == 10)     # z

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc = \
            the_lane.getShape3D(includeJunctions=True)

        self.assertTrue(len(result_lane_shape_with_junc) == 4)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_end_point_wi = result_lane_shape_with_junc[2]
        result_to_point_wi = result_lane_shape_with_junc[3]

        # check fromNode coords
        self.assertEqual(result_from_point_wi, (1000, 100, 10))

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wi[0] < 1200)   # x
        self.assertTrue(90 < result_start_point_wi[1] < 100)   # y
        self.assertTrue(result_start_point_wi[2] == 10)   # z

        # check second shape point coords
        self.assertTrue(1000 < result_end_point_wi[0] < 1200)     # x
        self.assertTrue(90 < result_end_point_wi[1] < 100)     # y
        self.assertTrue(result_end_point_wi[2] == 10)     # z

        # check toNode coords
        self.assertEqual(result_to_point_wi, (1200, 100, 10))

    @unittest.skipIf(False, '')
    def test_h003_lane_shape_2d(self):
        """ 

        The edge is the we-center line of an H (both directions,
        one lane per edge).

        This edge is not a straight line but has shape points defined.

        Junction shapes are engaged so the lanes of the edge 
        are somewhat shorter at the start and at the end.

        Still the edge goes from from to to node, so the shape 
        should start and end with these coords.

        2d version.
        """

        edge_id = 'center_ew'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(1)  # 'center_ew_1'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 3)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_extra_point_wo = result_lane_shape_without_junc[1]
        result_end_point_wo = result_lane_shape_without_junc[2]

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wo[0] < 1200)  # x
        self.assertTrue(100 < result_start_point_wo[1] < 110)  # y

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wo[0] == 1100)  # x
        self.assertTrue(125 < result_extra_point_wo[1] < 150)  # y

        # check third shape point coords
        self.assertTrue(1000 < result_end_point_wo[0] < 1200)  # x
        self.assertTrue(100 < result_end_point_wo[1] < 110)  # y

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc    = \
            the_lane.getShape(includeJunctions=True)

        self.assertTrue(len(result_lane_shape_with_junc) == 5)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_extra_point_wi = result_lane_shape_with_junc[2]
        result_end_point_wi = result_lane_shape_with_junc[3]
        result_to_point_wi = result_lane_shape_with_junc[4]

        # check fromNode coords
        self.assertEqual(result_from_point_wi,
                         (1200, 100))

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wi[0] < 1200)  # x
        self.assertTrue(100 < result_start_point_wi[1] < 110)  # y

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wi[0] == 1100)  # x
        self.assertTrue(125 < result_extra_point_wi[1] < 150)  # y

        # check third shape point coords
        self.assertTrue(1000 < result_end_point_wi[0] < 1200)  # x
        self.assertTrue(100 < result_end_point_wi[1] < 110)  # y

        # check toNode coords
        self.assertEqual(result_to_point_wi,
                         (1000, 100))

    @unittest.skipIf(False, '')
    def test_h003_lane_shape_3d(self):
        """ 

        The edge is the we-center line of an H (both directions,
        one lane per edge).

        This edge is not a straight line but has shape points defined.

        Junction shapes are engaged so the lanes of the edge 
        are somewhat shorter at the start and at the end.

        Still the edge goes from from to to node, so the shape 
        should start and end with these coords.

        3d version.
        """

        edge_id = 'center_ew'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(1)  # 'center_ew_1'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape3D(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 3)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_extra_point_wo = result_lane_shape_without_junc[1]
        result_end_point_wo = result_lane_shape_without_junc[2]

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wo[0] < 1200)  # x
        self.assertTrue(100 < result_start_point_wo[1] < 110)  # y
        self.assertTrue(result_start_point_wo[2] == 10)  # z

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wo[0] == 1100)  # x
        self.assertTrue(125 < result_extra_point_wo[1] < 150)  # y
        self.assertTrue(result_extra_point_wo[2] == 10)  # z

        # check third shape point coords
        self.assertTrue(1000 < result_end_point_wo[0] < 1200)  # x
        self.assertTrue(100 < result_end_point_wo[1] < 110)  # y
        self.assertTrue(result_end_point_wo[2] == 10)  # z

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc    = \
            the_lane.getShape3D(includeJunctions=True)

        self.assertTrue(len(result_lane_shape_with_junc) == 5)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_extra_point_wi = result_lane_shape_with_junc[2]
        result_end_point_wi = result_lane_shape_with_junc[3]
        result_to_point_wi = result_lane_shape_with_junc[4]

        # check fromNode coords
        self.assertEqual(result_from_point_wi,
                         (1200, 100, 10))

        # check first shape point coords
        self.assertTrue(1000 < result_start_point_wi[0] < 1200)  # x
        self.assertTrue(100 < result_start_point_wi[1] < 110)  # y
        self.assertTrue(result_start_point_wi[2] == 10)  # z

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wi[0] == 1100)  # x
        self.assertTrue(125 < result_extra_point_wi[1] < 150)  # y
        self.assertTrue(result_extra_point_wi[2] == 10)  # z

        # check third shape point coords
        self.assertTrue(1000 < result_end_point_wi[0] < 1200)  # x
        self.assertTrue(100 < result_end_point_wi[1] < 110)  # y
        self.assertTrue(result_end_point_wi[2] == 10)  # z

        # check toNode coords
        self.assertEqual(result_to_point_wi,
                         (1000, 100, 10))

    @unittest.skipIf(False, '')
    def test_h004_lane_shape_2d(self):
        """Get an internal lane and its shape. 

        Shape should not be influenced by the incluldeJunc parameter

        Use left cross of the H for this test.

        2d version.
        """

        edge_id = ':left_center_3'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # ':left_center_3_0'

        result_lane_shape_with_junc    = \
            the_lane.getShape(includeJunctions=True)

        result_lane_shape_without_junc    = \
            the_lane.getShape(includeJunctions=False)

        # there should be no difference between the two results
        self.assertEqual(result_lane_shape_with_junc,
                         result_lane_shape_without_junc)

        # there must be at least two shape points
        self.assertTrue(len(result_lane_shape_without_junc) >= 2)

        # each shape point should be somewhat close the the junction
        # and on the same z-level

        for shape_point in result_lane_shape_without_junc:
            self.assertTrue(995 < shape_point[0] < 1005)
            self.assertTrue(90 < shape_point[1] < 110)

    @unittest.skipIf(False, '')
    def test_h004_lane_shape_3d(self):
        """Get an internal lane and its shape. 

        Shape should not be influenced by the incluldeJunc parameter

        Use left cross of the H for this test.

        3d version.
        """

        edge_id = ':left_center_3'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # ':left_center_3_0'

        result_lane_shape_with_junc    = \
            the_lane.getShape3D(includeJunctions=True)

        result_lane_shape_without_junc    = \
            the_lane.getShape3D(includeJunctions=False)

        # there should be no difference between the two results
        self.assertEqual(result_lane_shape_with_junc,
                         result_lane_shape_without_junc)

        # there must be at least two shape points
        self.assertTrue(len(result_lane_shape_without_junc) >= 2)

        # each shape point should be somewhat close the the junction
        # and on the same z-level

        for shape_point in result_lane_shape_without_junc:
            self.assertTrue(995 < shape_point[0] < 1005)
            self.assertTrue(90 < shape_point[1] < 110)
            self.assertTrue(shape_point[2] == 10)

    @unittest.skipIf(False, '')
    def test_edge_001_lane_shape_2d(self):
        """ 

        Both way edge is the straight line between two nodes 
        edge has no extra shape points - no intersections engaged.  
        Edge is not centered.

        2d version.
        """

        edge_id = 'straight_with_counter'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'straight_with_counter_0'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 2)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_end_point_wo = result_lane_shape_without_junc[1]

        # check first shape point coords
        self.assertTrue(100 <= result_start_point_wo[0] <= 200)   # x
        self.assertTrue(-10 <= result_start_point_wo[1] < 0)   # y

        # check second shape point coords
        self.assertTrue(100 <= result_end_point_wo[0] <= 200)   # x
        self.assertTrue(-10 <= result_end_point_wo[1] < 0)   # y

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc    = \
            the_lane.getShape(includeJunctions=True)

        self.assertTrue(len(result_lane_shape_with_junc) == 4)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_end_point_wi = result_lane_shape_with_junc[2]
        result_to_point_wi = result_lane_shape_with_junc[3]

        # check fromNode coords
        self.assertEqual(result_from_point_wi,
                         (100, 0))

        # check first shape point coords
        self.assertTrue(100 <= result_start_point_wi[0] <= 200)   # x
        self.assertTrue(-10 <= result_start_point_wi[1] < 0)   # y

        # check second shape point coords
        self.assertTrue(100 <= result_end_point_wi[0] <= 200)   # x
        self.assertTrue(-10 <= result_end_point_wi[1] < 0)   # y

        # check toNode coords
        self.assertEqual(result_to_point_wi,
                         (200, 0))

    @unittest.skipIf(False, '')
    def test_edge_001_lane_shape_3d(self):
        """ 

        Both way edge is the straight line between two nodes 
        edge has no extra shape points - no intersections engaged.  
        Edge is not centered.

        3d version.
        """

        edge_id = 'straight_with_counter'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'straight_with_counter_0'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape3D(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 2)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_end_point_wo = result_lane_shape_without_junc[1]

        # check first shape point coords
        self.assertTrue(100 <= result_start_point_wo[0] <= 200)   # x
        self.assertTrue(-10 <= result_start_point_wo[1] < 0)   # y
        self.assertTrue(result_start_point_wo[2] == 0)   # z

        # check second shape point coords
        self.assertTrue(100 <= result_end_point_wo[0] <= 200)   # x
        self.assertTrue(-10 <= result_end_point_wo[1] < 0)   # y
        self.assertTrue(result_end_point_wo[2] == 10)   # z

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc    = \
            the_lane.getShape3D(includeJunctions=True)

        self.assertTrue(len(result_lane_shape_with_junc) == 4)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_end_point_wi = result_lane_shape_with_junc[2]
        result_to_point_wi = result_lane_shape_with_junc[3]

        # check fromNode coords
        self.assertEqual(result_from_point_wi,
                         (100, 0,  0))

        # check first shape point coords
        self.assertTrue(100 <= result_start_point_wi[0] <= 200)   # x
        self.assertTrue(-10 <= result_start_point_wi[1] < 0)   # y
        self.assertTrue(result_start_point_wi[2] == 0)   # z

        # check second shape point coords
        self.assertTrue(100 <= result_end_point_wi[0] <= 200)   # x
        self.assertTrue(-10 <= result_end_point_wi[1] < 0)   # y
        self.assertTrue(result_end_point_wi[2] == 10)   # z

        # check toNode coords
        self.assertEqual(result_to_point_wi,
                         (200, 0, 10))

    @unittest.skipIf(False, '')
    def test_sloopy_edge_003_lane_shape_2d(self):
        """ 

        Both way edge which is a sloopy line between two Nodes 
        since the edge has extra shape points 
        - no intersections engaged.  

        There was only one shape point defined in the edge.xml
        The coord of the from and to node where not included
        (since this is optional 
        - the counder direction does inclued them - see below)

        2d version.
        """

        edge_id = 'sloopy_we'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'sloopy_we_0'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 3)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_extra_point_wo = result_lane_shape_without_junc[1]
        result_end_point_wo = result_lane_shape_without_junc[2]

        # check first shape point coords
        self.assertTrue(3000 <= result_start_point_wo[0] <= 3500)  # x
        self.assertTrue(190 <= result_start_point_wo[1] < 200)  # y

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wo[0] == 3250)  # x
        self.assertTrue(230 <= result_extra_point_wo[1] < 250)  # y

        # check third shape point coords
        self.assertTrue(3000 <= result_end_point_wo[0] <= 3500)  # x
        self.assertTrue(190 <= result_end_point_wo[1] < 200)  # y

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc    = \
            the_lane.getShape(includeJunctions=True)

        self.assertEqual(len(result_lane_shape_with_junc), 5)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_extra_point_wi = result_lane_shape_with_junc[2]
        result_end_point_wi = result_lane_shape_with_junc[3]
        result_to_point_wi = result_lane_shape_with_junc[4]

        # check fromNode coords
        self.assertEqual(result_from_point_wi,
                         (3000, 200))

        # check first shape point coords
        self.assertTrue(3000 <= result_start_point_wi[0] <= 3500)  # x
        self.assertTrue(190 <= result_start_point_wi[1] < 200)  # y

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wi[0] == 3250)  # x
        self.assertTrue(230 <= result_extra_point_wi[1] < 250)  # y

        # check third shape point coords
        self.assertTrue(3000 <= result_end_point_wi[0] <= 3500)  # x
        self.assertTrue(190 <= result_end_point_wi[1] < 200)  # y

        # check toNode coords
        self.assertEqual(result_to_point_wi,
                         (3500, 200))

    @unittest.skipIf(False, '')
    def test_sloopy_edge_003_lane_shape_3d(self):
        """ 

        Both way edge which is a sloopy line between two Nodes 
        since the edge has extra shape points 
        - no intersections engaged.  

        There was only one shape point defined in the edge.xml
        The coord of the from and to node where not included
        (since this is optional 
        - the counder direction does inclued them - see below)

        3d version.
        """

        edge_id = 'sloopy_we'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # 'sloopy_we_0'

        #### check lane shape - without junction included ####

        result_lane_shape_without_junc    = \
            the_lane.getShape3D(includeJunctions=False)

        self.assertTrue(len(result_lane_shape_without_junc) == 3)

        result_start_point_wo = result_lane_shape_without_junc[0]
        result_extra_point_wo = result_lane_shape_without_junc[1]
        result_end_point_wo = result_lane_shape_without_junc[2]

        # check first shape point coords
        self.assertTrue(3000 <= result_start_point_wo[0] <= 3500)  # x
        self.assertTrue(190 <= result_start_point_wo[1] < 200)  # y
        self.assertTrue(result_start_point_wo[2] == 10)  # z

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wo[0] == 3250)  # x
        self.assertTrue(230 <= result_extra_point_wo[1] < 250)  # y
        self.assertTrue(result_extra_point_wo[2] == 10)  # z

        # check third shape point coords
        self.assertTrue(3000 <= result_end_point_wo[0] <= 3500)  # x
        self.assertTrue(190 <= result_end_point_wo[1] < 200)  # y
        self.assertTrue(result_end_point_wo[2] == 10)  # z

        #### check lane shape - with junction included #######

        result_lane_shape_with_junc    = \
            the_lane.getShape3D(includeJunctions=True)

        self.assertEqual(len(result_lane_shape_with_junc), 5)

        result_from_point_wi = result_lane_shape_with_junc[0]
        result_start_point_wi = result_lane_shape_with_junc[1]
        result_extra_point_wi = result_lane_shape_with_junc[2]
        result_end_point_wi = result_lane_shape_with_junc[3]
        result_to_point_wi = result_lane_shape_with_junc[4]

        # check fromNode coords
        self.assertEqual(result_from_point_wi,
                         (3000, 200, 10))

        # check first shape point coords
        self.assertTrue(3000 <= result_start_point_wi[0] <= 3500)  # x
        self.assertTrue(190 <= result_start_point_wi[1] < 200)  # y
        self.assertTrue(result_start_point_wi[2] == 10)  # z

        # check second shape point coords - extra point
        self.assertTrue(result_extra_point_wi[0] == 3250)  # x
        self.assertTrue(230 <= result_extra_point_wi[1] < 250)  # y
        self.assertTrue(result_extra_point_wi[2] == 10)  # z

        # check third shape point coords
        self.assertTrue(3000 <= result_end_point_wi[0] <= 3500)  # x
        self.assertTrue(190 <= result_end_point_wi[1] < 200)  # y
        self.assertTrue(result_end_point_wi[2] == 10)  # z

        # check toNode coords
        self.assertEqual(result_to_point_wi,
                         (3500, 200, 10))

    @unittest.skipIf(False, '')
    def test_straight_edge_005_lane_shape_2d(self):
        """ 

        Single way edge with spread type center - no shape points 
        - no intersections engaged.  

        Shape of edge and lane are identic. 

        No junctions are included.

        2d version.
        """

        edge_id = 'straight_no_counter'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # straight_no_counter_0

        expected_result     = \
            [(100.0, 100.0), (200.0, 100.0)]

        #### check lane shape ################################

        result_lane_shape_with_junc    = \
            the_lane.getShape(includeJunctions=True)

        result_lane_shape_without_junc = \
            the_lane.getShape(includeJunctions=False)

        self.assertEqual(result_lane_shape_with_junc,
                         expected_result)

        self.assertEqual(result_lane_shape_without_junc,
                         expected_result)

    @unittest.skipIf(False, '')
    def test_straight_edge_005_lane_shape(self):
        """ 

        Single way edge with spread type center - no shape points 
        - no intersections engaged.  

        Shape of edge and lane are identic. 

        No junctions are included.

        3d version.
        """

        edge_id = 'straight_no_counter'
        the_edge = self.sumo_net.getEdge(edge_id)
        the_lane = the_edge.getLane(0)  # straight_no_counter_0

        expected_result     = \
            [(100.0, 100.0, 10.0),  (200.0, 100.0, 10.0)]

        #### check lane shape ################################

        result_lane_shape_with_junc    = \
            the_lane.getShape3D(includeJunctions=True)

        result_lane_shape_without_junc = \
            the_lane.getShape3D(includeJunctions=False)

        self.assertEqual(result_lane_shape_with_junc,
                         expected_result)

        self.assertEqual(result_lane_shape_without_junc,
                         expected_result)


class Test_ShapesConvertion(unittest.TestCase):

    """ """

    @unittest.skipIf(False, '')
    def test_convertShape_empty_string(self):
        """Inspecting the sumolib's function convertShape()"""

        self.assertEqual(sumolib.net.convertShape(''), [])

    @unittest.skipIf(False, '')
    def test_convertShape_string_2d_one_point(self):
        """Inspecting the sumolib's function convertShape() with one 2d point"""

        self.assertEqual(sumolib.net.convertShape('10,11'), [(10, 11, 0)])

    @unittest.skipIf(False, '')
    def test_convertShape_string_2d_two_point(self):
        """Inspecting the sumolib's function convertShape() with two 2d points"""

        self.assertEqual(sumolib.net.convertShape('10,11 12,13 '),
                         [(10, 11, 0), (12, 13, 0)])

    @unittest.skipIf(False, '')
    def test_convertShape_string_3d_one_point(self):
        """Inspecting the sumolib's function convertShape() with one 3d point"""

        self.assertEqual(sumolib.net.convertShape('10,11,5'), [(10, 11, 5)])

    @unittest.skipIf(False, '')
    def test_convertShape_string_3d_two_point(self):
        """Inspecting the sumolib's function convertShape() with two 3d points"""

        self.assertEqual(sumolib.net.convertShape('10,11,5 12,13,5'),
                         [(10, 11, 5), (12, 13, 5)])


if __name__ == '__main__':
    unittest.main()
