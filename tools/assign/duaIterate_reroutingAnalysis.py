#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
import glob
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa


def parse_args(args=None):
    USAGE = "Usage: " + sys.argv[0] + " [options]"
    parser = sumolib.options.ArgumentParser(
            usage=USAGE,
            description="Extract fraction of vehicles that changed their route per iteration")
    parser.add_argument("-d", "--base-directory", dest="baseDir", default=".",
                        help="Base directory of duaIterate run")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="Give more details for every iteration")
    options = parser.parse_args(args=args)
    return options


def main():
    options = parse_args()
    lastRoutes = {}  # vehID -> edges
    files = glob.glob(os.path.join(options.baseDir, "**/*.rou.alt.xml"))
    files = [(int(os.path.basename(os.path.dirname(f))), f) for f in files]
    for index, (step, file) in enumerate(sorted(files)):
        vehs = 0.0
        changed = []
        for veh in sumolib.xml.parse(file, 'vehicle'):
            vehs += 1
            last = int(veh.routeDistribution[0].last)
            lastEdges = veh.routeDistribution[0].route[last].edges
            if veh.id in lastRoutes and lastRoutes[veh.id] != lastEdges:
                changed.append(veh.id)
            lastRoutes[veh.id] = lastEdges
        numChanged = len(changed)
        if options.verbose:
            print("file=%s vehs=%s changed=%s frac=%s ids=%s" % (
                file, vehs, numChanged, numChanged / vehs, ' '.join(changed)))

        if index > 0 and not options.verbose:
            if vehs == 0:
                print("no vehicles in file '%s'" % file)
            else:
                print(numChanged / vehs)


##################
if __name__ == "__main__":
    main()
