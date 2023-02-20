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

# @file    tlsCycleAdaptation.py
# @author  Yun-Pang Floetteroed
# @date    2017-05-10

"""
- The Webster's equation is used to optimize the cycle length
  and the green times of the traffic lights in a sumo network
  with a given route file.

- Traffic lights without traffic flows will not be optimized.
- PCE is used instead of the number of vehicles.

- If a lane group has green times in more than one phase,
  the respective flows will be equally divided into the corresponding
  phases for calculating the green splits.

- If the critical flow or the sum of the critical flows is larger than 1,
 the optimal cycle length will be set to 120 sec.

- Duration for yellow phase will be adjusted according to
  the defined option value

- Duration for all-red phase will be not adjusted.
"""

from __future__ import absolute_import
from __future__ import print_function

import collections

import sumolib


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("-n", "--net-file", dest="netfile", required=True,
                         help="define the net file (mandatory)")
    optParser.add_option("-o", "--output-file", dest="outfile",
                         default="tlsAdaptation.add.xml", help="define the output filename")
    optParser.add_option("-r", "--route-files", dest="routefiles", required=True,
                         help="define the route file separated by comma (mandatory)")
    optParser.add_option("-b", "--begin", dest="begin", type=int,
                         default=0, help="begin time of the optimization period with unit second")
    optParser.add_option("-y", "--yellow-time", dest="yellowtime", type=int,
                         default=4, help="yellow time")
    optParser.add_option("-a", "--all-red", dest="allred", type=int,
                         default=0, help="all-red time per cycle")
    optParser.add_option("-l", "--lost-time", dest="losttime", type=int,
                         default=4, help="lost time for start-up and clearance in each phase")
    optParser.add_option("-g", "--min-green", dest="mingreen", type=int,
                         default=4, help=" minimal green time when there is no traffic volume")
    optParser.add_option("--green-filter-time", dest="greenFilter", type=int, default=0,
                         help="when computing critical flows, do not count phases with a green time below INT")
    optParser.add_option("--min-cycle", dest="mincycle", type=int,
                         default=20, help="minimal cycle length")
    optParser.add_option("--max-cycle", dest="maxcycle", type=int,
                         default=120, help="maximal cycle length")
    optParser.add_option("-e", "--existing-cycle", dest="existcycle", action="store_true",
                         default=False, help="use the existing cycle length")
    optParser.add_option("--write-critical-flows", dest="write_critical_flows", action="store_true",
                         default=False, help="print critical flows for each tls and phase")
    optParser.add_option("-p", "--program", dest="program", default="a",
                         help="save new definitions with this program id")
    optParser.add_option("-H", "--saturation-headway", dest="satheadway", type=float, default=2,
                         help="saturation headway in seconds for calculating hourly saturation flows")
    optParser.add_option("-R", "--restrict-cyclelength", dest="restrict", action="store_true",
                         default=False, help="restrict the max. cycle length as the given one")
    optParser.add_option("-u", "--unified-cycle", dest="unicycle", action="store_true", default=False,
                         help="use the calculated max cycle length as the cycle length for all intersections")
    optParser.add_option("--sorted", action="store_true", default=False,
                         help="assume the route file is sorted (aborts reading earlier)")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    return optParser.parse_args(args=args)


def getEdges(veh, net, routes):
    if isinstance(veh.route, str):
        edges = routes[veh.route]
    else:
        edges = veh.route[0].edges
    return [net.getEdge(e) for e in edges.split()]


def getRoutes(files):
    route_dict = {}
    for file in files:
        for route in sumolib.output.parse(file, 'route'):
            route_dict[route.id] = route.edges
    return route_dict


def getFlows(net, routeFiles, tlsList, begin, verbose, isSorted=False):
    tlsFlowsMap = {}
    end = begin + 3600
    for tls in tlsList:
        tlsFlowsMap[tls._id] = collections.defaultdict(lambda: collections.defaultdict(int))
    route_dict = getRoutes(routeFiles.split(','))
    for file in routeFiles.split(','):
        if verbose:
            print("parsing route file:", file)
        triggered = parsed = 0
        for veh in sumolib.output.parse(file, 'vehicle'):
            if veh.depart == "triggered":
                triggered += 1
                continue
            if sumolib.miscutils.parseTime(veh.depart) >= end:
                if isSorted:
                    break
                continue
            if sumolib.miscutils.parseTime(veh.depart) >= begin:
                edgeList = getEdges(veh, net, route_dict)
                for idx, edge in enumerate(edgeList):
                    tls = None if edge.getToNode().getType() in ("rail_crossing", "rail_signal") else edge.getTLS()
                    if tls and idx < len(edgeList) - 1:
                        # c: [[inLane, outLane, linkNo],[],..]
                        for c in tls.getConnections():
                            inEdge = c[0].getEdge()
                            outEdge = c[1].getEdge()
                            if inEdge == edge and outEdge == edgeList[idx + 1]:
                                pce = 1.
                                if veh.type == "bicycle":
                                    pce = 0.2
                                elif veh.type in ["moped", "motorcycle"]:
                                    pce = 0.5
                                elif veh.type in ["truck", "trailer", "bus", "coach"]:
                                    pce = 3.5
                                tlsFlowsMap[tls.getID()][inEdge.getID() + " " + outEdge.getID()][c[2]] += pce
                                parsed += 1
        if triggered > 0:
            print("Warning: Ignored %s triggered vehicles in %s." % (triggered, file))
        if parsed == 0:
            print("Warning: No vehicles parsed from %s." % file)
        elif verbose:
            print("Parsed %s vehicles from %s." % (parsed, file))
    # remove the doubled counts
    connFlowsMap = {}
    for t in tlsList:
        connFlowsMap[t.getID()] = {}
        for subRoute in tlsFlowsMap[t.getID()]:
            totalConns = len(tlsFlowsMap[t.getID()][subRoute])
            for conn in tlsFlowsMap[t.getID()][subRoute]:
                tlsFlowsMap[t.getID()][subRoute][conn] /= totalConns
                connFlowsMap[t.getID()][conn] = tlsFlowsMap[t.getID()][subRoute][conn]

        # remove the redundant connection flows
        connFlowsMap = removeRedundantFlows(t, connFlowsMap)

    return connFlowsMap


def getEffectiveTlsList(tlsList, connFlowsMap, verbose):
    effectiveTlsList = []
    for tl in tlsList:
        if len(tl.getPrograms()) == 0:
            continue
        valid = True
        for program in tl.getPrograms().values():
            for phase in program.getPhases():
                if len(phase.state) > len(tl.getConnections()):
                    print("Skipping TLS '%s' due to unused states (%s states, %s connections)" % (
                        tl.getID(), len(phase.state), len(tl.getConnections())))
                    valid = False
                    break
        if valid:
            for conn in connFlowsMap[tl.getID()]:
                if connFlowsMap[tl.getID()][conn] > 0:
                    effectiveTlsList.append(tl)
                    break
    return effectiveTlsList


def removeRedundantFlows(t, connFlowsMap):
    # if two or more intersections share the lane-lane connection indices together,
    # the redundant connection flows will set to zero.
    connsList = t.getConnections()
    connsList = sorted(connsList, key=lambda connsList: connsList[2])
    redundantConnsList = []
    identical = True
    for c1 in connsList:
        for c2 in connsList:
            if c1[2] != c2[2]:
                if c1[1]._edge == c2[0]._edge:
                    identical = identityCheck(c1[0]._edge, c2[0]._edge._incoming, identical)
                    if identical:
                        for toEdge in c2[0]._edge._outgoing:
                            for c in c2[0]._edge._outgoing[toEdge]:
                                if c._tlLink not in redundantConnsList:
                                    redundantConnsList.append(c._tlLink)
                    else:
                        for conn_1 in c1[0]._edge._outgoing[c2[0]._edge]:
                            if conn_1._direction == 's':
                                for toEdge in c2[0]._edge._outgoing:
                                    for conn_2 in c2[0]._edge._outgoing[toEdge]:
                                        if conn_2._tlLink not in redundantConnsList:
                                            redundantConnsList.append(conn_2._tlLink)
    for conn in redundantConnsList:
        if conn in connFlowsMap[t._id]:
            connFlowsMap[t._id][conn] = 0.
    return connFlowsMap


def identityCheck(e1, incomingLinks, identical):
    for i in incomingLinks:
        if i != e1:
            identical = False
            break
    return identical


def getLaneGroupFlows(tl, connFlowsMap, phases, greenFilter):
    connsList = tl.getConnections()
    groupFlowsMap = {}  # i(phase): duration, laneGroup1, laneGroup2, ...
    connsList = sorted(connsList, key=lambda connsList: connsList[2])

    # check if there are shared lane groups, i.e. some lane groups have only "g" (no "G")
    ownGreenConnsList = []
    for i, p in enumerate(phases):
        for j, control in enumerate(p.state):
            if control == "G" and j not in ownGreenConnsList:
                ownGreenConnsList.append(j)
    yellowRedTime = 0
    greenTime = 0
    currentLength = 0
    phaseLaneIndexMap = collections.defaultdict(list)
    for i, p in enumerate(phases):
        currentLength += p.duration
        if 'G' in p.state and 'y' not in p.state and p.duration >= greenFilter:
            greenTime += p.duration
            groupFlowsMap[i] = [p.duration]
            groupFlows = 0
            laneIndexList = []
            for j, control in enumerate(p.state):
                inEdge = connsList[j][0]._edge._id
                if j == 0:
                    exEdge = inEdge
                if (inEdge == exEdge and control == 'G') or (inEdge == exEdge and
                                                             control == 'g' and j not in ownGreenConnsList):
                    if j in connFlowsMap[tl._id]:
                        groupFlows += connFlowsMap[tl._id][j]
                    if connsList[j][0].getIndex() not in laneIndexList:
                        laneIndexList.append(connsList[j][0].getIndex())

                if exEdge != inEdge or j == len(p.state) - 1:
                    if laneIndexList:
                        phaseLaneIndexMap[i].append(laneIndexList)
                        groupFlowsMap[i].append(groupFlows)

                    laneIndexList = []
                    groupFlows = 0
                    if control == "G":
                        if j in connFlowsMap[tl._id]:
                            groupFlows = connFlowsMap[tl._id][j]
                            if connsList[j][0].getIndex() not in laneIndexList:
                                laneIndexList.append(connsList[j][0].getIndex())
                exEdge = inEdge
        elif 'G' not in p.state and 'g' in p.state and 'y' not in p.state and 'r' not in p.state:
            print("Check: only g for all connections:%s in phase %s" % (tl._id, i))
        elif ('G' not in p.state and 'g' not in p.state) or ('G' not in p.state and 'y' in p.state and 'r' in p.state):
            yellowRedTime += int(p.duration)
        if options.verbose and i in groupFlowsMap:
            print("phase: %s" % i)
            print("group flows: %s" % groupFlowsMap[i])
            print("The used lanes: %s" % phaseLaneIndexMap[i])
    if options.verbose:
        print("the current cycle length:%s sec" % currentLength)
    return groupFlowsMap, phaseLaneIndexMap, currentLength


def getMaxOptimizedCycle(groupFlowsMap, phaseLaneIndexMap, currentLength, cycleList, options):
    lostTime = len(groupFlowsMap) * options.losttime + options.allred
    satFlows = 3600. / options.satheadway
    # calculate the critical flow ratios and the respective sum
    criticalFlowRateMap = {}
    for i in groupFlowsMap:   # [duration. groupFlow1, groupFlow2...]
        criticalFlowRateMap[i] = 0.
        maxFlow = 0
        index = None
        if len(groupFlowsMap[i][1:]) > 0:
            for j, f in enumerate(groupFlowsMap[i][1:]):
                if f >= maxFlow:
                    maxFlow = f
                    index = j
            criticalFlowRateMap[i] = (maxFlow / float((len(phaseLaneIndexMap[i][index])))) / satFlows
        else:
            criticalFlowRateMap[i] = 0.
    sumCriticalFlows = sum(criticalFlowRateMap.values())

    if options.existcycle:
        optCycle = currentLength
    elif sumCriticalFlows >= 1.:
        optCycle = options.maxcycle
        if options.verbose:
            print("Warning: the sum of the critical flows >= 1:%s" % sumCriticalFlows)
    else:
        optCycle = int(round((1.5 * lostTime + 5.) / (1. - sumCriticalFlows)))

    if not options.existcycle and optCycle < options.mincycle:
        optCycle = options.mincycle
    elif not options.existcycle and optCycle > options.maxcycle:
        optCycle = options.maxcycle

    cycleList.append(optCycle)

    return cycleList


def optimizeGreenTime(tl, groupFlowsMap, phaseLaneIndexMap, currentLength, options):
    lostTime = len(groupFlowsMap) * options.losttime + options.allred
    satFlows = 3600. / options.satheadway
    # calculate the critical flow ratios and the respective sum
    criticalFlowRateMap = {}
    for i in groupFlowsMap:   # [duration. groupFlow1, groupFlow2...]
        criticalFlowRateMap[i] = 0.
        maxFlow = 0
        index = None
        if len(groupFlowsMap[i][1:]) > 0:
            for j, f in enumerate(groupFlowsMap[i][1:]):
                if f >= maxFlow:
                    maxFlow = f
                    index = j
            criticalFlowRateMap[i] = (maxFlow / float((len(phaseLaneIndexMap[i][index])))) / satFlows
        else:
            criticalFlowRateMap[i] = 0.
    sumCriticalFlows = sum(criticalFlowRateMap.values())
    if options.write_critical_flows:
        print(tl.getID(), criticalFlowRateMap)

    if options.existcycle:
        optCycle = currentLength
    elif sumCriticalFlows >= 1.:
        optCycle = options.maxcycle
        if options.verbose:
            print("Warning: the sum of the critical flows >= 1:%s" % sumCriticalFlows)
    else:
        optCycle = int(round((1.5 * lostTime + 5.) / (1. - sumCriticalFlows)))

    if not options.existcycle and optCycle < options.mincycle:
        optCycle = options.mincycle
    elif not options.existcycle and optCycle > options.maxcycle:
        optCycle = options.maxcycle

    # calculate the green time for each critical group
    effGreenTime = optCycle - lostTime
    totalLength = len(groupFlowsMap)*options.yellowtime + options.allred
    minGreenPhasesList = []
    adjustGreenTimes = 0
    totalGreenTimes = 0
    subtotalGreenTimes = 0
    for i in criticalFlowRateMap:
        groupFlowsMap[i][0] = effGreenTime * \
            (criticalFlowRateMap[i] / sum(criticalFlowRateMap.values())) - options.yellowtime + options.losttime
        groupFlowsMap[i][0] = int(round(groupFlowsMap[i][0]))
        totalGreenTimes += groupFlowsMap[i][0]
        if groupFlowsMap[i][0] < options.mingreen:
            groupFlowsMap[i][0] = options.mingreen
            minGreenPhasesList.append(i)
        else:
            subtotalGreenTimes += groupFlowsMap[i][0]
        totalLength += groupFlowsMap[i][0]

    # adjust the green times if minimal green times are applied for keeping the defined maximal cycle length.
    if minGreenPhasesList and totalLength > options.maxcycle and options.restrict:
        totalLength = len(groupFlowsMap)*options.yellowtime + options.allred
        if options.verbose:
            print("Re-allocate the green splits!")
        adjustGreenTimes = totalGreenTimes - len(minGreenPhasesList) * options.mingreen
        for i in groupFlowsMap:
            if i not in minGreenPhasesList:
                groupFlowsMap[i][0] = int(round((groupFlowsMap[i][0] / float(subtotalGreenTimes)) * adjustGreenTimes))
            totalLength += groupFlowsMap[i][0]

    if options.unicycle and totalLength != optCycle:
        diff = optCycle - totalLength
        secs_to_distribute = [int(diff / abs(diff))] * abs(diff)
        keys = list(groupFlowsMap.keys())
        for i, s in enumerate(secs_to_distribute):
            groupFlowsMap[keys[i % len(groupFlowsMap)]][0] += s

    if options.verbose:
        totalLength = len(groupFlowsMap)*options.yellowtime + options.allred
        for i in groupFlowsMap:
            totalLength += groupFlowsMap[i][0]
            print("Green time for phase %s: %s" % (i, groupFlowsMap[i][0]))
        print("The optimal cycle length:%s\n" % totalLength)

    return groupFlowsMap


def main(options):
    net = sumolib.net.readNet(options.netfile, withPrograms=True, withPedestrianConnections=True)
    tlsList = net.getTrafficLights()

    if options.verbose:
        print("the total number of tls: %s" % len(tlsList))
    print("Begin time:%s" % options.begin)
    # get traffic flows for each connection at each TL
    connFlowsMap = getFlows(net, options.routefiles, tlsList, options.begin, options.verbose, options.sorted)

    # remove the tls where no traffic volumes exist
    effectiveTlsList = getEffectiveTlsList(tlsList, connFlowsMap, options.verbose)

    with open(options.outfile, 'w') as outf:
        sumolib.xml.writeHeader(outf, root="additional", options=options)
        if len(effectiveTlsList) > 0:
            if options.unicycle:
                cycleList = []
                if options.verbose:
                    print("Firstly only calculate the maximal optimized cycle length! ")
                for tl in effectiveTlsList:
                    if options.verbose:
                        print("tl-logic ID: %s" % tl._id)
                    programs = tl.getPrograms()
                    for pro in programs:
                        phases = programs[pro].getPhases()

                        # get the connection flows and group flows
                        groupFlowsMap, phaseLaneIndexMap, currentLength = getLaneGroupFlows(tl, connFlowsMap, phases, 0)

                        # only optimize the cycle length
                        cycleList = getMaxOptimizedCycle(groupFlowsMap, phaseLaneIndexMap,
                                                         currentLength, cycleList, options)

                options.maxcycle = max(cycleList)
                options.mincycle = max(cycleList)
                options.restrict = True
                if options.verbose:
                    print("The maximal optimized cycle length is %s." % max(cycleList))
                    print(" It will be used for calculating the green splits for all intersections.")

            # calculate the green splits; the optimal length will be also calculate if options.unicycle is set as false.
            for tl in effectiveTlsList:
                if options.verbose:
                    print("tl-logic ID: %s" % tl._id)
                programs = tl.getPrograms()
                for pro in programs:
                    phases = programs[pro].getPhases()

                    # get the connection flows and group flows
                    groupFlowsMap, phaseLaneIndexMap, currentLength = getLaneGroupFlows(
                        tl, connFlowsMap, phases, options.greenFilter)

                    # optimize the cycle length and calculate the respective green splits
                    groupFlowsMap = optimizeGreenTime(tl, groupFlowsMap, phaseLaneIndexMap, currentLength, options)

                # write output
                outf.write('    <tlLogic id="%s" type="%s" programID="%s" offset="%i">\n' %
                           (tl._id, programs[pro]._type, options.program, programs[pro]._offset))

                phases = programs[pro].getPhases()
                for i, p in enumerate(phases):
                    duration = p.duration
                    if i in groupFlowsMap:
                        duration = groupFlowsMap[i][0]
                    # the yellow phase
                    elif 'y' in p.state and 'r' in p.state:
                        duration = options.yellowtime
                    else:
                        print("Duration for Phase %s is from the input file." % i)
                    outf.write('        <phase duration="%s" state="%s"/>\n' % (duration, p.state))
                outf.write('    </tlLogic>\n')
        else:
            print("There are no flows at the given intersections. No green time optimization is done.")
        outf.write('</additional>\n')


if __name__ == "__main__":
    options = get_options()
    main(options)
