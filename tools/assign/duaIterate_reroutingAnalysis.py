#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
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

"""
Extract fraction of vehicles that changed their route in a duaIterate.py run by
parsing the rou.alt.xml files
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import glob
from collections import defaultdict
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa


def parse_args(args=None):
    USAGE = "Usage: " + sys.argv[0] + " [options]"
    parser = sumolib.options.ArgumentParser(
        usage=USAGE,
        description="Extract fraction of vehicles that changed their route per iteration")
    parser.add_argument("-d", "--base-directory", dest="baseDir", category="input", default=".", type=str,
                        help="Base directory of duaIterate run")
    parser.add_argument("-t", "--traveltime", action="store_true", default=False,
                        help="Analyze average and variance of chosen-route traveltime per iteration")
    parser.add_argument("-w", "--window", type=int, default=0,
                        help="Analyze average and variance of per-vehicle traveltime variance over the last WINDOW iterations")  # noqa
    parser.add_argument("--od", type=str, default=None,
                        help="Restrict analysis to a particular O-D relation (FROM,TO)")
    parser.add_argument("-r", "--route", type=str, default=None,
                        help="Restrict analysis to a particular route (space- or comma-separated edges)")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="Give more details for every iteration")
    options = parser.parse_args(args=args)
    if options.od is not None:
        parts = options.od.split(",")
        if len(parts) != 2 or not parts[0] or not parts[1]:
            sys.exit("Option --od requires FROM,TO")
        options.od = (parts[0], parts[1])
    if options.route is not None:
        options.route = " ".join(options.route.replace(",", " ").split())
    return options


def mean_and_var(values):
    if not values:
        return 0.0, 0.0
    mean = sum(values) / float(len(values))
    var = sum((v - mean) ** 2 for v in values) / float(len(values))
    return mean, var


def matches_od(veh, edges_list, od):
    orig, dest = od
    if edges_list and edges_list[0] == orig and edges_list[-1] == dest:
        return True
    if veh.hasAttribute("fromTaz") and veh.hasAttribute("toTaz"):
        return veh.fromTaz == orig and veh.toTaz == dest
    return False


def main(args=None):
    options = parse_args(args=args)
    lastRoutes = {}  # vehID -> edges
    costHistory = defaultdict(list)  # vehID -> [chosen route cost per iteration]
    files = glob.glob(os.path.join(options.baseDir, "**/*.rou.alt.xml"))
    files = [(int(os.path.basename(os.path.dirname(f))), f) for f in files]
    for index, (step, file) in enumerate(sorted(files)):
        vehs = 0.0
        changed = []
        costs = []
        iterVehIDs = []
        for veh in sumolib.xml.parse(file, 'vehicle'):
            last = int(veh.routeDistribution[0].last)
            chosenRoute = veh.routeDistribution[0].route[last]
            lastEdges = chosenRoute.edges
            edgeList = lastEdges.split()
            if options.od is not None and not matches_od(veh, edgeList, options.od):
                continue
            prevEdges = lastRoutes.get(veh.id)
            lastRoutes[veh.id] = lastEdges
            cost = float(chosenRoute.cost) if chosenRoute.hasAttribute("cost") else None
            if cost is not None:
                costHistory[veh.id].append(cost)
            if options.route is not None and lastEdges != options.route:
                continue
            vehs += 1
            iterVehIDs.append(veh.id)
            if prevEdges is not None and prevEdges != lastEdges:
                changed.append(veh.id)
            if cost is not None:
                costs.append(cost)
        numChanged = len(changed)
        frac = (numChanged / vehs) if vehs > 0 else 0.0
        extra = ""
        extraCols = []
        if options.traveltime:
            ttMean, ttVar = mean_and_var(costs)
            extra += " ttMean=%.2f ttVar=%.2f" % (ttMean, ttVar)
            extraCols.extend(["%.2f" % ttMean, "%.2f" % ttVar])
        if options.window > 0:
            vehVars = [
                mean_and_var(costHistory[vid][-options.window:])[1]
                for vid in iterVehIDs if vid in costHistory
            ]
            stabMean, stabVar = mean_and_var(vehVars)
            extra += " ttVarMean=%.2f ttVarVar=%.2f" % (stabMean, stabVar)
            extraCols.extend(["%.2f" % stabMean, "%.2f" % stabVar])
        normFile = file.replace("\\", "/")
        if options.verbose:
            print("file=%s vehs=%s changed=%s frac=%s%s ids=%s" % (
                normFile, vehs, numChanged, frac, extra, ' '.join(changed)))

        if not options.verbose and (index > 0 or extraCols):
            if vehs == 0:
                print("no vehicles in file '%s'" % normFile)
            elif extraCols:
                print(" ".join([str(frac)] + extraCols))
            else:
                print(frac)


##################
if __name__ == "__main__":
    main()
