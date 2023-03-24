#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
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
from sumolib.miscutils import parseTime, getFlowNumber  # noqa
import sumolib  # noqa


class SimObject:
    def __init__(self, obj_id, depart, objfrom, objto, edges, vtype):
        self.objid = obj_id
        self.depart = depart
        self.objfrom = objfrom
        self.objto = objto
        self.edges = edges
        self.vtype = vtype
        self.fromTaz = None
        self.toTaz = None


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("-r", "--route-files", dest="routefiles",
                         help="define the route file separated by comma (mandatory)")
    optParser.add_option("-o", "--output-file", dest="outfile",
                         help="define the output filename")
    optParser.add_option("--timeline-list", dest="timelinelist", type=str,
                         default="3600,200,200,200,200,200,200,200,200,200,200,200,200",
                         help="Define the interval duration and then the scaled percentage for each interval; "
                              "e.g. 200% of the current demand")
    optParser.add_option("--timeline-pair", dest="timelinepair", type=str,
                         default="7200,200;7200,200;7200,200;7200,200;7200,200;7200,200",
                         help="Define the timeline pairs (duration, scacled percentage)")
    optParser.add_option("--random", action="store_true", dest="random",
                         default=False, help="use a random seed to initialize the random number generator")
    optParser.add_option("-s", "--seed", type=int, dest="seed", default=42, help="random seed")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="tell me what you are doing")

    options, args = optParser.parse_known_args(args=args)

    if options.timelinelist:
        duration = float(options.timelinelist.split(",")[0])
        options.timelinelist = [[duration, float(i)] for i in options.timelinelist.split(",")[1:]]

    elif options.timelinepair:
        timelinelist = [x.split(',') for x in options.timelinepair.split(';')]
        options.timelinelist = []
        for data in timelinelist:
            options.timelinelist.append([float(x) for x in data])
        # options.timelinelist = list(map(float, templist))

    if not options.routefiles:
        optParser.print_help()
        sys.exit("--route-files missing")
    else:
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
            scale = periodMap[p]/100.
        elif depart < p and depart >= periodList[i-1]:
            scale = periodMap[p]/100.

    return scale


def writeObjs(totalList, outf):
    for elem in totalList:
        outf.write(elem.toXML(' '*4))


def scaleRoutes(options, outf):
    lastDepart = 0
    lastBegin = 0
    periodMap = {}
    accPeriod = 0
    periodList = []
    idMap = {}
    for d in options.timelinelist:
        accPeriod += d[0]
        periodList.append(accPeriod)
        periodMap[accPeriod] = d[1]

    # get all ids
    for routefile in options.routefiles:
        for elem in sumolib.xml.parse(routefile, ['vehicle', 'trip', 'flow', 'person', 'personFlow', 'vType']):
            idMap[elem.id] = None

    # scale the number of objs for each pre-defined interval
    for routefile in options.routefiles:
        currIndex = 0
        candidatsList = []
        for elem in sumolib.xml.parse(routefile, ['vehicle', 'trip', 'flow', 'person', 'personFlow', 'vType']):
            if elem.name == 'vType':
                outf.write(elem.toXML(' '*4))
            elif elem.name in ['flow', 'personFlow']:
                begin = parseTime(elem.begin)
                if begin < lastBegin:
                    sys.stderr.write("Unsorted departure %s for %s '%s'" % (
                        begin, elem.tag, elem.id))
                    lastBegin = begin
                scale = getScale(begin, periodList, periodMap)
                elem.number = str(int(getFlowNumber(elem) * scale))
                outf.write(elem.toXML(' '*4))
            else:
                depart = parseTime(elem.depart)
                if depart < lastDepart:
                    sys.stderr.write("Unsorted departure %s for %s '%s'" % (
                        depart, elem.tag, elem.id))
                    lastDepart = depart
                if depart < periodList[currIndex] and (currIndex == 0 or depart >= periodList[currIndex - 1]):
                    candidatsList.append(elem)
                else:
                    if currIndex < len(periodList):
                        if candidatsList:
                            totalList, idMap = getScaledObjList(periodMap, periodList, currIndex, candidatsList, idMap)
                            writeObjs(totalList, outf)
                            currIndex += 1
                            candidatsList = []
                            # check the current or the first object in the next period
                            if depart < periodList[currIndex] and depart >= periodList[currIndex - 1]:
                                candidatsList.append(elem)
                    else:
                        outf.write(elem.toXML(' '*4))
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
    main(get_options(sys.argv))
