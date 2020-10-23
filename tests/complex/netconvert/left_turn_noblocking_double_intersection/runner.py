#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2011-05-23

from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import subprocess
sys.path.append(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
from sumolib import checkBinary  # noqa

net_output = 'joined.net.xml'
trips_output = 'trips.log'

netconvert = checkBinary('netconvert')
assert(netconvert)
sumo = checkBinary('sumo')
assert(sumo)

args_netc = [netconvert,
             '--node-files', 'input_nodes.nod.xml',
             '--edge-files', 'input_edges.edg.xml',
             '--output', net_output,
             '--offset.disable-normalization']

args_sumo = [sumo,
             '--net-file', net_output,
             '--route-files', 'input_routes.rou.xml',
             '--end', '50',
             '--no-step-log',
             '--no-duration-log',
             '--tripinfo-output', trips_output]

subprocess.call(args_netc)
subprocess.call(args_sumo)

# vehicles should have completed their trips
complete = False
for line in open(trips_output):
    if 'veh0' in line:
        complete = True

if complete:
    print('test passed. no blocking occured')
else:
    print('test failed. vehicles were blocked')
