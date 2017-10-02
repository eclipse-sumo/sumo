# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    unittest_utils.py
# @author  Marek Heinrich
# @date    2015-02-24
# @version $Id$



def skip_all_but_selected():
    global lost, finished, afinished, unfinished, devel_skip, devel_run, broken, visual

    lost = True
    finished = True
    afinished = True
    unfinished = True
    devel_skip = True
    devel_run = True
    broken = True
    visual = True

# skip_all_but_selected()


def assertAlmostEqualTupleList(not_self, result, expected_result):
    if (len(result) == 0 or len(expected_result) == 0 or
            len(result) != len(expected_result)):
        assert False, "lists have differing length or length of zero"

    for ii in range(len(result)):
        not_self.assertAlmostEqual(result[ii][0], expected_result[ii][0])
        not_self.assertAlmostEqual(result[ii][1], expected_result[ii][1])
