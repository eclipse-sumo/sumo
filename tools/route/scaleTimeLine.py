#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    scaleTimeLine.py
# @author  Yun-Pang Floetteroed
# @author  Jakob Erdmann
# @date    2022-04-19

"""
- write a new route file with fewer or more vehicles/flows depending on
  the given percentages and the sorted route/trip file(s)

- if more than one route file are given, all outputs will be stored in
  one output file. It may need to sort this file by departure time.
"""
from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import copy
import random

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.miscutils import parseTime, getFlowNumber, intIfPossible  # noqa
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_argument("-r", "--route-files", dest="routefiles", category="input", type=ap.file_list,
                    required=True, help="define the route file separated by comma (mandatory)")
    ap.add_argument("-o", "--output-file", dest="outfile", category="output", type=ap.file,
                    help="define the output filename")
    ap.add_argument("--timeline-list", dest="timelinelist", type=str,
                    # TGw2_PKW from https://sumo.dlr.de/docs/Demand/Importing_O/D_Matrices.html#daily_time_lines
                    # multiplied by 10 (suitable for using with peak-hour-traffic and tools/route/route_1htoday.py
                    default="3600,8,5,4,3,4,12,45,74,66,52,50,50,52,53,56,67,84,86,74,50,39,30,21,16",
                    help="Define the interval duration and then the scaled percentage for each interval; "
                    "e.g. 200 percent of the current demand")
    ap.add_argument("--timeline-pair", dest="timelinepair", type=str,
                    help="Define the timeline pairs (duration, scacled percentage)")
    ap.add_argument("--random", action="store_true", dest="random", category="random",
                    default=False, help="use a random seed to initialize the random number generator")
    ap.add_argument("-s", "--seed", type=int, dest="seed", category="random", default=42, help="random seed")
    ap.add_argument("-v", "--verbose", dest="verbose", action="store_true",
                    default=False, help="tell me what you are doing")
    options = ap.parse_args(args=args)

    if options.timelinepair:
        pairs = [x.split(',') for x in options.timelinepair.split(';')]
        options.timelinelist = []
        for d, s in pairs:
            options.timelinelist.append([float(d), float(s)])
    else:
        duration = float(options.timelinelist.split(",")[0])
        options.timelinelist = [[duration, float(i)] for i in options.timelinelist.split(",")[1:]]

    options.routefiles = options.routefiles.split(',')
    if not options.outfile:
        options.outfile = options.routefiles[0][:-4] + "_scaled.rou.xml"
    return options


def getScaledObjList(periodMap, periodList, currIndex, candidatsList, idMap):
    scale = periodMap[periodList[currIndex]]/100.
    sampleSize = int((scale - 1.) * len(candidatsList))
    selectedList = random.choices(candidatsList, k=abs(sampleSize))
    selectedList = [copy.deepcopy(o) for o in selectedList]
    if scale >= 1.:
        selectedList, idMap = changeIDs(selectedList, idMap)
        totalList = candidatsList + selectedList
    else:
        idList = [i.id for i in selectedList]
        totalList = [i for i in candidatsList if i.id not in idList]

    totalList = sorted(totalList, key=lambda simobject: simobject.depart)

    return totalList, idMap


def changeIDs(selectedList, idMap):
    selectedList = sorted(selectedList, key=lambda simobject: simobject.id)
    for obj in selectedList:
        # get last cloneId
        if idMap[obj.id]:
            cloneIdx = idMap[obj.id] + 1
        else:
            cloneIdx = 1
        idMap[obj.id] = cloneIdx
        obj.id = obj.id + '#' + str(cloneIdx)        # e.g. '123' --> '123#1' --> '123#1#1'

    return selectedList, idMap


def getScale(depart, periodList, periodMap):
    scale = 1.
    for i, p in enumerate(periodList):
        if i == 0 and depart < p:
            scale = periodMap[p] / 100.
        elif depart < p and depart >= periodList[i - 1]:
            scale = periodMap[p] / 100.

    return scale


def writeObjs(totalList, outf):
    for elem in totalList:
        outf.write(elem.toXML(' '*4))


def scaleRoutes(options, outf):
    periodMap = {}
    accPeriod = 0
    periodList = []
    idMap = {}
    for duration, scale in options.timelinelist:
        accPeriod += duration
        periodList.append(accPeriod)
        periodMap[accPeriod] = scale

    # get all ids
    for routefile in options.routefiles:
        for elem in sumolib.xml.parse(routefile, ['vehicle', 'trip', 'flow', 'person', 'personFlow', 'vType']):
            idMap[elem.id] = None

    # scale the number of objs for each pre-defined interval
    for routefile in options.routefiles:
        lastDepart = 0
        currIndex = 0
        candidatsList = []
        periodBegin = 0
        periodEnd = periodList[currIndex]
        for elem in sumolib.xml.parse(routefile, ['vehicle', 'trip', 'flow', 'person', 'personFlow', 'vType']):
            if elem.name == 'vType':
                outf.write(elem.toXML(' ' * 4))
            elif elem.name in ['flow', 'personFlow']:
                begin = parseTime(elem.begin)
                if begin < lastDepart:
                    sys.stderr.write("Unsorted departure %s for %s '%s'" % (
                        begin, elem.tag, elem.id))
                    lastDepart = begin
                scale = getScale(begin, periodList, periodMap)
                if elem.hasAttribute("number"):
                    elem.number = str(int(getFlowNumber(elem) * scale))
                elif elem.hasAttribute("period"):
                    if "exp" in elem.period:
                        rate = float(elem.period[4:-2])
                        elem.period = 'exp(%s)' % rate * scale
                    else:
                        elem.period = float(elem.period) / scale
                outf.write(elem.toXML(' ' * 4))
            else:
                depart = parseTime(elem.depart)
                elem.depart = intIfPossible(depart)
                if depart < lastDepart:
                    sys.stderr.write("Unsorted departure %s for %s '%s'" % (
                        depart, elem.tag, elem.id))
                    lastDepart = depart
                if depart >= periodBegin and depart < periodEnd:
                    candidatsList.append(elem)
                else:
                    # write old period and start new period
                    if candidatsList:
                        totalList, idMap = getScaledObjList(periodMap, periodList, currIndex, candidatsList, idMap)
                        writeObjs(totalList, outf)
                        candidatsList.clear()
                    while currIndex + 1 < len(periodList):
                        currIndex += 1
                        periodBegin = periodEnd
                        periodEnd = periodList[currIndex]
                        if depart >= periodBegin and depart < periodEnd:
                            candidatsList.append(elem)
                            break
        if candidatsList:
            totalList, idMap = getScaledObjList(periodMap, periodList, currIndex, candidatsList, idMap)
            writeObjs(totalList, outf)


def main(options):
    if not options.random:
        random.seed(options.seed)
    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", options=options)  # noqa
        scaleRoutes(options, outf)
        outf.write('</routes>\n')
    outf.close()


if __name__ == "__main__":
    main(get_options(sys.argv[1:]))
