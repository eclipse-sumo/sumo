#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2011-05-23
@version $Id$

call jtrrouter twice and check that the output differs

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

import sys
import os
import subprocess
import random
sys.path.append(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
from sumolib import checkBinary

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
