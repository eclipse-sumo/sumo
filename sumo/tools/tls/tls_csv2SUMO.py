#!/usr/bin/env python
"""
@file    tls_csv2SUMO.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id: tls_csv2SUMO.py 6858 2009-02-25 13:27:57Z behrisch $

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


minTimes = []
maxTimes = []
normTimes = []
defs = []
links = []
key = ""
subkey = ""
offset = 0

fd = open(sys.argv[1])
for l in fd:
    l = l.strip()
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
        defs.append(v[1:])
fd.close()

parser = make_parser()
net1 = sumonet.NetReader()
parser.setContentHandler(net1)
parser.parse(sys.argv[2])
net1 = net1.getNet()

tls = net1._id2tls[key]
noConnections = tls._maxConnectionNo + 1
linkMap = [0]*noConnections
minorV = [False]*noConnections
mapCount = [0]*noConnections
for tl_c in tls._connections:
    ei = tl_c[0] # incoming lane in our net
    eo = tl_c[1] # outgoing lane in our net
    for l in links:
        valid = True
        if l[0].find('_')<0:
            # edge only given
            if l[0]!=ei._edge._id:
                valid = False
        else:
            # lane given
            if l[0]!=ei.getID():
                valid = False
        if l[1]!="":
            if l[1].find('_')<0:
                # edge only given
                if l[1]!=eo._edge._id:
                    valid = False
            else:
                # lane given
                if l[1]!=eo.getID():
                    valid = False
        if valid:
            linkMap[tl_c[2]] = l[2]
            minorV[tl_c[2]] = l[3]
            mapCount[tl_c[2]] = mapCount[tl_c[2]] + 1

for i in range(0, len(normTimes)):
    for l in range(0, len(linkMap)):
        if mapCount[l]==0:
            print >> sys.stderr, "Error: Link " + str(l) + " is not described."
            sys.exit()

print "<add>"
print "    <tl-logic type=\"static\">"
print "        <key>" + key + "</key>"
print "        <subkey>" + subkey + "</subkey>"
print "        <phaseno>" + str(len(normTimes)) + "</phaseno>"
print "        <offset>" + offset + "</offset>"
for i in range(0, len(normTimes)):
    phase = ""
    brake = ""
    yellow = ""
    for l in range(0, len(linkMap)):
        d = defs[linkMap[l]-1]
        if d[i]=='r':
            phase = "0" + phase
            brake = "1" + brake
            yellow = "0" + yellow
        elif d[i]=='y' or d[i]=='a':
            phase = "0" + phase
            brake = "1" + brake
            yellow = "1" + yellow
        elif d[i]=='g':
            phase = "1" + phase
            brake = "0" + brake
            yellow = "0" + yellow
        else:
            print >> sys.stderr, "missing value; setting to g"
            phase = "1" + phase
            brake = "0" + brake
            yellow = "0" + yellow
    pd = '        <phase duration="' + normTimes[i] + '" '
    if len(minTimes)==len(normTimes):
        pd = pd + 'min_dur="' + minTimes[i] + '" '
    if len(maxTimes)==len(normTimes):
        pd = pd + 'max_dur="' + maxTimes[i] + '" '
    pd = pd + 'phase="' + phase + '" brake="' + brake + '" yellow="' + yellow + '"/>'
    print pd
print "    </tl-logic>"
print "</add>"
