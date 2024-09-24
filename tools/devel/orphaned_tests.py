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

# @file    orphaned_tests.py
# @author  Jakob Erdmann
# @date    2024-02-21


"""find test folders that are not mentioned in the respective test suite
"""
from __future__ import print_function
import sys
import os
from collections import defaultdict
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options():
    op = sumolib.options.ArgumentParser()
    op.add_argument("root", category="input", type=op.file,
                    help="root directory of tests to analyze")
    op.add_argument("--variant", action="store_true", default=False,
                    help="check whether the test is orphaned in the given suite variant")
    op.add_argument("--fix", action="store_true", default=False,
                    help="automatically append missing tests to test suites")
    return op.parse_args()


options = get_options()
numFixed = 0

for root, dirs, files in os.walk(options.root):
    numSuites = 0
    known_variant_tests = defaultdict(lambda : set())
    suites = []
    for fname in files:
        if fname.startswith("testsuite."):
            suites.append(fname)
            with open(os.path.join(root, fname)) as s:
                for line in s:
                    line = line.strip()
                    if line and not line.startswith("#"):
                        known_variant_tests[fname].add(line)
    if suites:
        orphaned = []
        mainSuite = sorted(suites)[0]
        if options.variant:
            mainSuite = options.variant
        known_tests = known_variant_tests[mainSuite]
        for d in dirs:
            if d in ["filter_files", "data"]:
                continue
            if d not in known_tests:
                orphaned.append(d)
                print("orphaned '%s'" % os.path.join(root, d))
        if orphaned and options.fix:
            numFixed += len(orphaned)
            # suite with shortest name is the main one
            with open(os.path.join(root, mainSuite), "a") as s:
                for t in orphaned:
                    print("\n", file=s)
                    print(t, file=s)

if options.fix:
    print("fixed %s orphaned tests" % numFixed)
