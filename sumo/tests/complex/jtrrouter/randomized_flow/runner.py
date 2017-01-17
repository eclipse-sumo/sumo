#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@author  Laura Bieker
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-01-18
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


def get_depart_lines(route_file):
    return [l for l in open(route_file) if 'depart' in l]

output_file1 = 'output1.rou.xml'
output_file2 = 'output2.rou.xml'

jtrrouter = checkBinary('jtrrouter')
assert(jtrrouter)

args = [jtrrouter,
        '--net-file', 'input_net.net.xml',
        '--flow-files', 'input_flows.flows.xml',
        '--turn-ratio-files', 'input_turns.turns.xml',
        '--output-file', output_file1,
        '--sinks=end',
        '--seed', None,
        '--no-step-log',
        '--randomize-flows']

args[11] = str(random.randint(0, 2 ** 31))
subprocess.call(args)
route_lines1 = get_depart_lines(output_file1)

args[8] = output_file2
args[11] = str(random.randint(0, 2 ** 31))
subprocess.call(args)
route_lines2 = get_depart_lines(output_file2)

if route_lines1 != route_lines2:
    print('test passed. output is random')
else:
    print('test failed. output is deterministic')
