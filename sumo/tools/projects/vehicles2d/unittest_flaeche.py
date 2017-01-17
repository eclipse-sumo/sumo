# -*- coding: utf-8 -*-
"""
@file    unittest_flaeche.py
@author  Marek Heinrich
@date    2015-02-24
@version $Id$

Test module for flaeche functions.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


import unittest
import math
import main

from unittest_constants import VISUAL, lost, finished, afinished, unfinished, devel_skip, devel_run, broken, visual, known_bug, debugging


class testCaseFlaeche(unittest.TestCase):

    @unittest.skipIf(finished, 'done')
    def test_flaeche(self):
        """constuctor test"""

        myFlaeche = main.Flaeche(xdim=20, ydim=10, scale=1)
        self.assertEqual(myFlaeche.cluster_length_x, 20)
        self.assertEqual(myFlaeche.cluster_length_y, 10)
        # return myId = 1

    @unittest.skipIf(finished, 'done')
    def test_flaeche_round(self):
        """rounding down dimention

        when Flaechen length is not a multipple of scale  """

        myFlaeche = main.Flaeche(xdim=20.5, ydim=20.5, scale=1)
        self.assertEqual(myFlaeche.cluster_length_x, 20)
        self.assertEqual(myFlaeche.cluster_length_y, 20)
        # return myId = 2

    @unittest.skipIf(finished, 'done')
    def test_flaeche_clustersize(self):
        """create Flaeche with scale not 1"""

        myId = 3
        myFlaeche = main.Flaeche(xdim=20.5, ydim=20.5, scale=0.5)
        self.assertEqual(myFlaeche.cluster_length_x, 41)
        self.assertEqual(myFlaeche.cluster_length_x, 41)

    @unittest.skipIf(devel_run, 'done')
    def test_flaeche_empty_copy(self):
        """get an empty copy of the flaeche and check some values"""

        myFlaeche = main.Flaeche(xdim=20, ydim=10, scale=1)
        myCopy = myFlaeche.get_empty_gird_copy()

        self.assertEqual(20, myCopy.cluster_length_x)
        self.assertEqual(10, myCopy.cluster_length_y)
        self.assertEqual(1,  myCopy.scale)
        self.assertEqual(16, myCopy.sectors)

    @unittest.skipIf(finished, 'done')
    def test_get_neighbours(self):
        """test to receive any neigbour, regardless wheather legal or not"""

        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=1)

        self.assertEqual([(1, 1), (1, 2), (1, 3), (2, 1), (2, 3), (3, 1), (3, 2), (3, 3)],
                         myFlaeche.get_neighbours((2, 2)))

        self.assertEqual([(0, 1), (0, 3), (1, 1), (1, 2), (1, 3)],
                         myFlaeche.get_neighbours((0, 2)))

        # return myId = 5

    @unittest.skipIf(finished, 'done')
    def test_neighbours(self):
        """test to vertify two cells are neibourghs """

        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=1)
        self.assertTrue(myFlaeche.neighbours((2, 2), (1, 1)))
        self.assertTrue(myFlaeche.neighbours((2, 2), (2, 3)))
        self.assertFalse(myFlaeche.neighbours((2, 2), (2, 4)))
        self.assertRaises(AssertionError, myFlaeche.neighbours, (2, 2), (2, 2))

    @unittest.skipIf(devel_run, '')
    def test_flaeche_coord_is_legal(self):
        """test to vertify that a  given position is within
           the limits of the flaeche"""
        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=0.5)
        self.assertFalse(myFlaeche.is_valid_coord_pos((-1,  0)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((0, -1)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((-1, -1)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((31,  0)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((8, 31)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((31, 31)))
        self.assertTrue(myFlaeche.is_valid_coord_pos((7, 7)))

    @unittest.skipIf(finished, 'done')
    def test_flaeche_node_is_legal(self):
        """test to vertify if a node is inside a Flaeche"""

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=1)
        self.assertFalse(myFlaeche.is_valid_node_pos((200, 200)))
        self.assertTrue(myFlaeche.is_valid_node_pos((0, 0)))

        self.assertTrue(myFlaeche.is_valid_node_pos((0,  0,  0)))
        self.assertFalse(myFlaeche.is_valid_node_pos((200, 200,  0)))
        self.assertFalse(myFlaeche.is_valid_node_pos((0,  0, -2)))
        self.assertFalse(myFlaeche.is_valid_node_pos((0,  0, 22)))

    @unittest.skipIf(finished, 'done')
    def test_flaeche_sector_id_is_legal(self):
        """test to vertify if a sector_id is conform with what is expected """

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=1, sectors=16)

        self.assertRaisesRegexp(AssertionError, "sector_id must be an integer",
                                myFlaeche.is_valid_sector_id, '1')

        self.assertRaisesRegexp(AssertionError, "sector_id must be greater equal zero",
                                myFlaeche.is_valid_sector_id, -1)

        self.assertRaisesRegexp(AssertionError, "sector_id is greater then the number of sectors",
                                myFlaeche.is_valid_sector_id, 16)
        self.assertRaisesRegexp(AssertionError, "sector_id is greater then the number of sectors",
                                myFlaeche.is_valid_sector_id, 17)

        self.assertTrue(myFlaeche.is_valid_sector_id(1))
        self.assertTrue(myFlaeche.is_valid_sector_id(15))

    @unittest.skipIf(finished, 'done')
    def test_flaeche_get_cell_and_cells(self):
        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=0.7)
        self.assertEqual(myFlaeche.get_cell((21.5, 21.5)), (30, 30))

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=1)

        self.assertEqual(myFlaeche.get_cell((15.5, 15.5)), (15, 15))
        self.assertEqual(myFlaeche.get_cell((15, 15)), (15, 15))

        # cells
        self.assertEqual(myFlaeche.get_cells((15.5, 15.5)), [(15, 15)])
        self.assertEqual(myFlaeche.get_cells((15, 15.5)), [(14, 15), (15, 15)])
        self.assertEqual(myFlaeche.get_cells((0, 15.5)), [(-1, 15), (0, 15)])

        self.assertEqual(myFlaeche.get_cells((15.5, 15)), [(15, 14), (15, 15)])
        self.assertEqual(myFlaeche.get_cells((15.5, 0)), [(15, -1), (15, 0)])

        self.assertEqual(
            myFlaeche.get_cells((15, 15)), [(14, 14), (14, 15), (15, 14), (15, 15)])
        self.assertEqual(
            myFlaeche.get_cells((15, 0)), [(14, -1), (14, 0), (15, -1), (15, 0)])
        self.assertEqual(
            myFlaeche.get_cells((0, 15)), [(-1, 14), (-1, 15), (0, 14), (0, 15)])
        self.assertEqual(myFlaeche.get_cells((0.5, 0.5)), [(0, 0)])

        # cells negative
        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=10)

        self.assertEqual(myFlaeche.get_cells((0.5, 0.5)), [(0, 0)])  # redunant
        self.assertEqual(myFlaeche.get_cells((-0.5, -0.5)), [(-1, -1)])
        self.assertEqual(myFlaeche.get_cells((-5, -5)), [(-1, -1)])
        self.assertEqual(myFlaeche.get_cells((-10, -5)), [(-2, -1), (-1, -1)])
        self.assertEqual(myFlaeche.get_cells((-5, 0)), [(-1, -1), (-1, 0)])
        self.assertEqual(
            myFlaeche.get_cells((0, 0)), [(-1, -1), (-1, 0), (0, -1), (0, 0)])

    def test_get_cell_and_sector(self):
        """test to vertify if a node is inside a Flaeche"""

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=1)
        self.assertEqual(myFlaeche.get_cell_and_sector((1.5, 1.5), 0)[1],  0)
        self.assertEqual(
            myFlaeche.get_cell_and_sector((1.5, 1.5), 0.0001)[1],  0)
        self.assertEqual(
            myFlaeche.get_cell_and_sector((1.5, 1.5), 2 * math.pi / 16 / 2)[1],  0)
        self.assertEqual(myFlaeche.get_cell_and_sector(
            (1.5, 1.5), 2 * math.pi / 16 / 2 + 0.001)[1], 1)
        self.assertEqual(
            myFlaeche.get_cell_and_sector((1.5, 1.5), 2 * math.pi - 0.001)[1],  0)
        self.assertEqual(
            myFlaeche.get_cell_and_sector((1.5, 1.5), math.pi - 0.001)[1],  8)

        # multi_turn
        self.assertEqual(
            myFlaeche.get_cell_and_sector((1.5, 1.5), 2 * math.pi)[1],  0)
        self.assertEqual(myFlaeche.get_cell_and_sector(
            (1.5, 1.5), 4 * math.pi / 16 / 2 + 0.001)[1], 1)

    @unittest.skipIf(finished, 'done')
    def test_get_possition_from_cell_center_id(self):
        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=1)
        self.assertEqual(
            myFlaeche.get_possition_from_cell_center_id((0, 0)),  (0.5, 0.5))
        self.assertEqual(
            myFlaeche.get_possition_from_cell_center_id((1, 1)),  (1.5, 1.5))
        self.assertEqual(
            myFlaeche.get_possition_from_cell_center_id((1, 2)),  (1.5, 2.5))

    @unittest.skipIf(finished, 'done')
    def test_convert_trajectroy_points_to_bypassed_cells(self):
        """test to find the propper cells of a series of points"""

        def get_trajectory(print_traj=True):
            vessel = main.Vessel(myFlaeche,
                                 [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                                  (-10, -10), (0, 0)])
            result_pos = vessel.get_points_on_the_circle_ego(radius_to_point=40,
                                                             angle_ego=0.5 * math.pi)

            result_pos = [(pp[0] + 5, pp[1] + 5, pp[2]) for pp in result_pos]

            if print_traj:
                print result_pos

            return result_pos

        visual = VISUAL

        #########################
        # basic test
        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=10)
        trajectory = [(5, 5, 0), (6, 6, 0), (6, 11, 0)]

        expected_result = [(0, 0, 0), (0, 1, 0)]
        result = myFlaeche.convert_trajectroy_points_to_bypassed_cells(
            trajectory)
        self.assertEqual(expected_result, result)

        #########################
        # test with graphical output

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=60, ydim=60, scale=10,
                                 output=image_name)

        # manualy reduced trajectory from:
        # trajectory = get_trajectory()

        trajectory = [

            #### (0, 0, 0)
            (5.0, 5.0, 0.0),
            (9.046732879497288, 5.205227064324194, 0.10134169850289655),

            #### (1, 0, 0)
            (10.053543796885169, 5.320512917970279, 0.1266771231286207),

            #### (1, 0, 1)
            (12.056790650312337, 5.627399048098636, 0.17734797238006897),
            (19.837984034790708, 7.853880017109511, 0.3800313693858621),

            #### (2, 0, 1)
            (20.774234204532743, 8.241687535190776, 0.4053667940115862),

            #### (2, 1, 1)
            (24.412078501243244, 10.026135354216716, 0.5067084925144828),
            (27.011708609495656, 11.601127502703704, 0.5827147663916552),

            #### (2, 1, 2)
            (27.850728603791694, 12.169462351708948, 0.6080501910173793),
            (28.67508187148069, 12.758869461839799, 0.6333856156431035),
            (29.484239301906513, 13.36897052249206, 0.6587210402688276),

            #### (3, 1, 2)
            (30.27768153785216, 13.99937394066165, 0.6840564648945517),
            (32.558676763027464, 16.0082885108352, 0.7600627387717241),
            (35.35032490771164, 18.945100691111108, 0.8614044372746207),

            #### (3, 2, 2)
            (36.63102947750794, 20.515760698093484, 0.912075286526069),

            #### (3, 2, 3)
            (39.47088002974199, 24.708168392795713, 1.0387524096546898),
            (39.97386464578329, 25.587921498756767, 1.064087834280414),

            #### (4, 2, 3)
            (40.45440130753634, 26.480134235795184, 1.0894232589061383),


            #### (4, 3, 3)
            (43.723084754648184, 34.97389870965123, 1.3174420805376565),

            #### (4, 3, 4)
            (44.53873297312447, 38.94288889981701, 1.4187837790405535),


            #### (4, 4, 4)
            (44.98716301185723, 43.98669142747258, 1.5454609021691748)
        ]

        expected_result = [(0, 0, 0),
                           (1, 0, 0), (1, 0, 1),
                           (2, 0, 1), (2, 1, 1), (2, 1, 2),
                           (3, 1, 2), (3, 2, 2), (3, 2, 3),
                           (4, 2, 3), (4, 3, 3), (4, 3, 4), (4, 4, 4)]

        result = myFlaeche.convert_trajectroy_points_to_bypassed_cells(
            trajectory)

        self.assertEqual(expected_result, result)

        if visual:
            myFlaeche.vis_reset()
            for pp in trajectory:
                myFlaeche.vis_add_single_point((pp[0], pp[1]), 'darkseagreen')
            myFlaeche.vis_add_red(result)
            myFlaeche.vis_show()


#    @unittest.skipIf(finished, 'done')
    def test_nodeData(self):
        """test to vertify if a node is inside a Flaeche"""

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=1)
        self.assertEqual(
            main.NodeDataHandler(myFlaeche.cluster[1][1]).center_x, 1.5)
        self.assertEqual(
            main.NodeDataHandler(myFlaeche.cluster[1][1]).center_y, 1.5)

        self.assertEqual(
            main.NodeDataHandler(myFlaeche.cluster[0][0]).center_x, 0.5)
        self.assertEqual(
            main.NodeDataHandler(myFlaeche.cluster[0][0]).center_y, 0.5)

    @unittest.skipIf(finished, 'done')
    def test_convert_cell_id_to_tuple(self):
        """convering cell id strings to tuples"""

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=1)
        self.assertEqual(
            (12, 12, 5), myFlaeche.convert_cell_id_to_tuple("12_12_5"))

    @unittest.skipIf(finished, 'done')
    def test_all_nodes_in_list_are_valid(self):
        """check if all nodes in a list are valid"""

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=10)
        #self.assertEqual((12,12,5), myFlaeche.convert_cell_id_to_tuple("12_12_5"))

        node_list = [(5, 5, 0), (5, 6, 0)]
        self.assertTrue(myFlaeche.all_nodes_in_list_are_valid(node_list))

        node_list = [(5, 5), (5,  6)]
        self.assertTrue(myFlaeche.all_nodes_in_list_are_valid(node_list))

        node_list = [(15, 5), (5,  6)]
        self.assertFalse(myFlaeche.all_nodes_in_list_are_valid(node_list))

        node_list = [(5, 5), (5, 16)]
        self.assertFalse(myFlaeche.all_nodes_in_list_are_valid(node_list))

        node_list = [(5, 5), (-5, 16)]
        self.assertFalse(myFlaeche.all_nodes_in_list_are_valid(node_list))

    @unittest.skipIf(devel_run, 'done')
    def test_all_nodes_in_list_are_valid_and_not_blocked(self):
        """check if all nodes in a list are valid"""

        myFlaeche = main.Flaeche(xdim=100, ydim=100, scale=10)
        blocked_nodes = [(5, 9)]
        myFlaeche.load_node_data(blocked_nodes)


#        node_list = [(5, 5, 0), (5, 6, 0)]
#        self.assertFalse(myFlaeche.all_nodes_in_list_are_valid(node_list))

        node_list = [(5, 5), (5,  6)]
        self.assertTrue(
            myFlaeche.all_nodes_in_list_are_valid_and_not_blocked(node_list))

        node_list = [(5, 5), (5,  6), (5, 12)]
        self.assertFalse(
            myFlaeche.all_nodes_in_list_are_valid_and_not_blocked(node_list))

        node_list = [(5, 5), (5,  6), (5, 9)]
        self.assertFalse(
            myFlaeche.all_nodes_in_list_are_valid_and_not_blocked(node_list))
