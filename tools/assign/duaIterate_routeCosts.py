#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    duaIterate_routeCosts.py
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
47 Costs: count 1124, min 896.14 (veh0), max 1960.17 (veh1), mean 1355.41, Q1 1257.53, median 1325.86, Q3 1434.97
48 Costs: count 1124, min 896.47 (veh0), max 1993.74 (veh2), mean 1355.02, Q1 1257.32, median 1323.68, Q3 1434.09
49 Costs: count 1124, min 898.51 (veh3), max 1958.68 (veh1), mean 1355.00, Q1 1257.93, median 1323.39, Q3 1434.92

Implementation Note:
    edgeIDs are mapped to numbers in a numpy array to conserve memory
    (because strings have a large fixed memory overhead this reduces memory consumption by a factor of 10)
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import glob
import subprocess
from collections import namedtuple
import numpy as np
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.statistics import Statistics  # noqa

Route = namedtuple('Route', ['vehID', 'cost', 'index', 'used', 'depart', 'edges'])
StringDict = {}
ReverseStringDict = {}


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
        if i is None:
            return False
    return True


def stringToNumber(string):
    if string not in StringDict:
        StringDict[string] = len(StringDict)
    return StringDict[string]


def numberToString(n):
    if not ReverseStringDict:
        ReverseStringDict.update(((v, k) for k, v in StringDict.items()))
    return ReverseStringDict[n]


def load(baseDir, iterations, suffix="gz"):
    """iterations is an iterable that gives the iteration numberes to load
    """
    result = []
    files = glob.glob(os.path.join(baseDir, "**/*.rou.alt.%s" % suffix))
    files = [(int(os.path.basename(os.path.dirname(f))), f) for f in files]
    for step, file in sorted(files):
        if step not in iterations:
            continue
        print("loading step %s, file %s" % (step, file))
        result.append((step, loadRoutes(file)))
    return result


def loadRoutes(file):
    result = []
    for vehicle in sumolib.xml.parse(file, ['vehicle']):
        last = int(vehicle.routeDistribution[0].last)
        for ri, route in enumerate(vehicle.routeDistribution[0].route):
            edges = np.array(list(map(stringToNumber, route.edges.split())), dtype=np.uint32)
            result.append(Route(vehicle.id, float(route.cost), ri, ri == last, vehicle.depart, edges))
    return result


def filter(stepRoutes, origin=None, dest=None, via=None, forbidden=None, cutVia=False):
    """Filter given routes according to origin, destination, via-edges or forbidden edges
    If cutVia is set, the route will be truncated by the first and last via-edge and it's cost set to -1
    """
    if cutVia:
        if not via:
            print("ignoring cutVia because via is not set", sys.stderr)
        if len(via) == 1 and not origin and not dest:
            print("cannot cutVia because only a single edge is given without origin or dest", sys.stderr)
    result = []
    if origin:
        origin = stringToNumber(origin)
    if dest:
        dest = stringToNumber(dest)
    if via:
        via = list(map(stringToNumber, via))
    if forbidden:
        forbidden = list(map(stringToNumber, forbidden))
    for step, routes in stepRoutes:
        routes2 = []
        for r in routes:
            if origin and r.edges[0] != origin:
                continue
            if dest and r.edges[-1] != dest:
                continue
            if via or forbidden:
                edgeSet = set(r.edges)
                if forbidden and not edgeSet.isdisjoint(forbidden):
                    continue
                if via and not (edgeSet.issuperset(via) and hasSequence(r.edges, via)):
                    continue
            if cutVia and via:
                iStart = npindex(r.edges, via[0])
                if len(via) == 1:
                    if origin:
                        iStart = 0
                        iEnd = iStart
                    elif dest:
                        iEnd = r.edges.size - 1
                    else:
                        iEnd = iStart
                else:
                    iEnd = npindex(r.edges, via[-1])
                routes2.append(Route(r.vehID, -1, r.index, False, r.depart, r.edges[iStart:iEnd + 1]))
            else:
                routes2.append(r)
        result.append((step, routes2))
    return result


def costs(stepRoutes):
    """Compute statistics on costs computed by duarouter for the provided routes"""
    for step, routes in stepRoutes:
        s = Statistics("%s Costs" % step)
        for r in routes:
            s.add(r.cost, r.vehID)
        print(s)


def distinct(stepRoutes, verbose=True):
    """Count the number of occurences for each distinct route and return distinct routes"""
    result = []
    for step, routes in stepRoutes:
        routes2 = []
        counts = {}  # edges -> (count, info)
        for r in routes:
            etup = tuple(r.edges)
            if etup in counts:
                counts[etup][0] += 1
            else:
                # store information sufficient for identifying the route
                counts[etup] = [1, (r.vehID, r.index)]
                routes2.append(r)
        result.append((step, routes2))
        if verbose:
            print("Route usage counts in step %s (Count vehID routeIndex)" % step)
            rcounts = [(v, k) for k, v in counts.items()]
            for (count, info), edges in sorted(rcounts):
                print(count, info)
            print("Total distinct routes: %s" % len(counts))
    return result


def recompute_costs(baseDir, stepRoutes, netfile=None, tmpfileprefix="tmp"):
    """Recompute costs for all routes and all edgeData intervals"""
    DUAROUTER = sumolib.checkBinary('duarouter')
    tmp_input = tmpfileprefix + ".rou.gz"
    tmp_output = tmpfileprefix + "_out.rou.gz"
    tmp_output_alt = tmpfileprefix + "_out.rou.alt.gz"
    dumpfiles = glob.glob(os.path.join(baseDir, "**/dump_*.xml"))
    duarcfgs = glob.glob(os.path.join(baseDir, "**/*.duarcfg"))
    dumpDict = dict([(int(os.path.basename(os.path.dirname(f))), f) for f in dumpfiles])
    duarDict = dict([(int(os.path.basename(os.path.dirname(f))), f) for f in duarcfgs])
    result = []
    for step, routes in stepRoutes:
        dumpfile = dumpDict[step]
        with open(tmp_input, 'w') as tf:
            tf.write('<routes>\n')
            for interval in sumolib.xml.parse_fast(dumpfile, "interval", ["begin"]):
                for r in routes:
                    # write a routeID for easier plotting
                    vehID = "%s_%s_%s" % (r.vehID, r.index, interval.begin)
                    routeID = "%s_%s" % (r.vehID, r.index)
                    tf.write('    <vehicle id="%s" depart="%s">\n' % (vehID, interval.begin))
                    tf.write('        <route edges="%s"/>\n' % ' '.join(map(numberToString, r.edges)))
                    tf.write('        <param key="routeID" value="%s"/>\n' % routeID)
                    tf.write('    </vehicle>\n')
            tf.write('</routes>\n')

        args = [DUAROUTER, '-c', duarDict[step],
                '--weight-files', dumpfile,
                '--skip-new-routes',
                '--exit-times',
                '-r', tmp_input,
                '-o', tmp_output]
        if netfile:
            args += ['-n', netfile]
        subprocess.call(args)
        result.append((step, loadRoutes(tmp_output_alt)))
    return result
