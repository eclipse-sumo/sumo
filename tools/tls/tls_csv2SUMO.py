#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    tls_csv2SUMO.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2009-08-01
# @version $Id$

"""
Converts a csv-tls-description into one SUMO can read as additional file.
Format of the csv-file:

key;105
subkey;utopia
offset;0
(link;<LINK_NUMBER>;<FROMEDGE>|<FROMLANE>;[<TOEDGE>|<TOLANE>];0)+
(param;<NAME>;<VALUE>)+
(<LINK_NUMBER>;<PHASE_LIGHT>[;<PHASE_LIGHT>]+)+
(min;<PHASE_LENGTH>[;<PHASE_LENGTH>]+)*
time;<PHASE_LENGTH>[;<PHASE_LENGTH>]+
(max;<PHASE_LENGTH>[;<PHASE_LENGTH>]+)*


"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import argparse

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net

parser = argparse.ArgumentParser(description='Create tls xml def from csv.')
parser.add_argument('TLS_CSV', help='tls definition')
parser.add_argument('NET', help='sumo net file')
parser.add_argument(
    '-d', '--debug', action='store_true', help='print additional debug info')

args = parser.parse_args()


def computeLinkPhasesAndTimes(links2sigGrpPhase):
    global normTimes
    global defs
    global links2index
    phases = {}

    # compute all boundaries between phases
    for myKey in links2sigGrpPhase:
        phaseDef = links2sigGrpPhase[myKey]
        for i in range(0, len(phaseDef), 2):
            phases[int(phaseDef[i])] = True
    lastTime = 0
    newTimes = []
    sortPhases = []

    # sort the boundaries and compute the normTimes (here newTimes)) list
    for time in sorted(phases):
        if int(time) != 0:
            sortPhases.append(time)
        if int(time) > lastTime:
            newTimes.append(str(int(time) - lastTime))
        lastTime = int(time)
    normTimes = newTimes
    if args.debug:
        print ('normTimes', newTimes, file=sys.stderr)

    # create the phase string
    for myKey in links2sigGrpPhase:
        phaseDef = links2sigGrpPhase[myKey]
        currentPhaseIndex = 0
        newPhases = []
        sigGrpPhaseIndex = 1
        if args.debug:
            print ('phaseDef', phaseDef, 'len', len(phaseDef), file=sys.stderr)
        for currentPhase in sortPhases:
            if args.debug:
                print ('SGPIndex', sigGrpPhaseIndex, 'startCurrPhase', currentPhase,
                       'endCurrPhase',  phaseDef[sigGrpPhaseIndex + 1], file=sys.stderr)
            newPhases.append(phaseDef[sigGrpPhaseIndex])
            if int(phaseDef[sigGrpPhaseIndex + 1]) == currentPhase and sigGrpPhaseIndex + 2 < len(phaseDef):
                sigGrpPhaseIndex += 2
        if args.debug:
            print(myKey, newPhases, file=sys.stderr)
        links2index[myKey] = len(defs)
        defs.append(newPhases)


allTLS = args.TLS_CSV.split(",")
allMinTimes = []
allMaxTimes = []
allNormTimes = []
allDefs = []
allLinks = []
allParams = []
allKeys = []
allSubkeys = []
allOffsets = []
allLink2Indices = []


for tlsFile in allTLS:
    fd = open(tlsFile)
    key = None
    hasSigGrpPhase = False
    for l in fd:
        l = l.strip()
        if len(l) > 0 and l[0] == '#':
            continue
        v = l.split(";")
        if v[0] == "key":
            if key is not None:
                allMinTimes.append(minTimes)
                allMaxTimes.append(maxTimes)
                allNormTimes.append(normTimes)
                allDefs.append(defs)
                allLinks.append(links)
                allParams.append(params)
                allKeys.append(key)
                allSubkeys.append(subkey)
                allOffsets.append(offset)
                allLink2Indices.append(links2index)

            minTimes = []
            maxTimes = []
            normTimes = []
            defs = []
            links = []
            params = []
            # key = ""
            subkey = ""
            offset = 0
            links2index = {}
            links2sigGrpPhase = {}

            key = v[1]
        elif v[0] == "subkey":
            subkey = v[1]
        elif v[0] == "offset":
            offset = v[1]
        elif v[0] == "link":
            # linkNo = int(v[1])
            linkNo = v[1]
            fromDef = v[2]
            toDef = v[3]
            minor = v[4] == "1"
            links.append([fromDef, toDef, linkNo, minor])
        elif v[0] == "param":
            params.append([v[1], v[2]])
        elif v[0] == "min":
            minTimes = v[1:]
        elif v[0] == "max":
            maxTimes = v[1:]
        elif v[0] == "time":
            normTimes = v[1:]
        elif v[0] == "siggrpphase":
            links2sigGrpPhase[v[1]] = v[2:]
            hasSigGrpPhase = True
        else:
            if len(v) > 1:
                links2index[v[0]] = len(defs)
                defs.append(v[1:])

        pass

    if hasSigGrpPhase:
        computeLinkPhasesAndTimes(links2sigGrpPhase)

    if len(defs) > 0:
        links2index[-1] = len(defs)
        defs.append(['g'] * len(defs[0]))

    if key is not None:
        allMinTimes.append(minTimes)
        allMaxTimes.append(maxTimes)
        allNormTimes.append(normTimes)
        allDefs.append(defs)
        allLinks.append(links)
        allParams.append(params)
        allKeys.append(key)
        allSubkeys.append(subkey)
        allOffsets.append(offset)
        allLink2Indices.append(links2index)
    fd.close()

net1 = sumolib.net.readNet(args.NET)

print('<?xml version="1.0" encoding="UTF-8"?>\n<add>')
for keyIndex, key in enumerate(allKeys):
    minTimes = allMinTimes[keyIndex]
    maxTimes = allMaxTimes[keyIndex]
    normTimes = allNormTimes[keyIndex]
    defs = allDefs[keyIndex]
    links = allLinks[keyIndex]
    params = allParams[keyIndex]
    subkey = allSubkeys[keyIndex]
    offset = allOffsets[keyIndex]
    links2index = allLink2Indices[keyIndex]

    tls = net1._id2tls[key]
    noConnections = tls._maxConnectionNo + 1
    linkMap = [0] * noConnections
    laneMap = [None] * noConnections
    for tl_c in tls._connections:
        li = tl_c[0]  # incoming lane in our net
        lo = tl_c[1]  # outgoing lane in our net
        for l in links:
            valid = True
            if l[0].find('_') < 0:
                # edge only given
                if l[0] != li.getEdge().getID():
                    valid = False
            else:
                # lane given
                if l[0] != li.getID():
                    valid = False
            if l[1] != "":
                if l[1].find('_') < 0:
                    # edge only given
                    if l[1] != lo.getEdge().getID():
                        valid = False
                else:
                    # lane given
                    if l[1] != lo.getID():
                        valid = False
            if valid:
                linkMap[tl_c[2]] = l[2]
                laneMap[tl_c[2]] = (li, lo)
        if laneMap[tl_c[2]] is None:
            print("Warning: No link definition for connection (%s, %s)!. Using 'g' by default" % (
                li.getID(), lo.getID()), file=sys.stderr)
            linkMap[tl_c[2]] = -1
            laneMap[tl_c[2]] = (li, lo)

    nodes = set()
    for l in laneMap:
        if l:
            nodes.add(l[0].getEdge()._to)

    indices = {}
    for n in nodes:
        indices[n] = {}
        index = 0
        for i in n._incLanes:
            e = '_'.join(i.split("_")[:-1])
            l = i.split("_")[-1]
            if e in net1._crossings_and_walkingAreas:
                continue
            e = net1._id2edge[e]
            li = e._lanes[int(l)]
            for c in li._outgoing:
                indices[n][(li, c._toLane)] = index
                index = index + 1

    for l in range(0, len(linkMap)):
        if linkMap[l] not in links2index:
            print("Error: Link %s is not described (%s)!" % (
                l, linkMap[l]), file=sys.stderr)
            sys.exit()

    print('    <tlLogic id="' + key + '" type="static" programID="' +
          subkey + '" offset="' + offset + '">')
    for p in params:
        print('        <param key="' + p[0] + '" value="' + p[1] + '"/>')

    for i in range(0, len(normTimes)):
        state = ""
        for l in range(0, len(linkMap)):
            index = links2index[linkMap[l]]
            d = defs[index]
            if d[i] == 'r':
                state = state + "r"
            elif d[i] == 'y' or d[i] == 'a':
                state = state + "y"
            elif d[i] == 'g':
                state = state + "g"
            elif d[i] == 'o' or d[i] == 'x':
                state = state + "o"
            else:
                sys.stderr.write(
                    "missing value %s at %s (%s); setting to g\n" % (d[i], index, linkMap[l]))
                state = state + "g"
        for l1 in range(0, len(state)):
            if state[l1] == 'g':
                wait = False
                for l2 in range(0, len(state)):
                    if l1 == l2:
                        continue
                    if state[l2] != 'g' and state[l2] != 'G' and state[l2] != 'y' and state[l2] != 'Y':
                        continue
                    link1 = laneMap[l1]
                    link2 = laneMap[l2]
                    node1 = link1[0].getEdge()._to
                    node2 = link2[0].getEdge()._to
                    mustWait = node1 == node2
                    if mustWait:
                        mindices = indices[node1]
                        mprohs = node1._prohibits
                        mustWait = mprohs[mindices[link1]][
                            len(mprohs) - mindices[link2] - 1] == '1'
                    wait = wait or mustWait
                if not wait:
                    state = state[:l1] + 'G' + state[l1 + 1:]

        pd = '        <phase duration="' + normTimes[i] + '" '
        pd = pd + 'state="' + state + '"'
        if len(minTimes) == len(normTimes):
            pd = pd + ' minDur="' + minTimes[i] + '"'
        if len(maxTimes) == len(normTimes):
            pd = pd + ' maxDur="' + maxTimes[i] + '"'
        pd = pd + '/>'
        print(pd)
    print("    </tlLogic>")
print("</add>\n")
