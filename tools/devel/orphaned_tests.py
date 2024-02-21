#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    orhaphaned_tests.py
# @author  Jakob Erdmann
# @date    2024-02-21


"""find test folders that are not mentioned in the respective test suite
"""
from __future__ import print_function
import sys
import os
from collections import defaultdict
import glob


for root, dirs, files in os.walk(sys.argv[1]):
    numSuites = 0
    known_tests = set()
    for fname in files:
        if fname.startswith("testsuite."):
            numSuites += 1
            with open(os.path.join(root, fname)) as s:
                for line in s:
                    line = line.strip()
                    if line and not line.startswith("#"):
                        known_tests.add(line)
    if numSuites != 0:
        for d in dirs:
            if d not in known_tests:
                print("orphaned '%s'" % os.path.join(root, d))

