#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    test.py
# @author  Marek Heinrich
# @date    2014-11-17
# @version $Id$


import sys
import unittest
import unittest_flaeche
import unittest_vessel
import unittest_dijkstra
import unittest_ada_star
import unittest_ada_star_bugs_on_the_way


def notes():
    """this functions contains only comments

    continue developing the step

    make a test where there are no points left in the
    open list but the destinaition is not found

    """


if __name__ == '__main__':
    #   unittest.main()

    run_all_tests = False

    if len(sys.argv) > 1:
        if 'all' in sys.argv:
            run_all_tests = True

    if run_all_tests:
        suite = unittest.TestSuite()

        suite.addTests(unittest.TestLoader().loadTestsFromTestCase(
            unittest_flaeche.testCaseFlaeche))

        suite.addTests(unittest.TestLoader().loadTestsFromTestCase(
            unittest_vessel.testCaseVessel))

        suite.addTests(unittest.TestLoader().loadTestsFromTestCase(
            unittest_dijkstra.someTestcaseDD))

        suite.addTests(unittest.TestLoader().loadTestsFromTestCase(
            unittest_ada_star.someTestcase_ada_star))

#        suite.addTests(unittest.TestLoader().loadTestsFromTestCase(
#            unittest_ada_star_bugs_on_the_way.someTestcase_ada_star_bugs))

    else:
        suite = unittest.TestSuite()

#        suite.addTest(unittest_flaeche.testCaseFlaeche(
#            "test_all_nodes_in_list_are_valid_and_not_blocked"))

#        suite.addTest(unittest_vessel.testCaseVessel(
#            "test_get_points_on_the_circle_ego"))

#        suite.addTest(unittest_ada_star.someTestcase_ada_star(
#            "test_get_distance_between"))

        suite.addTest(unittest_ada_star_bugs_on_the_way.someTestcase_ada_star_bugs(
            "test_ada_run_and_rebuild_hindrance_one_spot_shadowed_improve"))


#        suite.addTest(someTestcase_ada_star("test_get_reachable_center_points_spooky"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_find_final__a"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_find_final"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_update_open_list"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_find_final__bug_a"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_find_final__bug_b"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_find_final__bug_c"))


#    unittest.TextTestRunner(verbosity=2).run(suite)
    unittest.TextTestRunner().run(suite)
