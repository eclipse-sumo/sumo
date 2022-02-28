#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    duaIterate_reroutingAnalysis.py
# @author  Jakob Erdmann
# @date    2022-02-28

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import re
import glob
from optparse import OptionParser
from collections import defaultdict
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib


def parse_args(args=None):
    USAGE = "Usage: " + sys.argv[0] + " [options]"
    parser = sumolib.options.ArgumentParser(description="Extract fraction of vehicls that changed their route per iteration")
    parser.add_argument("-d", "--base-directory", dest="baseDir", default=".",
                        help="Base directory of duaIterate run")
    options = parser.parse_args(args=args)
    return options


def main():
    options = parse_args()
    lastRoutes = {}  # vehID -> edges
    for step, file in enumerate(sorted(glob.glob(os.path.join(options.baseDir, "**/*.rou.alt.xml")))):
        vehs = 0.0
        changed = 0.0
        for veh in sumolib.xml.parse(file, 'vehicle'):
            vehs += 1
            last = int(veh.routeDistribution[0].last)
            lastEdges = veh.routeDistribution[0].route[last].edges
            if veh.id in lastRoutes and lastRoutes[veh.id] != lastEdges:
                changed += 1;
            lastRoutes[veh.id] = lastEdges
        #print("file=%s vehs=%s changed=%s frac=%s" % (file, vehs, changed, changed / vehs))
        if step > 0:
            if vehs == 0:
                print("no vehicles in file '%s'" % file)
            else:
                print(changed / vehs)


##################
if __name__ == "__main__":
    main()
