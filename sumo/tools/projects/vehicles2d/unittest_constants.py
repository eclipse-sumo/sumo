# -*- coding: utf-8 -*-
"""
@file    unittest_constants.py
@author  Marek Heinrich
@date    2015-02-24
@version $Id$

Test module keeping test related attributes.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


VISUAL = False
#VISUAL = True

# skip short cuts
lost = True
finished = False  # don't skip tests
afinished = True
unfinished = True  # tests that are more conceptal should not be run
# unless they are under developement
devel_skip = True  # things aren't ready jet for beeing tested
# this is what I am workin on, hence run this test and let it fail
devel_run = False
broken = True  # did work somewhen,
#  but due to developent this test was broken, fix after devel
visual = False  # finished tests that draw things
#
known_bug = True  # skip this test, as it will fail, but keep this example code
# to check in the end that the bug is realy dead
# good for buggs in multiple variants and good for stripping down and up

known_bug = False

debugging = False  # run the test, while trying to find out what is wrong here
