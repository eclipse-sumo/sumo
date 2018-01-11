#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function


import os
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib.net

net = sumolib.net.readNet(sys.argv[1])
for r in net.getRoundabouts():
    print("nodes=['%s'] edges=['%s']" % (
        "', '".join(r.getNodes()),
        "', '".join(r.getEdges())))
