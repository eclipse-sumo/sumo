#!/usr/bin/env python
"""
@file    tls_csv2SUMO.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id$

Verifies the traffic lights in the given network.
Currently verified:
- phase length matches controlled link number

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import sys, os
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


if len(sys.argv)<2:
    print >> sys.stderr, "Call: tls_csv2SUMO.py <NET>"
    sys.exit()

parser = make_parser()
net1 = sumonet.NetReader(withPrograms=True)
parser.setContentHandler(net1)
parser.parse(sys.argv[1])
net1 = net1.getNet()

for tlsID in net1._id2tls:
    print "Checking tls '%s'" % tlsID 
    tls = net1._id2tls[tlsID]
    noConnections = tls._maxConnectionNo + 1
    for prog in tls._programs:
        print "   Checking program '%s'" % prog
        prog = tls._programs[prog]
        for i, phase in enumerate(prog._phases):
            if len(phase[0])!=noConnections:
                print "      Error: phase %s describes %s signals instead of %s." % (i, len(phase[0]), noConnections)
