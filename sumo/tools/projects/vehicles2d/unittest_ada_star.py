# -*- coding: utf-8 -*-
"""
@file    unittest_ada_star.py
@author  Marek Heinrich
@date    2015-02-24
@version $Id$

Test module for the standard path finding module.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


import sys
import unittest
import numpy
import math
import main


from commons import ANList, StarNodeC, reduce_angle_to_get_smalles_postive_equivialent
# StarNode,  DNList,

from unittest_constants import VISUAL, lost, finished, afinished, unfinished, devel_skip, devel_run, broken, visual, known_bug, debugging


class someTestcase_ada_star(unittest.TestCase):
    ########### adastar #######################################

    @unittest.skipIf(finished, 'done')
    def test_adastar_nodes_id_creation(self):
        """test to check the id string and get_coords of a main.AdAStarNode"""

        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=1)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        myAdA = main.AdAStar(start_node=(0, 0, 0),
                             end_node=(20, 10, 0),
                             vessel=vessel)

        dn_2 = myFlaeche.get_node_data((3, 3))
        mySN = StarNodeC(node_data=dn_2,
                         sector_id=0,
                         reached_by_angle=4 * math.pi,
                         costs_till_here=4,
                         estimated_remaining_costs=5,
                         previous_node=None
                         )

        self.assertEqual('3_3_0',
                         mySN.id)

        self.assertEqual((3, 3, 0),
                         mySN.get_coords())

        self.assertEqual(9.,
                         mySN.full_costs)

        self.assertEqual(0,
                         mySN.reached_by_angle)

    @unittest.skipIf(finished, 'done')
    def test_common_reduce_angle(self):
        """angles should be limited to the range [0, 2*pi["""

        self.assertEqual(0,
                         reduce_angle_to_get_smalles_postive_equivialent(0))

        self.assertEqual(math.pi,
                         reduce_angle_to_get_smalles_postive_equivialent(math.pi))

        self.assertEqual(1.5 * math.pi,
                         reduce_angle_to_get_smalles_postive_equivialent(1.5 * math.pi))

        self.assertEqual(0,
                         reduce_angle_to_get_smalles_postive_equivialent(2. * math.pi))

        self.assertEqual(0.5 * math.pi,
                         reduce_angle_to_get_smalles_postive_equivialent(2.5 * math.pi))

        self.assertEqual(0,
                         reduce_angle_to_get_smalles_postive_equivialent(8 * math.pi))

        self.assertEqual(0,
                         reduce_angle_to_get_smalles_postive_equivialent(-2 * math.pi))

        self.assertEqual(0.2 * math.pi,
                         reduce_angle_to_get_smalles_postive_equivialent(-1.8 * math.pi))

        self.assertEqual(0,
                         reduce_angle_to_get_smalles_postive_equivialent(-6. * math.pi))

    @unittest.skipIf(finished, 'done')
    def test_adastar_nodes_basic_funtionality(self):
        """testing init, get_min,  the home brewn node list """

        """test the ways of initialiation of DNLists"""
        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=1)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        myD = main.AdAStar(start_node=(0, 0, 0),
                           end_node=(20, 10, 0),
                           vessel=vessel)

        myDN_1 = StarNodeC(node_data=myFlaeche.get_node_data((3, 3)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        myDN_2 = StarNodeC(node_data=myFlaeche.get_node_data((4, 3)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        myDN_3 = StarNodeC(node_data=myFlaeche.get_node_data((5, 3)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        myDN_4 = StarNodeC(node_data=myFlaeche.get_node_data((20, 9)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=3,
                           estimated_remaining_costs=0,
                           previous_node=None
                           )

        myDD_1 = ANList([myDN_1, myDN_2, myDN_3])

        """insert a few nodes into the list, and iter over them by id"""
        myANL_id_1 = ANList([myDN_1, myDN_2, myDN_3], 'id')
        self.assertEqual(
            [ii for ii in myANL_id_1], ['3_3_0', '4_3_0', '5_3_0'])

        """alter the order of the few nodes , and iter over them by id"""
        myANL_id_2 = ANList([myDN_3, myDN_2, myDN_1], 'id')
        self.assertEqual(
            [ii for ii in myANL_id_2], ['5_3_0', '4_3_0', '3_3_0'])

        """insert a few nodes into the list, and iter over them by tuple"""
        myANL_tuple = ANList([myDN_1, myDN_2, myDN_3], 'tuple')
        self.assertEqual(
            [ii for ii in myANL_tuple], [(3, 3, 0), (4, 3, 0), (5, 3, 0)])

        """return a node by its id, if not exists return none """
        myANL_ret_id = ANList([myDN_1, myDN_2, myDN_3], 'id')
        self.assertEqual(myANL_ret_id.get_by_id('3_3_0'), myDN_1)
        self.assertIsNone(myANL_ret_id.get_by_id('3_3_1'))

        """return a node by its tuple, if not exists return none """
        myANL_ret_tup = ANList([myDN_1, myDN_2, myDN_3], 'tuple')
        self.assertEqual(myANL_ret_id.get_by_tuple((3, 3, 0)), myDN_1)
        self.assertIsNone(myANL_ret_id.get_by_tuple((3, 3, 1)))

        """return a node by its tuple, even if the DNodeList iters on ids """
        myANL_ret_tup = ANList([myDN_1, myDN_2, myDN_3], 'id')
        self.assertEqual(myANL_ret_id.get_by_tuple((3, 3, 0)), myDN_1)

        """test to get back the tuples of all nodes in the list"""
        myANL_tup_list = ANList([myDN_1, myDN_2, myDN_3])
        self.assertEqual(
            myANL_tup_list.get_tuples(), [(3, 3, 0), (4, 3, 0), (5, 3, 0)])

        """test to retrive the minimal node from List"""
        myANL_get_min = ANList([myDN_1, myDN_2, myDN_3, myDN_4])
        self.assertEqual(myANL_get_min.get_min_node(), myDN_4)

        """test to retrive the minimal node and remove it from the list"""
        myANL_pop_min = ANList([myDN_1, myDN_2, myDN_3, myDN_4])
        self.assertEqual(myANL_pop_min.get_min_node(pop=True), myDN_4)
        self.assertNotEqual(myANL_pop_min.get_min_node(), myDN_4)

        """test to see what happens if the node list is empty"""

        """make sure that only each node id is only once in the list
           should be already managed in the initalization
           keep a 'global'/class list with all nodes
           
           * handling alternation after creation - maybe function_closurures
           * overwriting the append function
           
           """

        """make sure that only nodes from the same AdAStar are added"""
        # currently there is only one AdAStar at a time

        """test to make sure, the list only takes adastar nodes"""
        # this does not work - not too urgent now
        # self.assertRaises(AssertionError, main.AdAStar.ANList, [myDN_1, myDN_2, 1] )

        """insert a few nodes into the list, and iter over them returning 
           the distance traveled"""  # to be done when needed

        """insert a few nodes into the list, and iter over them returning 
           the total distance """   # to be done when needed

    @unittest.skipIf(finished, 'done')
    def test_get_nodes_list(self):
        """test the getter of the nodes lists"""

        start = (10, 10, 0)
        end = (20, 10, 0)
        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=1)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        myD = main.AdAStar(start_node=start, end_node=end, vessel=vessel)
        self.assertEqual(myD.get_open_nodes(), [])

        myDN_1 = StarNodeC(node_data=myFlaeche.get_node_data((3, 3)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        myDN_2 = StarNodeC(node_data=myFlaeche.get_node_data((4, 3)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        myDN_3 = StarNodeC(node_data=myFlaeche.get_node_data((5, 5)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        myDN_4 = StarNodeC(node_data=myFlaeche.get_node_data((5, 6)),
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        """get the nodes tuples"""
        myD.open_nodes_list[0:0] = [myDN_1, myDN_2]
        self.assertEqual(
            sorted(myD.get_open_nodes('tuples')), sorted([(3, 3, 0), (4, 3, 0)]))

        """get the node them self """
        self.assertEqual(
            sorted(myD.get_open_nodes()), sorted([myDN_1, myDN_2]))

        """get the node tuples of closed list """
        myD.closed_nodes_list[0:0] = [myDN_3, myDN_4]
        self.assertEqual(
            sorted(myD.get_closed_nodes('tuples')), sorted([(5, 5, 0), (5, 6, 0)]))

    @unittest.skipIf(finished, 'done')
    def test_get_distance_to_end(self):
        """ttel the distance heurisitics from the point to dest."""

        start = (10, 10, 0)
        end = (20, 10, 0)
        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=1)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        myD = main.AdAStar(start_node=start, end_node=end, vessel=vessel)

        self.assertEqual(5,
                         myD.get_distance_to_end((15, 10, 0)))

        # reach the end
        self.assertEqual(0,
                         myD.get_distance_to_end((20, 10, 0)))

        # negaive root
        self.assertEqual(10,
                         myD.get_distance_to_end((20, 20, 0)))

        # illegal point
        self.assertRaises(StandardError,
                          myD.get_distance_to_end, (40, 40))

        # illegal point
        self.assertRaises(StandardError,
                          myD.get_distance_to_end, (40, 40, 0))

    @unittest.skipIf(finished, 'done')
    def test_get_distance_between(self):
        """test the correct disstance between two points"""

# scales = [1, 0.25] # 0.25 is to slow
        # compare speed with c++
        scales = [1, 2]

        for scale in scales:
            start = (10, 10, 0)
            end = (20, 10, 0)
            myFlaeche = main.Flaeche(xdim=60, ydim=60, scale=scale)

            vessel = main.Vessel(myFlaeche,
                                 [(0, 0), (40, 40), (40, 200), (-40, 200),
                                     (-40, 40), (0, 0)])

            myD = main.AdAStar(start_node=start, end_node=end, vessel=vessel)

            # points are equal
            self.assertEqual(0,
                             myD.get_distance_between_points((2, 2, 0), (2, 2, 0)))

            self.assertEqual(0,
                             myD.get_distance_between_points((2, 2, 0), (2, 2, 2)))

            # make sure that for ADA are allway 3-tuples used
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (2, 2), (2, 2))

        # needs more implementation
        # what to do if i want to know how far i am form my own point,
        # but under a different angle

            self.assertEqual(0,
                             myD.get_distance_between_points((2, 2, 0), (2, 2, 2)))

            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (-3, -3, 0), (10, 10, 0))

            # point a outside
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (-3,  -3, 0), (10, 10, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (-3,   8, 0), (10, 10, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (8,  -3, 0), (10, 10, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (8, 400, 0), (10, 10, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (400,   8, 0), (10, 10, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (400, 400, 0), (10, 10, 0))

            # point b outside
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (10, 10, 0), (-3,  -3, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (10, 10, 0), (-3,   8, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (10, 10, 0), (8,  -3, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (10, 10, 0), (8, 400, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (10, 10, 0), (400,   8, 0))
            self.assertRaises(StandardError,
                              myD.get_distance_between_points, (10, 10, 0), (400, 400, 0))

            # both points outside - check if both are different illegal values and check
            # that even if twice the same illegal point is submitted it will
            # not be accepted
            illegal_points = [(-3, -3, 0), (-3,   8, 0),
                              (8, -3, 0), (400, 400, 0),
                              (400,  8, 0), (8, 400, 0)]

            for ilpt_1 in illegal_points:
                for ilpt_2 in illegal_points:
                    self.assertRaises(StandardError,
                                      myD.get_distance_between_points, ilpt_1, ilpt_2)

            # both points are next to each other / all around
            self.assertEqual(myD.get_distance_between_points((10, 10, 0), (10,  9, 0)),
                             myFlaeche.scale)

            self.assertEqual(myFlaeche.scale,
                             myD.get_distance_between_points((10, 10, 0), (10, 11, 0)))

            self.assertEqual(myFlaeche.scale,
                             myD.get_distance_between_points((10, 10, 0), (9, 10, 0)))

            self.assertEqual(myFlaeche.scale,
                             myD.get_distance_between_points((10, 10, 0), (11, 10, 0)))

            self.assertAlmostEqual(myFlaeche.scale * 1.41421356,
                                   myD.get_distance_between_points((10, 10, 0), (9,  9, 0)))

            self.assertAlmostEqual(myFlaeche.scale * 1.41421356,
                                   myD.get_distance_between_points((10, 10, 0), (9, 11, 0)))

            self.assertAlmostEqual(myFlaeche.scale * 1.41421356,
                                   myD.get_distance_between_points((10, 10, 0), (11,  9, 0)))

            self.assertAlmostEqual(myFlaeche.scale * 1.41421356,
                                   myD.get_distance_between_points((10, 10, 0), (11, 11, 0)))

            # 2 points a bit off
            self.assertAlmostEqual(10 * scale,
                                   myD.get_distance_between_points((10, 10, 0), (20, 10, 0)))

            # 121 + 25 = 145  appox 12 * 12
            self.assertAlmostEqual(12 * scale,
                                   myD.get_distance_between_points(
                                       (10, 10, 0), (21, 15, 0)),
                                   0)
# return    # is visual

    @unittest.skipIf(finished, 'done')
    def test_ada_step_internals(self):
        """ Test for adastar's results after the first step        """

        """test if the start point is added to the open node list"""

        myFlaeche = main.Flaeche(
            xdim=500, ydim=500, scale=10, output='result_ada_star_internals')
        # vessel = main.Vessel(myFlaeche,
        #                     [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        vessel.r = 20
        vessel.speed = 10  # m/s
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

        start = (30, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        # do the first step and add the strat point to the open node list

        myD_step_zero = main.AdAStar(
            vessel=vessel, start_node=start, end_node=end)
        myD_step_zero.step()

        self.assertEqual([(30, 10, 0)],
                         myD_step_zero.get_open_nodes('tuples'))

        self.assertEqual([],
                         myD_step_zero.get_closed_nodes('tuples'))

        # now, do the second step and check the lists again
        expected_result_open_list = [
            (30, 6, 8), (30, 14, 8),
            (31, 6, 9), (31, 10, 0), (31, 14, 7),
            (32, 6, 10), (32, 7, 11), (32, 8, 12), (32, 9, 14),
            (32, 10, 0), (32, 11, 2), (32, 12, 4), (32, 13, 5), (32, 14, 6)]

        expected_result_closed_list = [(30, 10, 0)]

        myD_step_zero.step(visual=True)
        result_open_list = myD_step_zero.get_open_nodes('tuples')
        result_closed_list = myD_step_zero.get_closed_nodes('tuples')

        self.assertEqual(sorted(expected_result_open_list),
                         sorted(result_open_list))

        self.assertEqual(sorted(expected_result_closed_list),
                         sorted(result_closed_list))

        if visual:
            myFlaeche.draw_course_ghost_ship(vessel,
                                             myD_step_zero.current_node_copy.x_coord,
                                             myD_step_zero.current_node_copy.y_coord,
                                             myD_step_zero.current_node_copy.sector_id,
                                             vessel.r, 45)
            myFlaeche.vis_show()

        """test that nodes in the closed nodes list are omitted"""
        myD_fake_closed = main.AdAStar(
            vessel=vessel, start_node=start, end_node=end)

        # make a first step
        myD_fake_closed.step()

        # this is the second one to choose
        dn_2d = myFlaeche.get_node_data((31, 10))
        myDN_2 = StarNodeC(node_data=dn_2d,
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=5,
                           previous_node=None
                           )

        myD_fake_closed.closed_nodes_list.append(myDN_2)

        # should find myDN_2 as suspicious node
        # and reject as it already is in closed node list

        myD_fake_closed.step()
        self.assertTrue(
            (31, 10, 0) not in myD_fake_closed.get_open_nodes('tuples'))

    @unittest.skipIf(finished, '')
    def test_ada_step_internals_update_open_list(self):
        """ Test for adastar's results after the first step        """

        """ test if the start point is added to the open node list"""

        myFlaeche = main.Flaeche(
            xdim=500, ydim=500, scale=10, output='result_ada_star')
#        vessel = main.Vessel(myFlaeche,
#                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        vessel.r = 20
        vessel.speed = 10  # m/s
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

        start = (30, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD_fake_open = main.AdAStar(
            vessel=vessel, start_node=start, end_node=end)

        # make a first step
        myD_fake_open.step()

        # put a fake dummy of a node which will be found into the open list
        # put bad data for the dummy so the real one will be better and
        # the node data has to be  updated

        test_node_tuple = (31, 10, 0)

        dn_2d = myFlaeche.get_node_data(
            (test_node_tuple[0], test_node_tuple[1]))
        myDN_2 = StarNodeC(node_data=dn_2d,
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=4,
                           estimated_remaining_costs=180,
                           previous_node=None
                           )

        myD_fake_open.open_nodes_list.append(myDN_2)

        # make sure that the fake one is realy in the open list
        self.assertTrue((31, 10, 0) in myD_fake_open.get_open_nodes('tuples'))

        faked_node_data = ANList(
            myD_fake_open.open_nodes_list).get_by_tuple(test_node_tuple)
        self.assertEqual(faked_node_data.full_costs, 184)

        # make a step
        myD_fake_open.step()

        # make sure the node data has been updated
        self.assertTrue((31, 10, 0) in myD_fake_open.get_open_nodes('tuples'))
        faked_node_data = ANList(
            myD_fake_open.open_nodes_list).get_by_tuple(test_node_tuple)
        self.assertEqual(faked_node_data.full_costs, 10)

    @unittest.skipIf(finished, 'done')
    def test_dijsktra_step_internals_find_final(self):
        """ Test for adastar's results after the first step        """

        """test that the algo stops if the end is reached - manipulated lists"""

        myFlaeche = main.Flaeche(
            xdim=500, ydim=500, scale=10, output='result_ada_star')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])
        vessel.r = 20
        vessel.speed = 10

        start = (30, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD_fake_open = main.AdAStar(
            vessel=vessel, start_node=start, end_node=end)

        myD_fake_open.step()
        myD_fake_open.step()

        test_node_tuple = (40, 10, 0)

        dn_2d = myFlaeche.get_node_data(
            (test_node_tuple[0], test_node_tuple[1]))
        myDN_2 = StarNodeC(node_data=dn_2d,
                           sector_id=0,
                           reached_by_angle=0,
                           costs_till_here=1,
                           estimated_remaining_costs=1,
                           previous_node=None
                           )

        myD_fake_open.open_nodes_list.append(myDN_2)
        self.assertTrue(myD_fake_open.step())

    @unittest.skipIf(finished, 'done')
    def test_dijsktra_run_and_rebuild(self):
        """run the algorithm on a simple example - straight from left to right"""
        visual = VISUAL
        visual = True

        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_ada_star_full_run_straight')
        vessel = main.Vessel(myFlaeche,
                             # wrong orientation of the vessel shape
                             #[(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
                             [(0, 0), (-40, 40), (-140, 40), (-140, -40), (-40, -40), (0, 0)])
        vessel.r = 20
        vessel.speed = 10
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

        start = (30, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()
        myD.patch_path()

        path = ANList(myD.path, 'tuples').get_tuples()
        self.assertEqual(path, [
            (30, 10, 0), (31, 10, 0), (32, 10, 0), (33, 10, 0),
            (34, 10, 0), (35, 10, 0), (36, 10, 0), (37, 10, 0),
            (38, 10, 0), (39, 10, 0), (40, 10, 0)
        ])

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(finished, 'done')
    def test_dijsktra_step_internals_rebuild_error(self):
        myFlaeche = main.Flaeche(
            xdim=500, ydim=500, scale=10, output='result_ada_star')
        vessel = main.Vessel(myFlaeche,
                             # wrong orientation of the vessel shape
                             #[(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
                             [(0, 0), (-40, 40), (-140, 40), (-140, -40), (-40, -40), (0, 0)])
        vessel.r = 20

        myD = main.AdAStar((0, 0, 0), (10, 10, 0), vessel=vessel)
        self.assertRaisesRegexp(StandardError, "algorithm must be run first successfully",
                                myD.rebuild_path)

    @unittest.skipIf(broken, '')
    def test_ada_run_and_rebuild_hindrance_at_nodes_behind_start(self):
        """start node is blocked"""
        visual = VISUAL
        visual = True

        image_name = self.__dict__['_testMethodName']

        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        vessel.r = 20
        vessel.speed = 10
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

        blocked_nodes = [(10, 10), (11, 10), (12, 10), (13, 10), (14, 10)]
        myFlaeche.load_node_data(blocked_nodes)

        start = (10, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()

        path = ANList(myD.path, 'tuples').get_tuples()

        assert(False)

#        self.assertEqual(path, [
#            (30, 10, 0), (31, 10, 0), (32, 10, 0), (33, 10, 0),
#            (34, 10, 0), (35, 10, 0), (36, 10, 0), (37, 10, 0),
#            (38, 10, 0), (39, 10, 0), (40, 10, 0)
#        ])

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)


#    @unittest.skipIf(broken, '')
    @unittest.skipIf(devel_run, '')
    def test_dijsktra_step_internals_find_final__bug_a_s(self):
        """ unclear bug

        the result of this test is odd.
        when looking at the graphical output,
        it seem like the vehilce starts and ends in
        the wrong positions.
        """

        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_ada_non_straight')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 8)  # cell coordinates
        end = (16, 19, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        if False:

            myD.step()
            open_list = ANList(myD.open_nodes_list, 'tuples').get_tuples()
            print open_list

            myD.step()
            open_list = ANList(myD.open_nodes_list, 'tuples').get_tuples()
            print open_list

            print
            print

            myD.step()
            open_list = ANList(myD.open_nodes_list, 'tuples').get_tuples()
            print open_list

            print
            print

            myD.step()
            open_list = ANList(myD.open_nodes_list, 'tuples').get_tuples()
            print open_list

            print
            print

            myD.step()
            open_list = ANList(myD.open_nodes_list, 'tuples').get_tuples()
            print open_list

        myD.run(verbose=True, visual=True)
        myD.rebuild_path()
#        myD.patch_path()

        for ii in myD.path:
            print ii.id

        if visual:
            myD.draw_path(vessel=vessel)
#            myD.flaeche.vis_show()

        del(myD)
