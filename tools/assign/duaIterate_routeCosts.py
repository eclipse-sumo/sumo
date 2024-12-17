#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
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
# @date    2024-12-16

"""
Analyze the evolving travel time (cost) of routes over the iterations of
duaIterate.py based on edgeData output, rou.alt.xml and tripinfos
- user-defined routes
- occuring routes filtered by 
  - origin/destination
  - intermediate edges that must have been passed
  - edges that must have been avoided
- actual trip travel times

This module is meant to be loaded in an interpreter sessions to interrogate the data multiple times but only loading it
once.

Example sessions:

>>> import duaIterate_routeCosts as rc
>>> r = rc.load('.', range(47, 50))                  # load iteration numbers 47,48,49
>>> f = rc.filter(r, via=['531478184','25483415'])   # filter routes that pass both edges
47 Costs: count 1124, min 896.14 (126868_8), max 1960.17 (225725_1), mean 1355.41, Q1 1257.53, median 1325.86, Q3 1434.97
48 Costs: count 1124, min 896.47 (126868_8.1), max 1993.74 (225725_1), mean 1355.02, Q1 1257.32, median 1323.68, Q3 1434.09
49 Costs: count 1124, min 898.51 (126868_8.1), max 1958.68 (225725_1), mean 1355.00, Q1 1257.93, median 1323.39, Q3 1434.92

Implementation Note:
    edgeIDs are mapped to numbers in a numpy array to conserve memory 
    (because strings have a large fixed memory overhead this reduces memory consumption by a factor of 10)
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import glob
from collections import namedtuple
import numpy as np
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.statistics import Statistics

Route = namedtuple('Route', ['vehID', 'cost', 'used', 'edges'])
StringDict = {}

def npindex(array, x):
    i = np.where(array == x)
    if i[0].size > 0:
        return i[0][0]
    else:
        return None

def hasSequence(array, via):
    i = npindex(array, via[0])
    for edge in via[1:]:
        i = npindex(array, edge)
        if i == None:
            return False;
    return True

def stringToNumber(string):
    if string not in StringDict:
        StringDict[string] = len(StringDict)
    return StringDict[string]

def load(baseDir, iterations, suffix="gz"):
    """iterations is an iterable that gives the iteration numberes to load
    """
    iteration = set(iterations)
    result = []
    files = glob.glob(os.path.join(baseDir, "**/*.rou.alt.%s" % suffix))
    files = [(int(os.path.basename(os.path.dirname(f))), f) for f in files]
    for step, file in sorted(files):
        if step not in iterations:
            continue
        print("loading step %s, file %s" % (step, file))
        stepRoutes = []
        for vehicle in sumolib.xml.parse(file, ['vehicle']):
            last = int(vehicle.routeDistribution[0].last)
            for ri, route in enumerate(vehicle.routeDistribution[0].route):
                edges = np.array(list(map(stringToNumber, route.edges.split())), dtype=np.uint32)
                stepRoutes.append(Route(vehicle.id, float(route.cost), ri == last, edges))
        result.append((step, stepRoutes))
    return result


def filter(iterations, origin=None, dest=None, via=None, forbidden=None):
    result = []
    if origin:
        origin = stringToNumber(origin)
    if dest:
        dest = stringToNumber(dest)
    if via:
        via = list(map(stringToNumber, via))
    if forbidden:
        forbidden = list(map(stringToNumber, forbidden))
    for step, routes in iterations:
        routes2 = []
        for r in routes:
            if origin and r.edges[0] != origin:
                continue
            if dest and r.edges[-1] != dest:
                continue
            if via or forbidden:
                edgeSet = set(r.edges)
                if forbidden and not edgeSet.disjoint(forbidden):
                    continue
                if via and not (edgeSet.issuperset(via) and hasSequence(r.edges, via)):
                    continue
            routes2.append(r)
        result.append((step, routes2))
    return result

def costs(iterations):
    for step, routes in iterations:
        s = Statistics("%s Costs" % step)
        for r in routes:
            s.add(r.cost, r.vehID)
        print(s)


