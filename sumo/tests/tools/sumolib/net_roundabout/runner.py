#!/usr/bin/env python
"""
@file    runner.py
@author  Jakob Erdmann
@version $Id$

This script tests sumolib functions

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function


import os
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib.net

net = sumolib.net.readNet(sys.argv[1])
for r in net.getRoundabouts():
    print("nodes=%s edges=%s" % (
        [n for n in r.getNodes()],
        [e for e in r.getEdges()]))
