# -*- coding: utf-8 -*-
"""
@file    unittest_vessel.py
@author  Marek Heinrich
@date    2015-02-24
@version $Id$

Test module for vessels functions.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


import unittest
from sys import exit
import math
import numpy
import main

from unittest_constants import VISUAL, lost, finished, afinished, unfinished, devel_skip, devel_run, broken, visual, known_bug, debugging

from unittest_utils import skip_all_but_selected, assertAlmostEqualTupleList


class testCaseVessel(unittest.TestCase):

    @unittest.skipIf(finished, 'done')
    def test_vessel_tranform_points(self):
        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10)
        vessel_shape = [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), (0, 0)]
        vessel = main.Vessel(myFlaeche, vessel_shape)

        # transform points in global coord to ego coord
        # and back to global coords

        #####################
        # ego sits on 10,10 has no inclination and want's to know where in
        # global coordinates is 0, 0 and vice versa

        offset = (10, 10)
        omega = 0
        ego_point = (0, 0)
        global_point = (10, 10)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        ######################
        # ego sits on 9,9 has no inclination and want's to know where in
        # global coordinates is 1, 1 and vice versa

        offset = (9, 9)
        omega = 0
        ego_point = (1, 1)
        global_point = (10, 10)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        #########################
        # ego sits on 1, 1  has an incliantion of 60 degrees
        # and want's to know where in global coordinates is 2, 0

        offset = (1, 1)
        omega = math.pi / 3
        ego_point = (2, 0)
        global_point = (2, 1 + 2 * 0.866025403784)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        ####################
        # ego sits on 0, 0 has an incliantion of 90 degrees
        # and want's to know where in global coordinates is ego(0, 1)

        offset = (0, 0)
        omega = math.pi / 2
        ego_point = (1, 0)
        global_point = (0, 1)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        #####################
        # ego sits on 0, 0 has an incliantion of 45 degrees
        # and want's to know where in global coordinates is ego(1.4142..., 0)
        # and vice versa

        offset = (0, 0)
        omega = math.pi / 4
        ego_point = (math.sqrt(2), 0)
        global_point = (1, 1)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        #####################
        # ego sits on 1, 1 has an incliantion of 45 degrees
        # and want's to know where in global coordinates is ego(1.4142..., 0)
        # and vice versa

        offset = (1, 1)
        omega = math.pi / 4
        ego_point = (math.sqrt(2), 0)
        global_point = (2, 2)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

    @unittest.skipIf(finished, 'done')
    def test_hull(self):

        vessel_shape = [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), (0, 0)]

        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=1)
        vessel = main.Vessel(myFlaeche, vessel_shape)

        # segments

        self.assertEqual(vessel.get_num_hull_segments(), 5)
        self.assertEqual(vessel.get_hull_segment(0), ((0, 0), (1, 1)))

        self.assertRaisesRegexp(AssertionError, "hull segment out of range",
                                vessel.get_hull_segment, 60)

        self.assertEqual(vessel.get_hull_segments(),
                         [((0, 0),  (1, 1)),
                          ((1, 1),  (1, 3)),
                          ((1, 3),  (-1, 3)),
                          ((-1, 3), (-1, 1)),
                          ((-1, 1), (0, 0))])

        # inclination

        self.assertRaisesRegexp(StandardError, "points are identical",
                                vessel.get_inclination, (0, 0), (0, 0))
        self.assertRaisesRegexp(StandardError, "points are identical",
                                vessel.get_inclination, (1.0, 0), (1, 0))
        self.assertEqual(vessel.get_inclination((0, 0), (1, 1)), 1)
        self.assertEqual(vessel.get_inclination((0, 0), (1, -1)), -1)
        self.assertEqual(vessel.get_inclination((0, 0), (-1, -1)), 1)
        self.assertEqual(vessel.get_inclination((0, 0), (1, 2)), 2)
        self.assertEqual(vessel.get_inclination((1, 1), (2, 3)), 2)
        self.assertEqual(vessel.get_inclination((1, 1), (3, 4)), 1.5)
        self.assertEqual(vessel.get_inclination((5, 2), (1, 1)), 0.25)
        self.assertEqual(
            vessel.get_inclination((1, 1), (1, 2)), numpy.float('inf'))
        self.assertEqual(
            vessel.get_inclination((1, 2), (1, 1)), numpy.float('-inf'))

    @unittest.skipIf(finished, 'done')
    def test_vessel_tranform_hull(self):
        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10)
        vessel_shape = [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), (0, 0)]
        vessel = main.Vessel(myFlaeche, vessel_shape)

        ##################
        # get transformed the coords of the vehicle
        expected_result = [(15, 10), (16.0, 9.0), (18.0, 9.0),
                           (18.0, 11.0), (16.0, 11.0), (15, 10)]
        result = vessel.transform_hull_points(-math.pi / 2, (15, 10))
        self.assertEqual(expected_result, result)

        expected_result = [(15, 10), (15.0, 11.414213562373096),
                           (13.585786437626904, 12.82842712474619),
                           (12.17157287525381, 11.414213562373096),
                           (13.585786437626904, 10.0), (15, 10)]

        [(15, 10), (16.0, 9.0), (18.0, 9.0),
         (18.0, 11.0), (16.0, 11.0), (15, 10)]
        result = vessel.transform_hull_points(math.pi / 4, (15, 10))
        self.assertEqual(expected_result, result)

        ##################
        # check if shape can reach into negatives

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        expected_result = [(5, 5),
                           (-5, 15), (-25, 15), (-25, -5),
                           (-5, -5), (5, 5)]
        result = vessel.transform_hull_points(0, (5, 5))
        assertAlmostEqualTupleList(self, expected_result, result)

    @unittest.skipIf(finished, 'done')
    def test_vessel_intersection_points(self):
        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=1)
        vessel_shape = [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), (0, 0)]
        vessel = main.Vessel(myFlaeche, vessel_shape)

        # intersects

        self.assertEqual(vessel.get_intersection_points((4.5, 4.5), (8.2, 8.2)),
                         ([(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)],
                          [(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)]
                          )
                         )

        self.assertEqual(vessel.get_intersection_points((5, 5), (8, 8)),
                         ([(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)],
                          [(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)]
                          )
                         )

        result = vessel.get_intersection_points((4.5, 3.5), (8.2, 7.2))[0]
        expected_result = [(5.0, 4.0), (6.0, 5.0), (7.0, 6.0), (8.0, 7.0)]
        assertAlmostEqualTupleList(self, result, expected_result)

        self.assertEqual(vessel.get_intersection_points((4.5, 5), (8.2, 5)),
                         ([(5.0, 5.0), (6.0, 5.0), (7.0, 5.0), (8.0, 5.0)], []))

        self.assertEqual(vessel.get_intersection_points((4.5, 5), (4.5, 7.1)),
                         ([], [(4.5, 5), (4.5, 6), (4.5, 7)]))

        self.assertEqual(vessel.get_intersection_points((4.5, 7.1), (4.5, 5)),
                         ([], [(4.5, 5), (4.5, 6.0), (4.5, 7.0)]))

        myFlaeche = main.Flaeche(xdim=50, ydim=50, scale=0.5)
        vessel = main.Vessel(myFlaeche, vessel_shape)

        self.assertEqual(vessel.get_intersection_points((4.6, 4.6), (6.2, 6.2)),
                         ([(5.0, 5.0), (5.5, 5.5), (6.0, 6.0)],
                          [(5.0, 5.0), (5.5, 5.5), (6.0, 6.0)]
                          )
                         )

        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10)
        vessel = main.Vessel(myFlaeche, vessel_shape)

        self.assertEqual(vessel.get_intersection_points((140.0, 285.0),
                                                        (120, 285.0)),
                         ([(120.0, 285.0), (130.0, 285.0), (140.0, 285.0)],
                          []
                          )
                         )

        self.assertEqual(vessel.get_intersection_points((60.0, 285.0),
                                                        (60.0, 255.0)),
                         ([],
                          [(60.0, 260.0), (60.0, 270.0), (60.0, 280.0)],
                          )
                         )

        self.assertEqual(vessel.get_intersection_points((59.999999999999986, 285.0),
                                                        (60.0, 125.0)),
                         ([],
                          [(59.999999999999986, 200.0),
                           (59.999999999999986, 210.0),
                           (59.999999999999986, 220.0),
                           (59.999999999999986, 230.0),
                           (59.999999999999986, 240.0),
                           (59.999999999999986, 250.0),
                           (59.999999999999986, 260.0),
                           (59.999999999999986, 270.0),
                           (59.999999999999986, 280.0),
                           (60.0, 130.0),
                           (60.0, 140.0),
                           (60.0, 150.0),
                           (60.0, 160.0),
                           (60.0, 170.0),
                           (60.0, 180.0),
                           (60.0, 190.0)])
                         )

        ###########
        # check negative ones

        ################
        # hull_seg ((5, 5), (-5.0, 15.000000000000002))

        segment_start_negative = (5, 5)
        segment_end_negative = (-5.0, 15.000000000000002)
        segment_end_negative = (-5.0, 15)

        result_negative = vessel.get_intersection_points(segment_start_negative,
                                                         segment_end_negative)

        result_negative_expected = ([], [(0, 10)])
        self.assertEqual(result_negative_expected, result_negative)

        ################
        # hull_seg ((-5.0, 15.000000000000002), (-25.0, 15.000000000000002))

        segment_start_negative = (-5, 15)
        segment_end_negative = (-25.0, 15)

        result_negative = vessel.get_intersection_points(segment_start_negative,
                                                         segment_end_negative)

        result_negative_expected = ([(-20, 15), (-10, 15)], [])
        self.assertEqual(result_negative_expected, result_negative)

        ################
        # hull_seg ((-25.0, 15.000000000000002), (-25.0, -5.000000000000002))

        segment_start_negative = (-25, 15)
        segment_end_negative = (-25.0, -5)

        result_negative = vessel.get_intersection_points(segment_start_negative,
                                                         segment_end_negative)

        result_negative_expected = ([], [(-25, 0), (-25, 10)])
        self.assertEqual(result_negative_expected, result_negative)

        ################
        # hull_seg ((-25.0, -5.000000000000002), (-5.0, -5.000000000000002))

        segment_start_negative = (-25, -5)
        segment_end_negative = (-5, -5)

        result_negative = vessel.get_intersection_points(segment_start_negative,
                                                         segment_end_negative)

        result_negative_expected = ([(-20, -5), (-10, -5)], [])
        self.assertEqual(result_negative_expected, result_negative)

        ################
        # hull_seg ((-5.0, -5.000000000000002), (5, 5))

        segment_start_negative = (-5, -5)
        segment_end_negative = (5,  5)

        result_negative = vessel.get_intersection_points(segment_start_negative,
                                                         segment_end_negative)

        result_negative_expected = ([(0, 0)], [(0, 0)])
        self.assertEqual(result_negative_expected, result_negative)

    @unittest.skipIf(finished, 'done')
    def test_get_grey_and_black_shade(self):
        """testing which (visual) nodes are shadowed by the vehicle"""
        visual = VISUAL
        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10,
                                 output='result_grey_rotation_90_0')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        result = vessel.get_grey_shade(-math.pi / 2, (100, 85))
        self.assertEqual(
            [(9, 8), (10, 8), (10, 7), (11, 6), (11, 7),
             (12, 5), (12, 6), (13, 4), (13, 5), (14, 4),
                (15, 4), (16, 4), (17, 4), (18, 4), (19, 4),
                (20, 4), (21, 4), (22, 4), (23, 4), (24, 4),
                (25, 4), (26, 4), (27, 4), (28, 4), (29, 4),
                (30, 4), (29, 5), (30, 5), (29, 6), (30, 6),
                (29, 7), (30, 7), (29, 8), (30, 8), (29, 9),
                (30, 9), (29, 10), (30, 10), (29, 11), (30, 11),
                (29, 12), (30, 12), (13, 12), (14, 12), (15, 12),
                (16, 12), (17, 12), (18, 12), (19, 12), (20, 12),
                (21, 12), (22, 12), (23, 12), (24, 12), (25, 12),
                (26, 12), (27, 12), (28, 12), (10, 9), (11, 9),
                (11, 10), (12, 10), (12, 11), (13, 11)],
            result)

        if visual:
            myFlaeche.vis_reset()
            myFlaeche.vis_add_grey(result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

        # 45°
        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10,
                                 output='result_grey_rotation_45_0')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])
        vessel.transform_hull_points(-math.pi / 4, (100, 85))

        result = vessel.get_grey_shade(-math.pi / 4, (100, 85))
        self.assertEqual(
            [(9, 8), (10, 8), (11, 8), (12, 8), (13, 8), (14, 8),
             (15, 8), (16, 8), (16, 9), (17, 9), (17, 10), (18, 10),
                (18, 11), (19, 11), (19, 12), (20, 12), (20, 13), (21, 13),
                (21, 14), (22, 14), (22, 15), (23, 15), (23, 16), (24, 16),
                (24, 17), (25, 17), (25, 18), (26, 18), (26, 19), (21, 24),
                (21, 25), (22, 23), (22, 24), (23, 22), (23, 23), (24, 21),
                (24, 22), (25, 20), (25, 21), (26, 20), (9, 14), (10, 14),
                (10, 15), (11, 15), (11, 16), (12, 16), (12, 17), (13, 17),
                (13, 18), (14, 18), (14, 19), (15, 19), (15, 20), (16, 20),
                (16, 21), (17, 21), (17, 22), (18, 22), (18, 23), (19, 23),
                (19, 24), (20, 24), (20, 25), (9,
                                               9), (10, 9), (9, 10), (10, 10),
                (9, 11), (10, 11), (9, 12), (10, 12), (9, 13), (10, 13)],
            result)

        if visual:
            myFlaeche.vis_reset()
            myFlaeche.vis_add_grey(result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

        # 0°

        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10,
                                 output='result_grey_rotation_0_0')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])
        vessel.transform_hull_points(0, (100, 85))

        result = vessel.get_grey_shade(0, (100, 85))
        self.assertEqual(
            [(9, 8), (10, 8), (10, 9), (11, 9), (11, 10), (12, 10),
             (12, 11), (13, 11), (13, 12), (14, 12), (13, 13), (14, 13),
                (13, 14), (14, 14), (13, 15), (14, 15), (13, 16), (14, 16),
                (13, 17), (14, 17), (13, 18), (14, 18), (13, 19), (14, 19),
                (13, 20), (14, 20), (13, 21), (14, 21), (13, 22), (14, 22),
                (13, 23), (14, 23), (13, 24), (14, 24), (13, 25), (14, 25),
                (13, 26), (14, 26), (13, 27), (14, 27), (13, 28), (14, 28),
                (5, 28), (6, 28), (7, 28), (8,
                                            28), (9, 28), (10, 28), (11, 28),
                (12, 28), (5, 19), (5, 20), (5, 21), (5, 22), (5, 23), (5, 24),
                (5, 25), (5, 26), (5, 27), (5, 12), (5, 13), (6, 12), (6, 13),
                (5, 14), (6, 14), (5, 15), (6, 15), (5, 16), (6, 16), (5, 17),
                (6, 17), (5, 18), (6, 18), (6, 19), (6, 11), (7, 10), (7, 11),
                (8, 9), (8, 10), (9, 9)],
            result)

        if visual:
            myFlaeche.vis_reset()
            myFlaeche.vis_add_grey(result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

        # 90° black
        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10,
                                 output='result_black_rotation_90')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        vessel.transform_hull_points(-math.pi / 2, (100, 85))
        result = vessel.get_black_shade(-math.pi / 2, (100, 85))
        self.assertEqual(
            [(9, 8),
             (10, 7), (10, 8), (10, 9),
                (11, 6), (11, 7), (11, 8), (11, 9), (11, 10),
                (12, 5), (12, 6), (12, 7), (12,
                                            8), (12, 9), (12, 10), (12, 11),
                (13, 4), (13, 5), (13, 6), (13, 7), (13,
                                                     8), (13, 9), (13, 10), (13, 11), (13, 12),
                (14, 4), (14, 5), (14, 6), (14, 7), (14,
                                                     8), (14, 9), (14, 10), (14, 11), (14, 12),
                (15, 4), (15, 5), (15, 6), (15, 7), (15,
                                                     8), (15, 9), (15, 10), (15, 11), (15, 12),
                (16, 4), (16, 5), (16, 6), (16, 7), (16,
                                                     8), (16, 9), (16, 10), (16, 11), (16, 12),
                (17, 4), (17, 5), (17, 6), (17, 7), (17,
                                                     8), (17, 9), (17, 10), (17, 11), (17, 12),
                (18, 4), (18, 5), (18, 6), (18, 7), (18,
                                                     8), (18, 9), (18, 10), (18, 11), (18, 12),
                (19, 4), (19, 5), (19, 6), (19, 7), (19,
                                                     8), (19, 9), (19, 10), (19, 11), (19, 12),
                (20, 4), (20, 5), (20, 6), (20, 7), (20,
                                                     8), (20, 9), (20, 10), (20, 11), (20, 12),
                (21, 4), (21, 5), (21, 6), (21, 7), (21,
                                                     8), (21, 9), (21, 10), (21, 11), (21, 12),
                (22, 4), (22, 5), (22, 6), (22, 7), (22,
                                                     8), (22, 9), (22, 10), (22, 11), (22, 12),
                (23, 4), (23, 5), (23, 6), (23, 7), (23,
                                                     8), (23, 9), (23, 10), (23, 11), (23, 12),
                (24, 4), (24, 5), (24, 6), (24, 7), (24,
                                                     8), (24, 9), (24, 10), (24, 11), (24, 12),
                (25, 4), (25, 5), (25, 6), (25, 7), (25,
                                                     8), (25, 9), (25, 10), (25, 11), (25, 12),
                (26, 4), (26, 5), (26, 6), (26, 7), (26,
                                                     8), (26, 9), (26, 10), (26, 11), (26, 12),
                (27, 4), (27, 5), (27, 6), (27, 7), (27,
                                                     8), (27, 9), (27, 10), (27, 11), (27, 12),
                (28, 4), (28, 5), (28, 6), (28, 7), (28,
                                                     8), (28, 9), (28, 10), (28, 11), (28, 12),
                (29, 4), (29, 5), (29, 6), (29, 7), (29,
                                                     8), (29, 9), (29, 10), (29, 11), (29, 12),
                (30, 4), (30, 5), (30, 6), (30, 7), (30, 8), (30, 9), (30, 10), (30, 11), (30, 12)],
            result)

        # black 45°
        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10,
                                 output='result_black_rotation_45')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        vessel.transform_hull_points(-math.pi / 4, (100, 85))
        result = vessel.get_black_shade(-math.pi / 4, (100, 85))

        self.assertEqual(
            [(9, 8), (9, 9), (9, 10), (9, 11), (9, 12), (9, 13), (9, 14),
             (10, 8), (10, 9), (10, 10), (10, 11), (10,
                                                    12), (10, 13), (10, 14), (10, 15),
                (11, 8), (11, 9), (11, 10), (11, 11), (11,
                                                       12), (11, 13), (11, 14), (11, 15), (11, 16),
                (12, 8), (12, 9), (12, 10), (12, 11), (12, 12), (12, 13),
                (12, 14), (12, 15), (12, 16), (12, 17),
                (13, 8), (13, 9), (13, 10), (13, 11), (13, 12), (13, 13),
                (13, 14), (13, 15), (13, 16), (13, 17), (13, 18),
                (14, 8), (14, 9), (14, 10), (14, 11), (14, 12), (14, 13),
                (14, 14), (14, 15), (14, 16), (14, 17), (14, 18), (14, 19),
                (15, 8), (15, 9), (15, 10), (15, 11), (15, 12), (15, 13),
                (15, 14), (15, 15), (15, 16), (15,
                                               17), (15, 18), (15, 19), (15, 20),
                (16, 8), (16, 9), (16, 10), (16, 11), (16, 12), (16, 13),
                (16, 14), (16, 15), (16, 16), (16, 17), (16,
                                                         18), (16, 19), (16, 20), (16, 21),
                (17, 9), (17, 10), (17, 11), (17, 12), (17, 13), (17, 14),
                (17, 15), (17, 16), (17, 17), (17, 18), (17,
                                                         19), (17, 20), (17, 21), (17, 22),
                (18, 10), (18, 11), (18, 12), (18, 13), (18, 14), (18, 15),
                (18, 16), (18, 17), (18, 18), (18, 19), (18,
                                                         20), (18, 21), (18, 22), (18, 23),
                (19, 11), (19, 12), (19, 13), (19, 14), (19, 15), (19, 16),
                (19, 17), (19, 18), (19, 19), (19, 20), (19,
                                                         21), (19, 22), (19, 23), (19, 24),
                (20, 12), (20, 13), (20, 14), (20, 15), (20, 16), (20, 17),
                (20, 18), (20, 19), (20, 20), (20, 21), (20,
                                                         22), (20, 23), (20, 24), (20, 25),
                (21, 13), (21, 14), (21, 15), (21, 16), (21, 17), (21, 18),
                (21, 19), (21, 20), (21, 21), (21,
                                               22), (21, 23), (21, 24), (21, 25),
                (22, 14), (22, 15), (22, 16), (22, 17), (22, 18), (22, 19),
                (22, 20), (22, 21), (22, 22), (22, 23), (22, 24),
                (23, 15), (23, 16), (23, 17), (23, 18), (23, 19),
                (23, 20), (23, 21), (23, 22), (23, 23), (24, 16),
                (24, 17), (24, 18), (24, 19), (24, 20), (24, 21),
                (24, 22), (25, 17), (25, 18), (25, 19), (25, 20), (25, 21),
                (26, 18), (26, 19), (26, 20)],
            result)

        if visual:
            myFlaeche.vis_reset()
            myFlaeche.vis_add_black(result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

    @unittest.skipIf(finished, 'done')
    def test_get_grey_and_black_shade_at_zero_position(self):

        #############
        # negative postion or position on cell 0_0

        visual = VISUAL

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        sector_to_inspect = 0

        result_grey = vessel.get_grey_shade(myFlaeche.get_angle_from_sector_id(
            sector_to_inspect), (5, 5))
        result_black = vessel.get_black_shade(myFlaeche.get_angle_from_sector_id(
            sector_to_inspect), (5, 5))

        for ii in range(len(result_grey)):
            result_grey[ii] = (
                result_grey[ii][0] + 10, result_grey[ii][1] + 10)

        for ii in range(len(result_black)):
            result_black[ii] = (
                result_black[ii][0] + 10, result_black[ii][1] + 10)

        # move cells so that they are in the middle of the canvas
        expected_result_grey = [(7, 9), (7, 10), (7, 11),
                                (8, 9), (8, 11),
                                (9, 9), (9, 10), (9, 11),
                                (10, 9), (10, 10), (10, 11)]

        expected_result_black = [(7, 9), (7, 10), (7, 11),
                                 (8, 9), (8, 10), (8, 11),
                                 (9, 9), (9, 10), (9, 11),
                                 (10, 9), (10, 10), (10, 11)]

        vessel.transform_hull_points(myFlaeche.get_angle_from_sector_id(sector_to_inspect),
                                     (105, 105))

        self.assertEqual(sorted(expected_result_grey), sorted(result_grey))
        self.assertEqual(sorted(expected_result_black), sorted(result_black))

        if visual:
            myFlaeche.vis_reset()
            myFlaeche.vis_add_black(result_black)
            myFlaeche.vis_show(vessel.transformed_hull_points)

    @unittest.skipIf(finished, 'done')
    def test_get_grey_and_black_shade_small_vessel_sector_0(self):
        """testing which (visual) nodes are shadowed by the vehicle"""
        visual = VISUAL
        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)

        vessel = main.Vessel(myFlaeche,
                             [(0,    0), (-10,  10),
                              (-30,  10), (-30, -10),
                              (-10, -10), (0,   0)])

        sector_to_inspect = 0
        vessel_inclination = myFlaeche.get_angle_from_sector_id(
            sector_to_inspect)

        vessel.transform_hull_points(vessel_inclination,
                                     (105, 105))

        # dublicating test behaviour ??
        self.assertEqual(0, vessel_inclination)

        result_grey = sorted(
            vessel.get_grey_shade(vessel_inclination, (105, 105)))

        expected_result_grey = [(7, 9), (7, 10), (7, 11),
                                (8, 9), (8, 11),
                                (9, 9), (9, 10), (9, 11),
                                (10, 9), (10, 10), (10, 11)]

        self.assertEqual(expected_result_grey, result_grey)

        result_black = sorted(
            vessel.get_black_shade(vessel_inclination, (105, 105)))

        expected_result_black = [(7, 9), (7, 10), (7, 11),
                                 (8, 9), (8, 10), (8, 11),
                                 (9, 9), (9, 10), (9, 11),
                                 (10, 9), (10, 10), (10, 11)]

        self.assertEqual(expected_result_black, result_black)

        if visual:
            myFlaeche.vis_reset()
#           myFlaeche.vis_add_grey(result_grey)
            myFlaeche.vis_add_black(result_black)
            myFlaeche.vis_show(vessel.transformed_hull_points)

        if False:  # True: # print result
            exit(result_grey)

    @unittest.skipIf(finished, 'done')
    def test_get_grey_and_black_shade_small_vessel_sector_1(self):
        """testing which (visual) nodes are shadowed by the vehicle"""
        visual = VISUAL
        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)

        vessel = main.Vessel(myFlaeche,
                             [(0,    0), (-10,  10),
                              (-30,  10), (-30, -10),
                              (-10, -10), (0,   0)])

        sector_to_inspect = 1
        vessel_inclination = myFlaeche.get_angle_from_sector_id(
            sector_to_inspect)

        vessel.transform_hull_points(vessel_inclination,
                                     (105, 105))

        # dublicating test behaviour ??
        self.assertAlmostEqual(0.392699081699, vessel_inclination)

        result_grey = sorted(
            vessel.get_grey_shade(vessel_inclination, (105, 105)))

        expected_result_grey = [(7, 8), (7,  9), (7, 10),
                                (8, 8), (8, 10),
                                (9, 8), (9,  9), (9, 10), (9, 11),
                                (10, 9), (10, 10)]

        self.assertEqual(expected_result_grey, result_grey)

        result_black = sorted(
            vessel.get_black_shade(vessel_inclination, (105, 105)))

        expected_result_black = [(7, 8), (7, 9), (7, 10),
                                 (8, 8), (8, 9), (8, 10),
                                 (9, 8), (9, 9), (9, 10), (9, 11),
                                 (10, 9), (10, 10)]

        self.assertEqual(expected_result_black, result_black)

        if visual:
            myFlaeche.vis_reset()
#           myFlaeche.vis_add_grey(result_grey)
            myFlaeche.vis_add_black(result_black)
            myFlaeche.vis_show(vessel.transformed_hull_points)

        if False:  # True: # print result
            exit(result_grey)

    def test_get_predefiend_shadow_shape(self):
        visual = VISUAL
        visual = True

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        # get the blured shade
        blured_shade_expected = [(-3, -2), (-3, -1), (-3, 0), (-3, 1), (-3, 2),
                                 (-2, -2), (-2, -1), (-2,
                                                      0), (-2, 1), (-2, 2),
                                 (-1, -2), (-1, -1), (-1,
                                                      0), (-1, 1), (-1, 2),
                                 (0, -1), (0,  0), (0, 1)]

        blured_shade_expected = [(7,  8), (7,  9), (7, 10), (7, 11), (7, 12),
                                 (8,  8), (8,  9), (8,
                                                    10), (8, 11), (8, 12),
                                 (9,  8), (9,  9), (9,
                                                    10), (9, 11), (9, 12),
                                 (10,  9),           (10, 10), (10, 11)]

        #####################
        # with string as input
        blured_shade_result = vessel.get_predefiend_shadow_shape_from_cell_id(
            cell_id="10_10_0")

        self.assertEqual(sorted(blured_shade_expected),
                         sorted(blured_shade_result))

        #####################
        # with tuple as input
        blured_shade_result = vessel.get_predefiend_shadow_shape_from_cell_id(
            cell_id=(10, 10, 0))

        self.assertEqual(sorted(blured_shade_expected),
                         sorted(blured_shade_result))

        if visual:

            ###################
            # print the blured/merged shade (grey)
            myFlaeche.output = self.__dict__['_testMethodName'] + '_blured'
            vessel.transform_hull_points(myFlaeche.get_angle_from_sector_id(0),
                                         (105, 105))
            myFlaeche.vis_reset()
            myFlaeche.vis_add_grey(blured_shade_result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

    @unittest.skipIf(finished, 'done')
    def test_get_predefiend_shadow_shape_negatives(self):
        """this test is broken, cause a some cells in
        the black shades are missing .. see bmp output"""

        visual = VISUAL

        verbose = False

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        # get shade of left sector (green)
        left_sector_shade_expected = [(-3,  0), (-3, 1), (-3, 2),
                                      (-2,  0), (-2, 1), (-2, 2),
                                      (-1, -1), (-1, 0), (-1, 1), (-1, 2),
                                      (0,  0), (0, 1)]
        left_sector_shade_result = vessel.get_black_shade(myFlaeche.get_angle_from_sector_id(
            15), (5, 5))

        self.assertEqual(
            sorted(left_sector_shade_expected), sorted(left_sector_shade_result))

        # get shade of right sector (red)
        right_sector_shade_expected = [(-3, -2), (-3, -1), (-3, 0),
                                       (-2, -2), (-2, -1), (-2, 0),
                                       (-1, -2), (-1, -1), (-1, 0),
                                       (-1,  1), (0, -1), (0, 0)]
        right_sector_shade_result = vessel.get_black_shade(
            myFlaeche.get_angle_from_sector_id(1), (5, 5))

        self.assertEqual(
            sorted(right_sector_shade_expected), sorted(right_sector_shade_result))

        # get shade of sector_to_inspect
        sector_to_inspect_shade_expected = [(-3, -1), (-3, 0), (-3, 1),
                                            (-2, -1), (-2, 0), (-2, 1),
                                            (-1, -1), (-1, 0), (-1, 1),
                                            (0, -1), (0, 0), (0, 1)]
        sector_to_inspect_shade_result = vessel.get_black_shade(
            myFlaeche.get_angle_from_sector_id(0), (5, 5))

        self.assertEqual(sorted(sector_to_inspect_shade_expected),
                         sorted(sector_to_inspect_shade_result))

        # get the blured shade
        blured_shade_expected = [(-3, -2), (-3, -1), (-3, 0), (-3, 1), (-3, 2),
                                 (-2, -2), (-2, -1), (-2,
                                                      0), (-2, 1), (-2, 2),
                                 (-1, -2), (-1, -1), (-1,
                                                      0), (-1, 1), (-1, 2),
                                 (0, -1), (0,  0), (0, 1)]

        blured_shade_result = vessel.get_predefiend_shadow_shape_negatives(
            sector_id=0)

        self.assertEqual(sorted(blured_shade_expected),
                         sorted(blured_shade_result))

        # transform and display each seperately
        for result in [left_sector_shade_result,
                       right_sector_shade_result,
                       sector_to_inspect_shade_result,
                       blured_shade_result]:
            for ii in range(len(result)):
                result[ii] = (result[ii][0] + 10, result[ii][1] + 10)

        if visual:

            ###################
            # print the shade of the left sector (green)
            myFlaeche.output = self.__dict__['_testMethodName'] + '_001_left'
            vessel.transform_hull_points(myFlaeche.get_angle_from_sector_id(15),
                                         (105, 105))
            myFlaeche.vis_reset()
            myFlaeche.vis_add_green(left_sector_shade_result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

            ###################
            # print the shade of the right sector (green)
            myFlaeche.output = self.__dict__['_testMethodName'] + '_002_rigth'
            vessel.transform_hull_points(myFlaeche.get_angle_from_sector_id(1),
                                         (105, 105))
            myFlaeche.vis_reset()
            myFlaeche.vis_add_red(right_sector_shade_result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

            ###################
            # print the shade of the sector_to_inspect (grey)
            myFlaeche.output = self.__dict__[
                '_testMethodName'] + '_003_inspect'
            vessel.transform_hull_points(myFlaeche.get_angle_from_sector_id(0),
                                         (105, 105))
            myFlaeche.vis_reset()
            myFlaeche.vis_add_grey(sector_to_inspect_shade_result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

            ###################
            # print the blured/merged shade (grey)
            myFlaeche.output = self.__dict__['_testMethodName'] + '_004_blured'
            vessel.transform_hull_points(myFlaeche.get_angle_from_sector_id(0),
                                         (105, 105))
            myFlaeche.vis_reset()
            myFlaeche.vis_add_grey(blured_shade_result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

    @unittest.skipIf(finished, 'done')
    def test_vehicle_pyhsics(self):
        """return a hypthetic (future) postition depending on the driving incliantion

        of the stearing wheal """

        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        """test sreight"""

        vessel.x = 100
        vessel.y = 100
        vessel.rotation = 0
        vessel.r = 20
        self.assertEqual(
            vessel.physics_stearing(driving=0, delta=10), (110, 100, 0))

        vessel.x = 100
        vessel.y = 100
        vessel.rotation = math.pi
        vessel.r = 20
        self.assertEqual(
            vessel.physics_stearing(driving=0, delta=10), (90, 100, math.pi))

        vessel.x = 100
        vessel.y = 100
        vessel.rotation = math.pi / 2
        vessel.r = 20
        self.assertEqual(
            vessel.physics_stearing(driving=0, delta=10), (100, 110, math.pi / 2))

        vessel.x = 100
        vessel.y = 100
        vessel.rotation = -math.pi / 2
        vessel.r = 20
        self.assertEqual(
            vessel.physics_stearing(driving=0, delta=10), (100.0, 90.0, -math.pi / 2))

        vessel.x = 100
        vessel.y = 100
        vessel.rotation = math.pi / 4
        vessel.r = 20
        self.assertAlmostEqual(
            vessel.physics_stearing(driving=0, delta=14.14213562)[0], 110, 4)
        self.assertAlmostEqual(
            vessel.physics_stearing(driving=0, delta=14.14213562)[1], 110, 4)

        # driving hard right
        vessel.x = 100
        vessel.y = 100
        vessel.rotation = 0
        vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=1, delta=math.pi / 2 * 20),
                         (120, 120, math.pi / 2))

        # driving hard left
        vessel.x = 100
        vessel.y = 100
        vessel.rotation = 0
        vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=-1, delta=math.pi / 2 * 20),
                         (120.0, 80.0, -math.pi / 2))

        # driving right, but not hard
        vessel.x = 100
        vessel.y = 100
        vessel.rotation = 0
        vessel.r = 10
        self.assertEqual(vessel.physics_stearing(driving=0.5, delta=math.pi / 2 * 20),
                         (120, 120, math.pi / 2))

    @unittest.skipIf(finished, 'done')
    def test_get_radius_to_point_ego(self):
        """ test the function: get_radius_to_point_ego

        this test tests the returned ancle and radius to a point
        in ego coordinates

        all points should be checked for left and right turn

        * for normal points somewhere
        * for points streight ahead
        * for points the vehicle must turn 180 degrees for


        """

        myFlaeche = main.Flaeche(xdim=450, ydim=450, scale=10)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        #############################################
        # right-turn:
        # find radius and angle to a point when
        # xx is positive and yy is positive
        # (Case A)
        #
        # left-turn:
        # xx is positive and yy is negative

        R = 20
        gamma_minuscle = math.pi / 12              # 15°
        gamma_majuscle = 2 * gamma_minuscle      # 30°
        xx_r = R * math.sin(gamma_majuscle)
        yy_r = R * (1 - math.cos(gamma_majuscle))
        xx_l = xx_r
        yy_l = - yy_r

        self.assertAlmostEqual(gamma_minuscle, 0.2617993877991494)
        self.assertAlmostEqual(gamma_majuscle, 0.5235987755982988)
        self.assertAlmostEqual(xx_r, 10)
        self.assertAlmostEqual(yy_r, 2.679491924311226)
        self.assertAlmostEqual(xx_l, 10)
        self.assertAlmostEqual(yy_l, -2.679491924311226)

        expected_result_right_radius = R
        expected_result_right_angle = gamma_majuscle
        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - gamma_majuscle
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #############################################
        # right-turn:
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)
        #
        # left-turn:
        # xx is positiv and yy is negative

        R = 20
        gamma_minuscle = math.pi / 8              # 22.5°
        gamma_majuscle = 2 * gamma_minuscle     # 45°
        xx_r = R * math.sin(gamma_majuscle)
        yy_r = R * (1 - math.cos(gamma_majuscle))
        xx_l = xx_r
        yy_l = - yy_r

        expected_result_right_radius = R
        expected_result_right_angle = gamma_majuscle
        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - gamma_majuscle
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #############################################
        # right-turn:
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)
        #
        # left-turn:
        # xx is positiv and yy is negative

        R = 20
        gamma_minuscle = math.pi / 6              # 30°
        gamma_majuscle = 2 * gamma_minuscle     # 60°
        xx_r = R * math.sin(gamma_majuscle)
        yy_r = R * (1 - math.cos(gamma_majuscle))
        xx_l = xx_r
        yy_l = - yy_r

        expected_result_right_radius = R
        expected_result_right_angle = gamma_majuscle
        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - gamma_majuscle
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #############################################
        # right-turn:
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)
        #
        # left-turn:
        # xx is positiv and yy is negative

        R = 20
        gamma_minuscle = math.pi / 4              # 45°
        gamma_majuscle = 2 * gamma_minuscle     # 90°
        xx_r = R * math.sin(gamma_majuscle)
        yy_r = R * (1 - math.cos(gamma_majuscle))
        xx_l = xx_r
        yy_l = - yy_r

        expected_result_right_radius = R
        expected_result_right_angle = gamma_majuscle
        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - gamma_majuscle
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)

        #############################################
        # right-turn:
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)
        #
        # left-turn:
        # xx is positiv and yy is negative

        R = 20
        gamma_minuscle = math.pi / 6  # 60°
        gamma_majuscle = 2 * gamma_minuscle     # 120°
        xx_r = R * math.sin(gamma_majuscle)
        yy_r = R * (1 - math.cos(gamma_majuscle))
        xx_l = xx_r
        yy_l = - yy_r

        expected_result_right_radius = R
        expected_result_right_angle = gamma_majuscle
        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - gamma_majuscle
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #############################################
        # right-turn:
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)
        #
        # left-turn:
        # xx is positiv and yy is negative

        R = 20
        gamma_minuscle = 5 * math.pi / 6  # 75°
        gamma_majuscle = 2 * gamma_minuscle     # 150°
        xx_r = R * math.sin(gamma_majuscle)
        yy_r = R * (1 - math.cos(gamma_majuscle))
        xx_l = xx_r
        yy_l = - yy_r

        expected_result_right_radius = R
        expected_result_right_angle = gamma_majuscle
        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - gamma_majuscle
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive (right turn)
        # xx is positiv and yy is negative (left  turn)
        #
        # full 180° turn
        #
        # Case (??)

        R = 20
        gamma_minuscle = math.pi / 2  # 90°
        gamma_majuscle = 2 * gamma_minuscle     # 180°
        xx_r = R * math.sin(gamma_majuscle)
        yy_r = R * (1 - math.cos(gamma_majuscle))
        xx_l = xx_r
        yy_l = - yy_r

        expected_result_right_radius = R
        expected_result_right_angle = gamma_majuscle
        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - gamma_majuscle
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #########
        # check what happens in case of a 180 degree left/right Turn

        R = 20
        AA_r = math.pi
        xx_r = 0
        yy_r = 40
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA_r)

        AA_l = -math.pi
        xx_l = 0
        yy_l = -40
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_l, yy_l))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_l, yy_l))[1], AA_l)

        AA = math.pi
        xx_r = 0.00000000001
        yy_r = 40
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        AA = -math.pi
        xx_l = 0.00000000001
        yy_l = -40
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_l, yy_l))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_l, yy_l))[1], AA)

        R = 10
        AA = math.pi
        xx_r = 0.00000000001
        yy_r = 20
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        #############################################
        # find radius and angle to a point when
        # xx is negativ but still yy is positive (right turn)
        # xx is negativ and also  yy is negative (left  turn)
        # (Case B)

        R = 20
        beta = math.pi / 6
        xx_r = -R * math.sin(beta)
        yy_r = R * (1 - math.cos(beta))
        xx_l = xx_r
        yy_l = -yy_r

        expected_result_right_radius = R
        expected_result_right_angle = 2 * math.pi - beta

        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - (2 * math.pi - beta)
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #############################################
        # find radius and angle to a point when
        # xx is negativ but still yy is positive (right turn)
        # xx is negativ and also  yy is negative (left  turn)
        # (Case B)

        R = 20
        beta = math.pi / 4
        xx_r = -R * math.sin(beta)
        yy_r = R * (1 - math.cos(beta))
        xx_l = xx_r
        yy_l = -yy_r

        expected_result_right_radius = R
        expected_result_right_angle = 2 * math.pi - beta

        result_right_radius, result_right_angle = vessel.get_radius_to_point_ego(
            (xx_r, yy_r))

        expected_result_left_radius = R
        expected_result_left_angle = - (2 * math.pi - beta)
        result_left_radius, result_left_angle = vessel.get_radius_to_point_ego(
            (xx_l, yy_l))

        self.assertAlmostEqual(
            result_right_radius, expected_result_right_radius)
        self.assertAlmostEqual(
            result_right_angle,  expected_result_right_angle)

        self.assertAlmostEqual(result_left_radius, expected_result_left_radius)
        self.assertAlmostEqual(result_left_angle,  expected_result_left_angle)

        #######
        # make sure, that you don't wish to get the radius between
        # two same points

        xx_r = 0.00000000001
        yy_r = 0.00000000001
        self.assertRaisesRegexp(StandardError, "points must be different",
                                vessel.get_radius_to_point_ego, (xx_r, yy_r))

        xx_r = -0.00000000001
        yy_r = -0.00000000001
        self.assertRaisesRegexp(StandardError, "points must be different",
                                vessel.get_radius_to_point_ego, (xx_r, yy_r))

        xx_r = 0
        yy_r = 0
        self.assertRaisesRegexp(StandardError, "points must be different",
                                vessel.get_radius_to_point_ego, (xx_r, yy_r))

        #######
        # check what happens if the point is straight ahead or streight behind

        R = float('inf')
        AA = 0
        xx_r = 10
        yy_r = 0
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        R = float('-inf')
        AA = 0
        xx_r = -100
        yy_r = 0
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(
            vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        ###########################################

    @unittest.skipIf(finished, 'done')
    def test_get_reachables_old_style(self):
        myFlaeche = main.Flaeche(
            xdim=450, ydim=450, scale=10, output='reachables')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        vessel.x = 100
        vessel.y = 105
        vessel.rotation = 0
        vessel.r = 20

        expected_result = sorted([(11, 10), (11, 9), (11, 11)])
        result = vessel.get_reachables()
        self.assertEqual(result, expected_result)

        vessel.x = 100
        vessel.y = 105
        vessel.rotation = 0
        vessel.r = 80
        expected_result = sorted(
            [(11.0, 10.0), (12.0, 9.0), (12.0, 10.0), (12.0, 11.0)])
        result = vessel.get_reachables()
        self.assertEqual(result, expected_result)

        vessel.x = 105
        vessel.y = 105
        vessel.rotation = math.pi / 4
        vessel.r = 180
        result = vessel.get_reachables()

        myFlaeche.vis_add_current(myFlaeche.get_cell((vessel.x, vessel.y)))
        myFlaeche.vis_add_reachable(result)

        myFlaeche.draw_course(vessel, vessel.r, 45)
        myFlaeche.vis_show()

    @unittest.skipIf(True, 'useless test without asserts')
    def test_get_reachables_old_style_pp(self):
        """this test was written to produce some bmp output for a presentation"""

        myFlaeche = main.Flaeche(
            xdim=100, ydim=100, scale=20, output='reachables_pp')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (4, 4), (4, 20), (-4, 20), (-4, 4), (0, 0)])

        vessel.x = 10
        vessel.y = 10
        vessel.rotation = .25 * math.pi
        vessel.r = 400

        myFlaeche.draw_course(vessel, vessel.r, 45)
        myFlaeche.vis_show()

    @unittest.skipIf(finished, 'done')
    def test_get_reachable_center_points(self):
        visual = True
        visual = False
        myFlaeche = main.Flaeche(
            xdim=500, ydim=500, scale=10, output='reachables_center')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), (0, 0)])

        vessel.x = 300
        vessel.y = 305
        vessel.rotation = 0
        vessel.r = 20

        # 'get_inner_ego_bounding_box'
        result = sorted(vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                           vessel.rotation,
                                                           test_result='get_inner_ego_bounding_box'))

        expected_result = sorted([(300.0, 285.0), (300.0, 325.0),
                                  (320.0, 285.0), (320.0, 325.0)])
        self.assertEqual(result, expected_result)

        # 'get_inner_ego_bounding_box' rotate by 45°
        vessel.x = 300
        vessel.y = 305
        vessel.rotation = math.pi / 4
        vessel.r = 20

        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_inner_ego_bounding_box')
        sorted_result = sorted(result)

        rr = vessel.r / math.sqrt(2)  # (route radius)
        expected_result = sorted([(300.0 - rr,   305.0 + rr),  # 1
                                  (300.0,   305.0 + 2 * rr),  # 2
                                  (300.0 + rr,   305.0 - rr),  # 3
                                  (300.0 + 2 * rr,   305.0)])  # 4

        visual_result = result
        visual_result += result[0]

        if visual:
            myFlaeche.vis_add_poly(visual_result, 'green')

        self.assertEqual(sorted_result, expected_result)

        # 'get_outer_ego_bounding_box'
        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_outer_ego_bounding_box')
        sorted_result = sorted(result)

        rr = vessel.r / math.sqrt(2)
        RR = 3 * vessel.r / math.sqrt(2)

        expected_result = sorted([(300.0 - RR,   305.0 + rr),  # 1
                                  (300.0 - rr,   305.0 + RR),  # 2
                                  (300.0 + RR,   305.0 - rr),  # 3
                                  (300.0 + rr,   305.0 - RR)])  # 4

        visual_result = result
        visual_result += result[0]

        if visual:
            myFlaeche.vis_add_poly(visual_result, 'green')

        self.assertEqual(sorted_result, expected_result)

        # 'get_global_bounding_box'
        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_global_bounding_box')
        sorted_result = sorted(result)

        expected_result = sorted([(300.0 - RR,   305.0 - RR),  # A1
                                  (300.0 - RR,   305.0 + RR),  # A2
                                  (300.0 + RR,   305.0 - RR),  # A3
                                  (300.0 + RR,   305.0 + RR)])  # A4

        visual_result = result
        visual_result += result[0]

        if visual:
            myFlaeche.vis_add_poly(visual_result, 'purple')

        self.assertEqual(sorted(sorted_result), sorted(expected_result))

        # 'get_all_center_points'
        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_all_center_points')

        expected_result = [
            (255.0, 265.0), (255.0, 275.0), (255.0, 285.0), (255.0, 295.0),
            (255.0, 305.0), (255.0, 315.0), (255.0,
                                             325.0), (255.0, 335.0), (255.0, 345.0),

            (265.0, 265.0), (265.0, 275.0), (265.0, 285.0), (265.0, 295.0),
            (265.0, 305.0), (265.0, 315.0), (265.0,
                                             325.0), (265.0, 335.0), (265.0, 345.0),

            (275.0, 265.0), (275.0, 275.0), (275.0, 285.0), (275.0, 295.0),
            (275.0, 305.0), (275.0, 315.0), (275.0,
                                             325.0), (275.0, 335.0), (275.0, 345.0),

            (285.0, 265.0), (285.0, 275.0), (285.0, 285.0), (285.0, 295.0),
            (285.0, 305.0), (285.0, 315.0), (285.0,
                                             325.0), (285.0, 335.0), (285.0, 345.0),

            (295.0, 265.0), (295.0, 275.0), (295.0, 285.0), (295.0, 295.0),
            (295.0, 305.0), (295.0, 315.0), (295.0,
                                             325.0), (295.0, 335.0), (295.0, 345.0),

            (305.0, 265.0), (305.0, 275.0), (305.0, 285.0), (305.0, 295.0),
            #            (305.0, 305.0),
            (305.0, 315.0), (305.0, 325.0), (305.0, 335.0), (305.0, 345.0),

            (315.0, 265.0), (315.0, 275.0), (315.0, 285.0), (315.0, 295.0),
            (315.0, 305.0), (315.0, 315.0), (315.0,
                                             325.0), (315.0, 335.0), (315.0, 345.0),

            (325.0, 265.0), (325.0, 275.0), (325.0, 285.0), (325.0, 295.0),
            (325.0, 305.0), (325.0, 315.0), (325.0,
                                             325.0), (325.0, 335.0), (325.0, 345.0),

            (335.0, 265.0), (335.0, 275.0), (335.0, 285.0), (335.0, 295.0),
            (335.0, 305.0), (335.0, 315.0), (335.0,
                                             325.0), (335.0, 335.0), (335.0, 345.0),

            (345.0, 265.0), (345.0, 275.0), (345.0, 285.0), (345.0, 295.0),
            (345.0, 305.0), (345.0, 315.0), (345.0, 325.0), (345.0, 335.0), (345.0, 345.0)]

        self.assertEqual(sorted(result), sorted(expected_result))

        if visual:
            for pp in result:
                myFlaeche.vis_add_single_point(pp, 'grey')

        myFlaeche.vis_add_current(myFlaeche.get_cell((vessel.x, vessel.y)))

        # 'get_zone_zero_center_points'
        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_zone_zero_center_points')

        expected_result_old = [
            (275.0, 305.0), (275.0, 315.0), (275.0, 325.0), (275.0, 335.0),
            (285.0, 305.0), (285.0, 315.0), (285.0, 325.0), (285.0, 335.0),
            (295.0, 285.0), (295.0, 295.0), (295.0, 305.0), (295.0, 315.0),
            (295.0, 325.0), (295.0, 335.0),
            (305.0, 275.0), (305.0, 285.0), (305.0, 295.0),
            #(305.0, 305.0),
            (305.0, 315.0), (305.0, 325.0),
            (315.0, 275.0), (315.0, 285.0), (315.0, 295.0), (315.0, 305.0),
            (325.0, 275.0), (325.0, 285.0), (325.0, 295.0), (325.0, 305.0)]

        # why do the results differ?
        expected_result = [
            (275.0, 305.0), (275.0, 315.0), (275.0, 325.0), (275.0, 335.0),
            (285.0, 305.0), (285.0, 315.0), (285.0, 325.0), (285.0, 335.0),
            (295.0, 295.0), (295.0, 305.0), (295.0, 315.0),
            (295.0, 325.0), (295.0, 335.0),

            (305.0, 275.0), (305.0, 285.0), (305.0, 295.0),
            #
            (305.0, 315.0),

            (315.0, 275.0), (315.0, 285.0), (315.0, 295.0), (315.0, 305.0),
            (325.0, 275.0), (325.0, 285.0), (325.0, 295.0), (325.0, 305.0)]

        self.assertEqual(sorted(result), sorted(expected_result))

        expected_result_out = [(295.0, 285.0),  # now zone three
                               (305.0, 325.0),  # now zone one
                               ]

        # get_zone_one_center_points
        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_zone_one_center_points')

        expected_result = [(305.0, 325.0), (315.0, 315.0)]
        self.assertEqual(sorted(result), sorted(expected_result))

        if visual:
            for pp in result:
                myFlaeche.vis_add_single_point(pp, 'green')

        # get_zone_two_center_points
        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_zone_two_center_points')

        expected_result = [
            (265.0, 345.0), (275.0, 345.0), (285.0, 345.0),
            (295.0, 345.0),
            (305.0, 335.0), (305.0, 345.0),
            (315.0, 325.0), (315.0, 335.0), (315.0, 345.0),
            (325.0, 315.0), (325.0, 325.0), (325.0, 335.0), (325.0, 345.0),
            (335.0, 275.0), (335.0, 285.0), (335.0, 295.0), (335.0, 305.0),
            (335.0, 315.0), (335.0, 325.0), (335.0, 335.0), (335.0, 345.0),
            (345.0, 265.0), (345.0, 275.0), (345.0, 285.0), (345.0, 295.0),
            (345.0, 305.0), (345.0, 315.0), (345.0, 325.0), (345.0, 335.0), (345.0, 345.0)]

        self.assertEqual(sorted(result), sorted(expected_result))
        if visual:
            for pp in result:
                myFlaeche.vis_add_single_point(pp, 'blue')

        # get_zone_three_center_points
        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_zone_three_center_points')

        expected_result = [
            (255.0, 265.0), (255.0, 275.0), (255.0, 285.0), (255.0, 295.0),
            (255.0, 305.0), (255.0, 315.0), (255.0,
                                             325.0), (255.0, 335.0), (255.0, 345.0),
            (265.0, 265.0), (265.0, 275.0), (265.0,
                                             285.0), (265.0, 295.0), (265.0, 305.0),
            (265.0, 315.0), (265.0, 325.0), (265.0, 335.0),
            (275.0, 265.0), (275.0, 275.0), (275.0, 285.0), (275.0, 295.0),
            (285.0, 265.0), (285.0, 275.0), (285.0, 285.0), (285.0, 295.0),
            (295.0, 265.0), (295.0, 275.0),
            (305.0, 265.0), (315.0, 265.0), (325.0, 265.0), (335.0, 265.0),
            (295.0, 285.0), ]

        if visual:
            for pp in result:
                myFlaeche.vis_add_single_point(pp, 'pink')

        self.assertEqual(sorted(result), sorted(expected_result))

        # get extention points

        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    test_result='get_extention_center_points')

        expected_result = [
            (215.0, 225.0), (215.0, 235.0), (215.0,
                                             245.0), (215.0, 255.0), (215.0, 265.0),
            (215.0, 275.0), (215.0, 285.0), (215.0,
                                             295.0), (215.0, 305.0), (215.0, 315.0),
            (215.0, 325.0), (215.0, 335.0), (215.0,
                                             345.0), (215.0, 355.0), (215.0, 365.0),
            (215.0, 375.0), (215.0, 385.0),
            (225.0, 225.0), (225.0, 235.0), (225.0,
                                             245.0), (225.0, 255.0), (225.0, 265.0),
            (225.0, 275.0), (225.0, 285.0), (225.0,
                                             295.0), (225.0, 305.0), (225.0, 315.0),
            (225.0, 325.0), (225.0, 335.0), (225.0,
                                             345.0), (225.0, 355.0), (225.0, 365.0),
            (225.0, 375.0), (225.0, 385.0),
            (235.0, 225.0), (235.0, 235.0), (235.0,
                                             245.0), (235.0, 255.0), (235.0, 265.0),
            (235.0, 275.0), (235.0, 285.0), (235.0,
                                             295.0), (235.0, 305.0), (235.0, 315.0),
            (235.0, 325.0), (235.0, 335.0), (235.0,
                                             345.0), (235.0, 355.0), (235.0, 365.0),
            (235.0, 375.0), (235.0, 385.0),
            (245.0, 225.0), (245.0, 235.0), (245.0,
                                             245.0), (245.0, 255.0), (245.0, 265.0),
            (245.0, 275.0), (245.0, 285.0), (245.0,
                                             295.0), (245.0, 305.0), (245.0, 315.0),
            (245.0, 325.0), (245.0, 335.0), (245.0,
                                             345.0), (245.0, 355.0), (245.0, 365.0),
            (245.0, 375.0), (245.0, 385.0),
            (255.0, 225.0), (255.0, 235.0), (255.0,
                                             245.0), (255.0, 255.0), (255.0, 355.0),
            (255.0, 365.0), (255.0, 375.0), (255.0, 385.0),
            (265.0, 225.0), (265.0, 235.0), (265.0,
                                             245.0), (265.0, 255.0), (265.0, 355.0),
            (265.0, 365.0), (265.0, 375.0), (265.0, 385.0),
            (275.0, 225.0), (275.0, 235.0), (275.0,
                                             245.0), (275.0, 255.0), (275.0, 355.0),
            (275.0, 365.0), (275.0, 375.0), (275.0, 385.0),
            (285.0, 225.0), (285.0, 235.0), (285.0,
                                             245.0), (285.0, 255.0), (285.0, 355.0),
            (285.0, 365.0), (285.0, 375.0), (285.0, 385.0),
            (295.0, 225.0), (295.0, 235.0), (295.0,
                                             245.0), (295.0, 255.0), (295.0, 355.0),
            (295.0, 365.0), (295.0, 375.0), (295.0, 385.0),
            (305.0, 225.0), (305.0, 235.0), (305.0,
                                             245.0), (305.0, 255.0), (305.0, 355.0),
            (305.0, 365.0), (305.0, 375.0), (305.0, 385.0),
            (315.0, 225.0), (315.0, 235.0), (315.0,
                                             245.0), (315.0, 255.0), (315.0, 355.0),
            (315.0, 365.0), (315.0, 375.0), (315.0, 385.0),
            (325.0, 225.0), (325.0, 235.0), (325.0,
                                             245.0), (325.0, 255.0), (325.0, 355.0),
            (325.0, 365.0), (325.0, 375.0), (325.0, 385.0),
            (335.0, 225.0), (335.0, 235.0), (335.0,
                                             245.0), (335.0, 255.0), (335.0, 355.0),
            (335.0, 365.0), (335.0, 375.0), (335.0, 385.0),
            (345.0, 225.0), (345.0, 235.0), (345.0,
                                             245.0), (345.0, 255.0), (345.0, 355.0),
            (345.0, 365.0), (345.0, 375.0), (345.0, 385.0),
            (355.0, 225.0), (355.0, 235.0), (355.0,
                                             245.0), (355.0, 255.0), (355.0, 265.0),
            (355.0, 275.0), (355.0, 285.0), (355.0,
                                             295.0), (355.0, 305.0), (355.0, 315.0),
            (355.0, 325.0), (355.0, 335.0), (355.0,
                                             345.0), (355.0, 355.0), (355.0, 365.0),
            (355.0, 375.0), (355.0, 385.0), (365.0,
                                             225.0), (365.0, 235.0), (365.0, 245.0),
            (365.0, 255.0), (365.0, 265.0), (365.0,
                                             275.0), (365.0, 285.0), (365.0, 295.0),
            (365.0, 305.0), (365.0, 315.0), (365.0,
                                             325.0), (365.0, 335.0), (365.0, 345.0),
            (365.0, 355.0), (365.0, 365.0), (365.0, 375.0), (365.0, 385.0),
            (375.0, 225.0), (375.0, 235.0), (375.0,
                                             245.0), (375.0, 255.0), (375.0, 265.0),
            (375.0, 275.0), (375.0, 285.0), (375.0,
                                             295.0), (375.0, 305.0), (375.0, 315.0),
            (375.0, 325.0), (375.0, 335.0), (375.0,
                                             345.0), (375.0, 355.0), (375.0, 365.0),
            (375.0, 375.0), (375.0, 385.0), (385.0,
                                             225.0), (385.0, 235.0), (385.0, 245.0),
            (385.0, 255.0), (385.0, 265.0), (385.0,
                                             275.0), (385.0, 285.0), (385.0, 295.0),
            (385.0, 305.0), (385.0, 315.0), (385.0,
                                             325.0), (385.0, 335.0), (385.0, 345.0),
            (385.0, 355.0), (385.0, 365.0), (385.0, 375.0), (385.0, 385.0)]

        self.assertEqual(sorted(result), sorted(expected_result))

        if visual:
            for pp in result:
                myFlaeche.vis_add_single_point(pp, 'darkseagreen')

        #############################
        # aux

        myFlaeche.draw_course(vessel, vessel.r, 2 * math.pi * vessel.r)

        if visual:
            myFlaeche.vis_show()
        return

    @unittest.skipIf(finished, 'done')
    def test_get_points_on_line_horizontal(self):
        #        vessel.x = 300; vessel.y = 305; vessel.rotation = 0; vessel.r = 20

        visual = VISUAL
        visual = True

        verbose = False

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=60, ydim=60, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        expected_result_pos = [(5.0, 25.0, 0), (10.0, 25.0, 0), (15.0, 25.0, 0),
                               (20.0, 25.0, 0), (25.0,
                                                 25.0, 0), (30.0, 25.0, 0),
                               (35.0, 25.0, 0), (40.0,
                                                 25.0, 0), (45.0, 25.0, 0),
                               (50.0, 25.0, 0), (55.0, 25.0, 0)
                               ]

        result_pos = vessel.get_points_on_line(start_point=(5, 25),
                                               end_point=(55, 25),
                                               omega=0,
                                               scale_fraction=2)
        assertAlmostEqualTupleList(self, expected_result_pos, result_pos)

        # invert start and end point
        result_pos = vessel.get_points_on_line(start_point=(55, 25),
                                               end_point=(5, 25),
                                               omega=0,
                                               scale_fraction=2)
        assertAlmostEqualTupleList(self, expected_result_pos, result_pos)

        if visual:
            for pp in result_pos:
                myFlaeche.vis_add_single_point(
                    (pp[0], pp[1] + 10), 'darkseagreen')

            myFlaeche.vis_show()

    @unittest.skipIf(finished, 'done')
    def test_get_points_on_line_diagonal(self):
        #        vessel.x = 300; vessel.y = 305; vessel.rotation = 0; vessel.r = 20

        visual = VISUAL
        visual = True

        verbose = False

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=60, ydim=60, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        expected_result = [(5.0, 5.0, 0),
                           (7.222222222222222,   7.222222222222222, 0),
                           (9.444444444444445,   9.444444444444445, 0),
                           (11.666666666666668, 11.666666666666668, 0),
                           (13.88888888888889,  13.88888888888889,  0),
                           (16.11111111111111,  16.11111111111111,  0),
                           (18.333333333333336, 18.333333333333336, 0),
                           (20.555555555555557, 20.555555555555557, 0),
                           (22.77777777777778,  22.77777777777778,  0),
                           (25.0,               25.0,               0),
                           (27.22222222222222,  27.22222222222222,  0),
                           (29.444444444444446, 29.444444444444446, 0),
                           (31.666666666666668, 31.666666666666668, 0),
                           (33.888888888888886, 33.888888888888886, 0),
                           (36.111111111111114, 36.111111111111114, 0),
                           (38.333333333333336, 38.333333333333336, 0),
                           (40.55555555555556,  40.55555555555556,  0),
                           (42.77777777777778,  42.77777777777778,  0),
                           (45.0,               45.0,               0)
                           ]

        result = vessel.get_points_on_line(start_point=(5, 5),
                                           end_point=(45, 45),
                                           omega=0,
                                           scale_fraction=3)

        assertAlmostEqualTupleList(self, expected_result, result)

        #########################
        # invert start and end point
        result = vessel.get_points_on_line(start_point=(45, 45),
                                           end_point=(5, 5),
                                           omega=0,
                                           scale_fraction=3)

        assertAlmostEqualTupleList(self, expected_result, result)

        if visual:
            for pp in result:
                myFlaeche.vis_add_single_point((pp[0], pp[1]), 'darkseagreen')

            myFlaeche.vis_show()

    @unittest.skipIf(devel_run, 'done')
    def test_get_points_on_line_vertical(self):
        #        vessel.x = 300; vessel.y = 305; vessel.rotation = 0; vessel.r = 20

        visual = VISUAL
        visual = True

        verbose = False

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=60, ydim=60, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        expected_result = [(25.0,  5.0,               0),
                           (25.0,  8.333333333333334, 0),
                           (25.0, 11.666666666666668, 0),
                           (25.0, 15.0,               0),
                           (25.0, 18.333333333333336, 0),
                           (25.0, 21.666666666666668, 0),
                           (25.0, 25.0,               0)]

        result = vessel.get_points_on_line(start_point=(25, 5),
                                           end_point=(25, 25),
                                           omega=0,
                                           scale_fraction=3)

        assertAlmostEqualTupleList(self, expected_result, result)

        #############
        # invert start and end point
        result = vessel.get_points_on_line(start_point=(25, 25),
                                           end_point=(25, 5),
                                           omega=0,
                                           scale_fraction=3)

        assertAlmostEqualTupleList(self, expected_result, result)

        if visual:
            for pp in result:
                myFlaeche.vis_add_single_point(
                    (pp[0], pp[1] + 10), 'darkseagreen')

            myFlaeche.vis_show()

    @unittest.skipIf(finished, 'done')
    def test_get_points_on_the_circle_ego(self):
        #        vessel.x = 300; vessel.y = 305; vessel.rotation = 0; vessel.r = 20

        visual = VISUAL

        verbose = False

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=20, ydim=20, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        expected_result_pos = [(0.0, 0.0, 0.0),
                               (1.0452846326765346, 0.0547810463172671,
                                0.10471975511965977),
                               (2.079116908177593, 0.2185239926619431,
                                0.20943951023931953),
                               (3.090169943749474, 0.4894348370484647,
                                0.3141592653589793),
                               (4.0673664307580015, 0.8645454235739913,
                                0.41887902047863906),
                               (4.999999999999999, 1.339745962155613,
                                0.5235987755982988),
                               (5.877852522924732, 1.9098300562505255,
                                0.6283185307179586),
                               (6.691306063588582, 2.5685517452260576,
                                0.7330382858376184),
                               (7.431448254773942, 3.3086939364114176,
                                0.8377580409572782),
                               (8.090169943749475, 4.12214747707527,
                                0.942477796076938),
                               (8.660254037844387, 5.000000000000001,
                                1.0471975511965979),
                               (9.135454576426008, 5.9326335692419985,
                                1.1519173063162575),
                               (9.510565162951535, 6.9098300562505255,
                                1.2566370614359172),
                               (9.781476007338055, 7.920883091822406,
                                1.361356816555577),
                               (9.945218953682733, 8.954715367323464,
                                1.4660765716752366),
                               (10.0, 9.999999999999996, 1.5707963267948963)]

        result_pos = vessel.get_points_on_the_circle_ego(radius_to_point=10,
                                                         angle_ego=0.5 * math.pi)
        assertAlmostEqualTupleList(self, expected_result_pos, result_pos)

        expected_result_neg = [(0.0, 0.0, 0.0),
                               (1.0452846326765373, -
                                0.0547810463172671, -0.10471975511965977),
                               (2.079116908177593, -
                                0.2185239926619431, -0.20943951023931953),
                               (3.090169943749475, -
                                0.4894348370484647, -0.3141592653589793),
                               (4.067366430758004, -
                                0.8645454235739913, -0.41887902047863906),
                               (4.999999999999999, -
                                1.339745962155613, -0.5235987755982988),
                               (5.877852522924733, -
                                1.9098300562505255, -0.6283185307179586),
                               (6.691306063588583, -
                                2.5685517452260576, -0.7330382858376184),
                               (7.431448254773945, -
                                3.3086939364114176, -0.8377580409572782),
                               (8.090169943749475, -
                                4.12214747707527, -0.942477796076938),
                               (8.660254037844387, -
                                5.000000000000001, -1.0471975511965979),
                               (9.13545457642601, -
                                5.9326335692419985, -1.1519173063162575),
                               (9.510565162951536, -
                                6.9098300562505255, -1.2566370614359172),
                               (9.781476007338057, -
                                7.920883091822406, -1.361356816555577),
                               (9.945218953682733, -
                                8.954715367323464, -1.4660765716752366),
                               (10.0, -9.999999999999996, -1.5707963267948963)]

        result_neg = vessel.get_points_on_the_circle_ego(radius_to_point=10,
                                                         angle_ego=-0.5 * math.pi)

        assertAlmostEqualTupleList(self, expected_result_neg, result_neg)

        if visual:
            for pp in result_pos:
                myFlaeche.vis_add_single_point(
                    (pp[0], pp[1] + 10), 'darkseagreen')

            for pp in result_neg:
                myFlaeche.vis_add_single_point((pp[0], pp[1] + 10), 'red')

            myFlaeche.vis_show()

    @unittest.skipIf(finished, 'done')
    def test_get_points_on_the_circle_ego_full_circle(self):
        #        vessel.x = 300; vessel.y = 305; vessel.rotation = 0; vessel.r = 20

        visual = VISUAL

        verbose = False

        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=60, ydim=60, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        expected_result_pos = [(0.0, 0.0, 0.0),
                               (4.999999999999999, 1.339745962155613,
                                0.5235987755982988),
                               (8.660254037844386, 4.999999999999999,
                                1.0471975511965976),
                               (10.0, 9.999999999999998, 1.5707963267948966),
                               (8.660254037844387, 14.999999999999998,
                                2.0943951023931953),
                               (5.0000000000000036,
                                18.660254037844382, 2.617993877991494),
                               (5.665538897647979e-15,
                                20.0, 3.1415926535897927),
                               (-4.999999999999994, 18.66025403784439,
                                3.6651914291880914),
                               (-8.660254037844384, 15.000000000000004,
                                4.1887902047863905),
                               (-10.0, 10.000000000000002, 4.71238898038469),
                               (-8.660254037844386,
                                4.999999999999999, 5.235987755982989),
                               (-4.9999999999999964, 1.3397459621556118, 5.759586531581288)]

        result_pos = vessel.get_points_on_the_circle_ego(radius_to_point=10,
                                                         angle_ego=2 * math.pi,
                                                         scale_fraction=2)

        assertAlmostEqualTupleList(self, expected_result_pos, result_pos)

        expected_result_neg = [(0.0, 0.0, 0.0),
                               (3.090169943749475, -
                                0.4894348370484647, -0.3141592653589793),
                               (5.877852522924733, -
                                1.9098300562505255, -0.6283185307179586),
                               (8.090169943749475, -
                                4.122147477075268, -0.9424777960769379),
                               (9.510565162951536, -
                                6.9098300562505255, -1.2566370614359172),
                               (10.0, -9.999999999999998, -1.5707963267948966),
                               (9.510565162951535, -
                                13.090169943749475, -1.8849555921538759),
                               (8.090169943749475, -
                                15.87785252292473, -2.199114857512855),
                               (5.877852522924732, -
                                18.090169943749473, -2.5132741228718345),
                               (3.090169943749474, -
                                19.510565162951536, -2.827433388230814),
                               (0.0, -20.0, -3.141592653589793),
                               (-3.090169943749474, -
                                19.510565162951536, -3.4557519189487724),
                               (-5.877852522924732, -
                                18.090169943749473, -3.7699111843077517),
                               (-8.090169943749475, -
                                15.877852522924734, -4.084070449666731),
                               (-9.510565162951535, -
                                13.090169943749475, -4.39822971502571),
                               (-10.0, -10.000000000000002, -4.71238898038469),
                               (-9.510565162951536, -
                                6.909830056250527, -5.026548245743669),
                               (-8.090169943749475, -
                                4.122147477075271, -5.340707511102648),
                               (-5.877852522924733, -
                                1.9098300562505266, -5.654866776461628),
                               (-3.090169943749475, -0.4894348370484647, -5.969026041820607)]
        result_neg = vessel.get_points_on_the_circle_ego(radius_to_point=10,
                                                         angle_ego=-2 *
                                                         math.pi,
                                                         scale_fraction=3)

        assertAlmostEqualTupleList(self, expected_result_neg, result_neg)

        if visual:
            for pp in result_pos:
                myFlaeche.vis_add_single_point(
                    (pp[0] + 30, pp[1] + 30), 'darkseagreen')

            for pp in result_neg:
                myFlaeche.vis_add_single_point((pp[0] + 30, pp[1] + 30), 'red')

            myFlaeche.vis_show()
