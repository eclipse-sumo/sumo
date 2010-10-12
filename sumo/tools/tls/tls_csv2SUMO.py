#!/usr/bin/env python
"""
@file    tls_csv2SUMO.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id$

Converts a csv-tls-description into one SUMO can read as additional file.
Format of the csv-file:

key;105
subkey;utopia
offset;0
(link;<LINK_NUMBER>;<FROMEDGE>|<FROMLANE>;[<TOEDGE>|<TOLANE>];0)+
(<LINK_NUMBER>;<PHASE_LIGHT>[;<PHASE_LIGHT>]+)+
(min;<PHASE_LENGTH>[;<PHASE_LENGTH>]+)*
time;<PHASE_LENGTH>[;<PHASE_LENGTH>]+
(max;<PHASE_LENGTH>[;<PHASE_LENGTH>]+)*



Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import sys, os
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


if len(sys.argv)<2:
    print >> sys.stderr, "Call: tls_csv2SUMO.py <TLS_CSV> <NET>"
    sys.exit()


allTLS = sys.argv[1].split(",")
allMinTimes = []
allMaxTimes = []
allNormTimes = []
allDefs = []
allLinks = []
allKeys = []
allSubkeys = []
allOffsets = []
allLink2Indices = []


for tlsFile in allTLS:
    minTimes = []
    maxTimes = []
    normTimes = []
    defs = []
    links = []
    key = ""
    subkey = ""
    offset = 0
    links2index = {}

    fd = open(tlsFile)
    for l in fd:
        l = l.strip()
        if len(l)>0 and l[0]=='#':
            continue
        v = l.split(";")
        if v[0]=="key":
            key = v[1]
        elif v[0]=="subkey":
            subkey = v[1]
        elif v[0]=="offset":
            offset = v[1]
        elif v[0]=="link":
            linkNo = int(v[1])
            fromDef = v[2]
            toDef = v[3]
            minor = v[4]=="1"
            links.append( [ fromDef, toDef, linkNo, minor ] )
        elif v[0]=="min":
            minTimes = v[1:]
        elif v[0]=="max":
            maxTimes = v[1:]
        elif v[0]=="time":
            normTimes = v[1:]
        else:
            if len(v)>1:
                links2index[int(v[0])] = len(defs)
                defs.append(v[1:])
    fd.close()

    allMinTimes.append(minTimes)
    allMaxTimes.append(maxTimes)
    allNormTimes.append(normTimes)
    allDefs.append(defs)
    allLinks.append(links)
    allKeys.append(key)
    allSubkeys.append(subkey)
    allOffsets.append(offset)
    allLink2Indices.append(links2index)

parser = make_parser()
net1 = sumonet.NetReader()
parser.setContentHandler(net1)
parser.parse(sys.argv[2])
net1 = net1.getNet()

print "<add>"
for keyIndex, key in enumerate(allKeys):
    minTimes = allMinTimes[keyIndex]
    maxTimes = allMaxTimes[keyIndex]
    normTimes = allNormTimes[keyIndex]
    defs = allDefs[keyIndex]
    links = allLinks[keyIndex]
    subkey = allSubkeys[keyIndex]
    offset = allOffsets[keyIndex]
    links2index = allLink2Indices[keyIndex]

    tls = net1._id2tls[key]
    noConnections = tls._maxConnectionNo + 1
    linkMap = [0]*noConnections
    laneMap = [None]*noConnections
    for tl_c in tls._connections:
        li = tl_c[0] # incoming lane in our net
        lo = tl_c[1] # outgoing lane in our net
        for l in links:
            valid = True
            if l[0].find('_')<0:
                # edge only given
                if l[0]!=li.getEdge().getID():
                    valid = False
            else:
                # lane given
                if l[0]!=li.getID():
                    valid = False
            if l[1]!="":
                if l[1].find('_')<0:
                    # edge only given
                    if l[1]!=lo.getEdge().getID():
                        valid = False
                else:
                    # lane given
                    if l[1]!=lo.getID():
                        valid = False
            if valid:
                linkMap[tl_c[2]] = l[2]
                laneMap[tl_c[2]] = (li, lo)
#print linkMap

    nodes = set()
    for l in laneMap:
        if l:
            nodes.add(l[0].getEdge()._to)

    indices = {}
    for n in nodes:
        indices[n] = {}
        index = 0
        for i in n._incLanes:
            (e, l) = i.split("_")
            e = net1._id2edge[e]
            li = e._lanes[int(l)]
            for c in li._outgoing:
                lo = c[0]
                indices[n][(li, lo)] = index
                index = index + 1

    for l in range(0, len(linkMap)):
        if linkMap[l] not in links2index:
            print >> sys.stderr, "Link %s is not described (%s)" % (l, linkMap[l])
            sys.exit()

    print '    <tl-logic type="static" id="' + key + '" programID="' + subkey + '" offset="' + offset + '">'
    for i in range(0, len(normTimes)):
        state = ""
        for l in range(0, len(linkMap)):
            index = links2index[linkMap[l]]
            d = defs[index]
            if d[i]=='r':
                state = state + "r"
            elif d[i]=='y' or d[i]=='a':
                state = state + "y"
            elif d[i]=='g':
                state = state + "g"
            else:
                print "missing value; setting to g"
                state = state + "g"
        for l1 in range(0, len(state)):
            if state[l1]=='g':
                wait = False
                for l2 in range(0, len(state)):
                    if l1==l2:
                        continue
                    if state[l2]!='g' and state[l2]!='G' and state[l2]!='y' and state[l2]!='Y':
                        continue
                    link1 = laneMap[l1]
                    link2 = laneMap[l2]
                    node1 = link1[0].getEdge()._to
                    node2 = link2[0].getEdge()._to
                    mustWait = node1==node2
                    if mustWait:
                        mindices = indices[node1]
                        mprohs = node1._prohibits
                        mustWait = mprohs[mindices[link1]][len(mprohs)-mindices[link2]-1]=='1'
                    wait = wait or mustWait
                if not wait:
                    state = state[:l1] + 'G' + state[l1+1:]
             
        pd = '        <phase duration="' + normTimes[i] + '" '
        if len(minTimes)==len(normTimes):
            pd = pd + 'min_dur="' + minTimes[i] + '" '
        if len(maxTimes)==len(normTimes):
            pd = pd + 'max_dur="' + maxTimes[i] + '" '
        pd = pd + 'state="' + state + '"/>'
        print pd
    print "    </tl-logic>"
print "</add>"


    

