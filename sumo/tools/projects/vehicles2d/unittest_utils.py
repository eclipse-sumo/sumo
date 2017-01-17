# -*- coding: utf-8 -*-
"""
@file    unittest_utils.py
@author  Marek Heinrich
@date    2015-02-24
@version $Id$

Test module for utils functions.

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


from unittest_constants import VISUAL, lost, finished, afinished, unfinished, devel_skip, devel_run, broken, visual, known_bug, debugging


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
    if (len(result) == 0 or len(expected_result) == 0
            or len(result) != len(expected_result)):
        assert False, "lists have differing length or length of zero"

    for ii in range(len(result)):
        not_self.assertAlmostEqual(result[ii][0], expected_result[ii][0])
        not_self.assertAlmostEqual(result[ii][1], expected_result[ii][1])
