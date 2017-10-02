# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    unittest_constants.py
# @author  Marek Heinrich
# @date    2015-02-24
# @version $Id$



VISUAL = False
# VISUAL = True

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
