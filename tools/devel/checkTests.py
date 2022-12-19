#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkTests.py
# @author  Jakob Erdmann
# @date    2022-12-16

"""
apply runSeeds to a list of tests and compute statistics
use case: tweaking the lane change model and then seeing the result of a
gazilliion tests with reduced noise
"""

import os
import sys
import glob
from subprocess import call

tests, prefix, apps = sys.argv[1:]
SEEDS = "0:100"
THREADS = "16"

EXTRACT = os.path.join(os.environ['SUMO_HOME'], 'tools', 'extractTest.py')
RUNSEEDS = os.path.join(os.environ['SUMO_HOME'], 'tools', 'runSeeds.py')
ASTATS = os.path.join(os.environ['SUMO_HOME'], 'tools', 'output', 'attributeStats.py')

appdirs = [a + "_0" for a in apps.split(',')]

for test in open(tests).readlines()[1:]:
    test = test.strip()
    fullpath = os.path.join(prefix, test)
    call([EXTRACT, fullpath])
    tdir = '_' + test.replace('/', '_')
    os.chdir(tdir)
    call([RUNSEEDS,
          '-k', 'test.sumocfg',
          '-a', apps,
          '--seeds', SEEDS,
          '--threads', THREADS,
          '--no-warnings',
          '--statistic-output', 'stats.xml'])

    outf = open('compare.txt', 'a')
    for ad in appdirs:
        files = glob.glob(os.path.join(ad, '*.stats.xml'))
        args = [ASTATS,
                '-e', 'vehicleTripStatistics',
                '-a', 'timeLoss'] + files
        call(args, stdout=outf)
    outf.close()
    os.chdir('..')
