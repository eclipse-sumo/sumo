# -*- coding: utf-8 -*-
"""
@file    unittest_ada_star_bugs_on_the_way.py
@author  Marek Heinrich
@date    2015-11-17
@version $Id$

Unittests for keeping records of bugs. Not for beauties.

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


class someTestcase_ada_star_bugs(unittest.TestCase):
    ########### adastar #######################################

    @unittest.skipIf(devel_run, 'done')
    def test_ada_run_and_rebuild_hindrance_at_start_node(self):
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

        blocked_nodes = [(10, 10)]
        myFlaeche.load_node_data(blocked_nodes)

        start = (10, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()

        path = ANList(myD.path, 'tuples').get_tuples()

#        self.assertEqual(path, [
#            (30, 10, 0), (31, 10, 0), (32, 10, 0), (33, 10, 0),
#            (34, 10, 0), (35, 10, 0), (36, 10, 0), (37, 10, 0),
#            (38, 10, 0), (39, 10, 0), (40, 10, 0)
#        ])

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(devel_run, 'done')
    def test_ada_run_and_rebuild_hindrance_one_spot_ignored(self):
        """algo simply ignores the blocked node!!!"""
        visual = VISUAL
        visual = True

        image_name = self.__dict__['_testMethodName']

        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
#        vessel = main.Vessel(myFlaeche,
# wrong orientation of the vessel shape
# [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
#                             [(0, 0), (-40, 40), (-140, 40), (-140, -40), (-40, -40), ( 0, 0)])

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        vessel.r = 20
        vessel.speed = 10
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

#        blocked_nodes = [(20, 10)]
        blocked_nodes = [(20, 11)]
        myFlaeche.load_node_data(blocked_nodes)

        start = (10, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()

        path = ANList(myD.path, 'tuples').get_tuples()

#        self.assertEqual(path, [
#            (30, 10, 0), (31, 10, 0), (32, 10, 0), (33, 10, 0),
#            (34, 10, 0), (35, 10, 0), (36, 10, 0), (37, 10, 0),
#            (38, 10, 0), (39, 10, 0), (40, 10, 0)
#        ])

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(devel_run, 'done')
    def test_ada_run_and_rebuild_hindrance_one_spot_ignored_still_oczil(self):
        """algo simply ignores the blocked node!!!"""
        visual = VISUAL
        visual = True

        image_name = self.__dict__['_testMethodName']

        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        vessel.r = 40  # 20
        vessel.speed = 10
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

        blocked_nodes = [(20, 10)]
        myFlaeche.load_node_data(blocked_nodes)

        start = (10, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()

        path = ANList(myD.path, 'tuples').get_tuples()
        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(devel_run, 'done')
    def test_ada_run_and_rebuild_hindrance_one_spot_shadowed_improve(self):
        """algo simply ignores the blocked node!!!"""
        visual = VISUAL
        visual = True

        image_name = self.__dict__['_testMethodName']

        myFlaeche = main.Flaeche(xdim=500, ydim=340, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        vessel.r = 40
        vessel.speed = 10

        blocked_nodes = [
            (20, 18), (21, 18), (22, 18), (23, 18),  # (24, 18),
            (20, 19), (21, 19), (22, 19), (23, 19), (24, 19),
            (20, 20), (21, 20), (22, 20), (23, 20),  # (24, 20),
            #                         (20, 21), (21, 21), (22, 21), (23, 21), (24, 21),
        ]
        myFlaeche.load_node_data(blocked_nodes)

        start = (10, 17, 0)  # cell coordinates
        end = (40, 17, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()

        path = ANList(myD.path, 'tuples').get_tuples()

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(devel_run, 'done')
    def test_ada_run_and_rebuild_hindrance_one_spot_oscilating(self):
        """algo starts to send the vessel on an ocilating track"""
        visual = VISUAL
        visual = True

        image_name = self.__dict__['_testMethodName']

        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
#        vessel = main.Vessel(myFlaeche,
# wrong orientation of the vessel shape
# [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
#                             [(0, 0), (-40, 40), (-140, 40), (-140, -40), (-40, -40), ( 0, 0)])

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10),
                              (-10, -10), (0, 0)])

        vessel.r = 20
        vessel.speed = 10
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

#        blocked_nodes = [(xx, 10) for xx in range(7, 28)]
        blocked_nodes = [(15, 10)]
        myFlaeche.load_node_data(blocked_nodes)

        start = (10, 10, 0)  # cell coordinates
        end = (40, 10, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()

        path = ANList(myD.path, 'tuples').get_tuples()

#        self.assertEqual(path, [
#            (30, 10, 0), (31, 10, 0), (32, 10, 0), (33, 10, 0),
#            (34, 10, 0), (35, 10, 0), (36, 10, 0), (37, 10, 0),
#            (38, 10, 0), (39, 10, 0), (40, 10, 0)
#        ])

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    def test_get_distance_between_endlessly_long(self):
        """building the vessel takes ages, as calculating
        the shadow seems to be pretty expencive,
        small scale huge ship

        for sure has an evil impact on the pathfinding"""

        scale = 0.25
        start = (10, 10, 0)
        end = (20, 10, 0)
        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=scale)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200),
                                 (-40, 40), (0, 0)])

    @unittest.skipIf(devel_run, 'done')
    def test_get_predefiend_shadow_shape(self):
        """why is the black shade missing one cell??"""
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

        vessel.generate_predefiend_shadow_shapes()

        vessel.transform_hull_points(
            myFlaeche.get_angle_from_sector_id(0), (105, 105))

        result = vessel.predefined_shadow_shapes[0]

        for ii in range(len(result)):
            print
            print ii, ': ', result[ii]
            result[ii] = (result[ii][0] + 10, result[ii][1] + 10)
            print ii, ': ', result[ii]

        if visual:
            myFlaeche.vis_reset()
            myFlaeche.vis_add_grey(result)
            myFlaeche.vis_show(vessel.transformed_hull_points)
#            myFlaeche.vis_show()

    @unittest.skipIf(devel_run, 'done')
    def test_get_predefiend_shadow_shape(self):
        """why is the black shade missing one cell??"""
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

        vessel.generate_predefiend_shadow_shapes()

        sector_to_inspect = 8

        vessel.transform_hull_points(myFlaeche.get_angle_from_sector_id(sector_to_inspect),
                                     (105, 105))

        result = vessel.predefined_shadow_shapes[sector_to_inspect]

        for ii in range(len(result)):
            print
            print ii, ': ', result[ii]
            result[ii] = (result[ii][0] + 10, result[ii][1] + 10)
            print ii, ': ', result[ii]

        if visual:
            myFlaeche.vis_reset()
            myFlaeche.vis_add_black(result)
            myFlaeche.vis_show(vessel.transformed_hull_points)

    @unittest.skipIf(finished, 'done')
    def test_ada_no_hindrances_circle_diagonal_up_to_down(self):
        """

        The vessel has a radius of __only_20__ and moves over
        an open space from up to down without any hindrances.
        It follows it's turning cirle and then leaves it
        to get to the destination on a diagonal path.

        This test used to be a bug.
        """

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

        start = (5,  11, 8)  # cell coordinates
        end = (16, 19, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run(verbose=True, visual=True)
        myD.rebuild_path()
#       myD.patch_path()

        if False:  # print results
            sys.exit(sorted(ANList(myD.path, 'tuples').get_tuples()))

        expected_result = [(4, 15, 1), (5, 11, 8), (7, 16, 1),
                           (10, 17, 1), (13, 18, 1), (16, 19, 1)]
        result = sorted(ANList(myD.path, 'tuples').get_tuples())

        self.assertEqual(expected_result, result)

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(finished, 'done')
    def test_ada_no_hindrances_circle_diagonal_down_to_up_left(self):
        """

        The vessel has a radius of __only_20__ and moves over
        an open space from __down_to_up__ without any hindrances.
        It follows it's turning cirle and then leaves it
        to get to the destination on a diagonal path.

        This test used to be a bug.
        """

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

        end = (5,  11, 8)  # cell coordinates
        start = (16, 19, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run(verbose=True, visual=True)
        myD.rebuild_path()

        if False:  # print results
            sys.exit(sorted(ANList(myD.path, 'tuples').get_tuples()))

        expected_result = [(5, 11, 9), (8, 12, 9), (11, 13, 9),
                           (14, 14, 9), (16, 19, 0), (17, 15, 9)]
        result = sorted(ANList(myD.path, 'tuples').get_tuples())

        self.assertEqual(expected_result, result)

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(finished, 'done')
    def test_ada_no_hindrances_circle_diagonal_down_to_up_right(self):
        """

        The vessel has a radius of __only_20__ and moves over
        an open space from __down_to_up__ __with_different_start_orientation__
        without any hindrances.
        It follows it's turning cirle and then leaves it
        to get to the destination on a diagonal path.

        This test used to be a bug.
        """

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

        end = (5,  11, 8)  # cell coordinates
        start = (16, 19, 8)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run(verbose=True, visual=True)
        myD.rebuild_path()

        if False:  # print results
            sys.exit(sorted(ANList(myD.path, 'tuples').get_tuples()))

        expected_result = [(5, 11, 10), (8, 13,  9),
                           (12, 16, 10), (14, 18, 10), (16, 19, 8)]

        result = sorted(ANList(myD.path, 'tuples').get_tuples())

        self.assertEqual(expected_result, result)

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(finished, 'done')
    def test_ada_no_hindrances_circle_horisontal_from_up_to_down_outside_canvas(self):
        """

        The vessel has a radius of 40 and moves over
        an open space from up to down without any hindrances.
        The radius is pretty much the vertical distance
        between the start and destination so the vessel moves
        allong its turning circle and then moves straight
        forward (horisontally) to destination. 

        In this configuration the vessel leaves the canvas partially.
        This should be impossible in the future.


        This test used to be a bug.
        """

        visual = VISUAL
        visual = True
        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 40
        vessel.speed = 10  # m/s

        start = (5,  11, 8)  # cell coordinates
        end = (16, 19, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()
        myD.rebuild_path()

        if False:  # print results
            sys.exit(sorted(ANList(myD.path, 'tuples').get_tuples()))

        expected_result = [(1, 16, 3), (2, 12, 6), (5, 11, 8),
                           (6, 19, 0), (7, 19, 0), (10, 19, 0),
                           (13, 19, 0), (16, 19, 0)]
        result = sorted(ANList(myD.path, 'tuples').get_tuples())

        self.assertEqual(expected_result, result)

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(finished, 'done')
    def test_ada_no_hindrances_circle_horisontal_from_up_to_down_inside_canvas(self):
        """

        The vessel has a radius of 40 and moves over
        an open space from up to down without any hindrances.
        The radius is pretty much the vertical distance
        between the start and destination so the vessel moves
        allong its turning circle and then moves straight
        forward (horisontally) to destination. 

        In this configuration the vessel does not leave the canvas
        at all, hence it should allways pass in the future.

        This test used to be a bug.
        """

        visual = VISUAL
        visual = True
        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 40
        vessel.speed = 10  # m/s

        start = (15,  11, 8)  # cell coordinates
        end = (26, 19, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()
        myD.rebuild_path()

        if False:  # print results
            sys.exit(sorted(ANList(myD.path, 'tuples').get_tuples()))

        expected_result = [(11, 16, 3), (12, 12, 6), (15, 11, 8),
                           (16, 19, 0), (17, 19, 0), (20, 19, 0),
                           (23, 19, 0), (26, 19, 0)]

        result = sorted(ANList(myD.path, 'tuples').get_tuples())
        self.assertEqual(expected_result, result)

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(finished, 'done')
    def test_ada_no_hindrances_circle_horisontal_from_down_to_up_outside_canvas(self):
        """

        The vessel has a radius of 40 and moves over
        an open space from __down_to_up__ without any hindrances.
        The radius is pretty much the vertical distance
        between the start and destination so the vessel moves
        allong its turning circle and then moves straight
        forward (horisontally) to destination. 

        In this configuration the vessel  __does_leave_the_canvas__.
        Hence it shall fail somewhen in the future.

        This test used to be a bug.
        """

        visual = VISUAL
        visual = True
        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 40
        vessel.speed = 10  # m/s


# start = (5,  11, 8)  # cell coordinates
#        end   = (16, 19, 0)
        start = (5,  21, 8)  # cell coordinates
        end = (16, 13, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()
        myD.rebuild_path()
#        myD.patch_path()

        if False:  # print results
            sys.exit(sorted(ANList(myD.path, 'tuples').get_tuples()))

        expected_result = [(1, 16, 13), (2, 20, 10), (5, 21, 8),
                           (6, 13,  0), (7, 13,  0), (10, 13, 0),
                           (13, 13,  0), (16, 13,  0)]
        result = sorted(ANList(myD.path, 'tuples').get_tuples())
        self.assertEqual(expected_result, result)

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

#        sys.exit()

    @unittest.skipIf(finished, 'done')
    def test_ada_no_hindrances_circle_horisontal_from_down_to_up_inside_canvas(self):
        """

        The vessel has a radius of 40 and moves over
        an open space from __down_to_up__ without any hindrances.
        The radius is pretty much the vertical distance
        between the start and destination so the vessel moves
        allong its turning circle and then moves straight
        forward (horisontally) to destination. 

        In this configuration the vessel  __never_leaves_the_canvas__.
        Hence it __should_allways_pass__ in the future.

        This test used to be a bug.
        """

        visual = VISUAL
        visual = True
        image_name = self.__dict__['_testMethodName']
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output=image_name)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 40
        vessel.speed = 10  # m/s

        start = (15,  21, 8)  # cell coordinates
        end = (26, 13, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()
        myD.rebuild_path()
#       myD.patch_path()

        if False:  # print results
            sys.exit(sorted(ANList(myD.path, 'tuples').get_tuples()))

        expected_result = [(11, 16, 13), (12, 20, 10), (15, 21, 8),
                           (16, 13, 0),  (17, 13, 0),  (20, 13, 0),
                           (23, 13, 0), (26, 13, 0)]

        result = sorted(ANList(myD.path, 'tuples').get_tuples())
        self.assertEqual(expected_result, result)

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(broken, '')
    def test_dijsktra_step_internals_find_final__bug_a_s_archive_version_asdas(self):
        """ unclear bug

        the result of this test is odd.
        when looking at the graphical output,
        it seem like the vehilce starts and ends in
        the wrong positions.
        """

        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__a_A')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 8)  # cell coordinates
        end = (16, 19, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()
        myD.rebuild_path()
        myD.patch_path()

        if visual:
            myD.draw_path(vessel=vessel)
        del(myD)

    @unittest.skipIf(broken, '')
#    @unittest.skipIf(known_bug, '')
    def test_dijsktra_step_internals_find_final__bug_b(self):
        """ unclear bug

        this combo and end point combo does not work,
        algo seaches for ever and finally 
        ends with an error after some 2.5 min  """
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__b')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 8)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()

    @unittest.skipIf(broken, '')
#    @unittest.skipIf(known_bug, '')
    def test_dijsktra_step_internals_find_final__bug_c(self):
        """ unclear bug

        this combo and end point combo does not work,
        algo seaches for ever and finally 
        ends with an error after some 2.5 min  """
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__c')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 0)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()

    @unittest.skipIf(broken, '')
#    @unittest.skipIf(debugging, '')
    def test_dijsktra_step_internals_find_final__bug_d(self):
        """ unclear bug

        this combo and end point combo does not work,
        algo seaches for ever and finally 
        ends with an error after some 2.5 min  """
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__d')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 0)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

####### surgery cut ####
        if False:

            myD.step()
            myD.step()
            print myD.get_open_nodes('tuples')

            print len(myD.get_open_nodes('tuples'))
            assert False


####### surgery glue ####

        myD.run()

    @unittest.skipIf(True, '')
    def test_dijsktra_step_internals_find_final__b(self):
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada')
        vessel = main.Vessel(myFlaeche,
                             #                             [(10, 0), (0, 10), (-20, 10), (-20, -10), (0, -10), ( 10, 0)])
                             # nosy:
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
# mistaken one:                [(0, 0), (10, 10), (10, 30), (-10, 30),
# (-10, 10), ( 0, 0)])
        vessel.r = 20

       ## blocked_nodes = [(xx, 15) for xx in range(10, 28)]
       # blocked_nodes = [(xx, 15) for xx in range(7, 28)]
       # myFlaeche.load_node_data(blocked_nodes)

        # start = (3,  11, 8)  # cell coordinates
        #end   = (16, 19, 0)

        start = (5,  20, 8)  # cell coordinates
        end = (22, 19, 0)

        # unclear bug
        # this combo does not work,
        # ending with an error after some 2.5 min
        start = (5,  11, 8)  # cell coordinates
        end = (16, 19, 0)

        start = (5,  11, 8)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

# myD.run(verbose=True, visual=visual) #        myD.run(visual=visual)


####### surgery cut ####
        if False:

            myD.step()
            myD.step()
            print myD.get_open_nodes('tuples')

            print len(myD.get_open_nodes('tuples'))

            assert False
            #        self.assertEqual(myD_step_zero.get_open_nodes('tuples'), [( 30, 10, 0) ])
            #        self.assertEqual(myD_step_zero.get_closed_nodes('tuples'), [])


####### surgery glue ####

        myD.run()
        if False:
            myD.rebuild_path()

            for bla in myD.path:
                print bla.id, bla.reached_by_angle * 180 / math.pi

                expected_result = [(3, 11, 0), (5, 12, 2), (8, 15, 6), (9, 15, 1), (10, 16, 1),
                                   (11, 17, 1), (12, 18, 1), (13, 19, 1), (15, 20, 3), (22, 19, 0)]
#        self.assertEqual(ANList(myD.path, 'tuples').get_tuples(), expected_result)
#        main.make_movie(myFlaeche.output)
#        if visual:
                myD.draw_path(vessel=vessel)

    @unittest.skipIf(broken, 'done')
##################
#    @unittest.skipIf(devel_run, 'done')
    def test_get_reachable_center_points_spooky(self):
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada_spooky')
        vessel = main.Vessel(myFlaeche,  # nosy:
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20

        vessel.x, vessel.y = myFlaeche.get_possition_from_cell_center_id(
            (5, 11))
        vessel.rotation = myFlaeche.get_angle_from_sector(8)

        print
        # self.vessel.get_reachable_center_points(( 55.0 115.0 ) 8 )))

        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    )
#                                                    test_result='get_all_center_points')
#!!                                                  test_result='get_zone_zero_center_points')
#                                                    test_result='get_zone_one_center_points')
#                                                    test_result='get_zone_two_center_points')
#                                                    test_result='get_zone_three_center_points')
#                                                     test_result='get_extention_center_points')

        for ii in result:
            if ii[0] == 45:
                print ii

        print 'counted points', len(result)

        for pp in result:
            myFlaeche.vis_add_single_point(pp[0:2], 'blue')

        myFlaeche.vis_add_colored_point((5, 11), 'red')
        myFlaeche.vis_add_colored_point((5, 12), 'grey')

        if visual:
            myFlaeche.vis_show()

    @unittest.skipIf(devel_skip, 'done')
    def test_wiki(self):
        visual = VISUAL
        """run the aglo like in the wikipedia example"""
        myFlaeche = main.Flaeche(
            xdim=300, ydim=300, scale=10, output='result_mediawiki_example')
        myD = main.AdAStar(myFlaeche, (17, 3), (3, 17))

        blocked_nodes = [(xx, 7) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 8) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 9) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 10) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 11) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 12) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 13) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 14) for xx in range(13, 16)]

        myFlaeche.load_node_data(blocked_nodes)
        myD = main.AdAStar(myFlaeche, (3, 19), (18, 3))
        myD.run(visual=visual)
        myD.rebuild_path()
        self.assertEqual(myD.DNList(myD.path, 'tuples').get_tuples(),
                         [(3, 19), (4, 18), (5, 17), (6, 16), (7, 15), (8, 15),
                          (9, 15), (10, 15), (11, 15), (12,
                                                        15), (13, 15), (14, 15),
                          (15, 15), (16, 14), (16, 13), (16,
                                                         12), (16, 11), (16, 10),
                          (16, 9), (16, 8), (16, 7), (17, 6), (17, 5), (17, 4), (18, 3)])

        if visual:
            myD.draw_path(final=True)
            main.make_movie(myFlaeche.output)


##############
#     this test is not realy usefull now, because it catches an error for
#     both: not giving the coorrect coordinates or giving coordinates without angle
#     Maybe make the teste better later or remove in 2016 :-)

#    @unittest.skipIf(devel_run, 'done')
#    def test_adastar_to_do(self):
#        """test for illegal start or end possitions"""
#
#        start = (0, 0, 0)
#        end = (20, 10, 0)
#        myFlaeche = main.Flaeche(xdim=10,ydim=10,scale=1)
        #    self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), ( 1,  1) )


#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), (11, 11) )
        # self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), (11,  1) )
        1
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 0, 0) )

    @unittest.skipIf(broken, '')
    def test_dijsktra_step_internals_find_final__bug_a_s_archive_version(self):
        """ unclear bug

        the result of this test is odd.
        when looking at the graphical output,
        it seem like the vehilce starts and ends in
        the wrong positions.
        """

        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__a_A')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 8)  # cell coordinates
        end = (16, 19, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()
        myD.rebuild_path()
        myD.patch_path()

#        if visual:
#            myD.draw_path(vessel=vessel)
#        del(myD)

    @unittest.skipIf(broken, '')
#    @unittest.skipIf(known_bug, '')
    def test_dijsktra_step_internals_find_final__bug_b(self):
        """ unclear bug

        this combo and end point combo does not work,
        algo seaches for ever and finally 
        ends with an error after some 2.5 min  """
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__b')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 8)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()

    @unittest.skipIf(broken, '')
#    @unittest.skipIf(known_bug, '')
    def test_dijsktra_step_internals_find_final__bug_c(self):
        """ unclear bug

        this combo and end point combo does not work,
        algo seaches for ever and finally 
        ends with an error after some 2.5 min  """
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__c')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 0)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD.run()

    @unittest.skipIf(broken, '')
#    @unittest.skipIf(debugging, '')
    def test_dijsktra_step_internals_find_final__bug_d(self):
        """ unclear bug

        this combo and end point combo does not work,
        algo seaches for ever and finally 
        ends with an error after some 2.5 min  """
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada__d')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20
        vessel.speed = 10  # m/s

        start = (5,  11, 0)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

####### surgery cut ####
        if False:

            myD.step()
            myD.step()
            print myD.get_open_nodes('tuples')

            print len(myD.get_open_nodes('tuples'))
            assert False


####### surgery glue ####

        myD.run()

    @unittest.skipIf(True, '')
    def test_dijsktra_step_internals_find_final__b(self):
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada')
        vessel = main.Vessel(myFlaeche,
                             #                             [(10, 0), (0, 10), (-20, 10), (-20, -10), (0, -10), ( 10, 0)])
                             # nosy:
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
# mistaken one:                [(0, 0), (10, 10), (10, 30), (-10, 30),
# (-10, 10), ( 0, 0)])
        vessel.r = 20

       ## blocked_nodes = [(xx, 15) for xx in range(10, 28)]
       # blocked_nodes = [(xx, 15) for xx in range(7, 28)]
       # myFlaeche.load_node_data(blocked_nodes)

        # start = (3,  11, 8)  # cell coordinates
        #end   = (16, 19, 0)

        start = (5,  20, 8)  # cell coordinates
        end = (22, 19, 0)

        # unclear bug
        # this combo does not work,
        # ending with an error after some 2.5 min
        start = (5,  11, 8)  # cell coordinates
        end = (16, 19, 0)

        start = (5,  11, 8)  # cell coordinates
        end = (20, 11, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

# myD.run(verbose=True, visual=visual) #        myD.run(visual=visual)


####### surgery cut ####
        if False:

            myD.step()
            myD.step()
            print myD.get_open_nodes('tuples')

            print len(myD.get_open_nodes('tuples'))

            assert False
            #        self.assertEqual(myD_step_zero.get_open_nodes('tuples'), [( 30, 10, 0) ])
            #        self.assertEqual(myD_step_zero.get_closed_nodes('tuples'), [])


####### surgery glue ####

        myD.run()
        if False:
            myD.rebuild_path()

            for bla in myD.path:
                print bla.id, bla.reached_by_angle * 180 / math.pi

                expected_result = [(3, 11, 0), (5, 12, 2), (8, 15, 6), (9, 15, 1), (10, 16, 1),
                                   (11, 17, 1), (12, 18, 1), (13, 19, 1), (15, 20, 3), (22, 19, 0)]
#        self.assertEqual(ANList(myD.path, 'tuples').get_tuples(), expected_result)
#        main.make_movie(myFlaeche.output)
#        if visual:
                myD.draw_path(vessel=vessel)

    @unittest.skipIf(broken, 'done')
##################
#    @unittest.skipIf(devel_run, 'done')
    def test_get_reachable_center_points_spooky(self):
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500, ydim=500, scale=10,
                                 output='result_hindrance_punctual_ada_spooky')
        vessel = main.Vessel(myFlaeche,  # nosy:
                             [(0, 0), (-10, 10), (-30, 10), (-30, -10), (-10, -10), (0, 0)])
        vessel.r = 20

        vessel.x, vessel.y = myFlaeche.get_possition_from_cell_center_id(
            (5, 11))
        vessel.rotation = myFlaeche.get_angle_from_sector(8)

        print
        # self.vessel.get_reachable_center_points(( 55.0 115.0 ) 8 )))

        result = vessel.get_reachable_center_points((vessel.x, vessel.y),
                                                    vessel.rotation,
                                                    )
#                                                    test_result='get_all_center_points')
#!!                                                  test_result='get_zone_zero_center_points')
#                                                    test_result='get_zone_one_center_points')
#                                                    test_result='get_zone_two_center_points')
#                                                    test_result='get_zone_three_center_points')
#                                                     test_result='get_extention_center_points')

        for ii in result:
            if ii[0] == 45:
                print ii

        print 'counted points', len(result)

        for pp in result:
            myFlaeche.vis_add_single_point(pp[0:2], 'blue')

        myFlaeche.vis_add_colored_point((5, 11), 'red')
        myFlaeche.vis_add_colored_point((5, 12), 'grey')

        if visual:
            myFlaeche.vis_show()

    @unittest.skipIf(devel_skip, 'done')
    def test_wiki(self):
        visual = VISUAL
        """run the aglo like in the wikipedia example"""
        myFlaeche = main.Flaeche(
            xdim=300, ydim=300, scale=10, output='result_mediawiki_example')
        myD = main.AdAStar(myFlaeche, (17, 3), (3, 17))

        blocked_nodes = [(xx, 7) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 8) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 9) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 10) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 11) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 12) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 13) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 14) for xx in range(13, 16)]

        myFlaeche.load_node_data(blocked_nodes)
        myD = main.AdAStar(myFlaeche, (3, 19), (18, 3))
        myD.run(visual=visual)
        myD.rebuild_path()
        self.assertEqual(myD.DNList(myD.path, 'tuples').get_tuples(),
                         [(3, 19), (4, 18), (5, 17), (6, 16), (7, 15), (8, 15),
                          (9, 15), (10, 15), (11, 15), (12,
                                                        15), (13, 15), (14, 15),
                          (15, 15), (16, 14), (16, 13), (16,
                                                         12), (16, 11), (16, 10),
                          (16, 9), (16, 8), (16, 7), (17, 6), (17, 5), (17, 4), (18, 3)])

        if visual:
            myD.draw_path(final=True)
            main.make_movie(myFlaeche.output)


##############
#     this test is not realy usefull now, because it catches an error for
#     both: not giving the coorrect coordinates or giving coordinates without angle
#     Maybe make the teste better later or remove in 2016 :-)

#    @unittest.skipIf(devel_run, 'done')
#    def test_adastar_to_do(self):
#        """test for illegal start or end possitions"""
#
#        start = (0, 0, 0)
#        end = (20, 10, 0)
#        myFlaeche = main.Flaeche(xdim=10,ydim=10,scale=1)
        #    self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), ( 1,  1) )


#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), (11, 11) )
        # self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), (11,  1) )
        1
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 0, 0) )
