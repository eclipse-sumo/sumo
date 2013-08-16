#!/usr/bin/env python
"""
@file    tls_check.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2009-08-01
@version $Id$

Verifies the traffic lights in the given network.
Currently verified:
- phase length matches controlled link number

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2009-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import sys, os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net


if len(sys.argv)<2:
    print >> sys.stderr, "Call: tls_check.py <NET>"
    sys.exit()

net1 = sumolib.net.readNet(sys.argv[1], withPrograms=True)

for tlsID in net1._id2tls:
    print "Checking tls '%s'" % tlsID 
    tls = net1._id2tls[tlsID]
    noConnections = tls._maxConnectionNo + 1
    for prog in tls._programs:
        print "   Checking program '%s'" % prog
        prog = tls._programs[prog]
        for i, phase in enumerate(prog._phases):
            if len(phase[0]) != noConnections:
                print "      Error: phase %s describes %s signals instead of %s." % (i, len(phase[0]), noConnections)
