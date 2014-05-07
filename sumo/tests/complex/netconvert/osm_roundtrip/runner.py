#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@author  Laura Bieker
@author  Michael Behrisch
@date    2011-05-27
@version $Id$

import osm network 
then import the built network again and check for idempotency

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


import sys,os,subprocess
import difflib
import StringIO
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "import", "osm"))
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
from sumolib import checkBinary
import fpdiff

osm_input = 'osm.xml'
net_output= 'from_osm'
net_output2= 'reloaded'

netconvert = checkBinary('netconvert')
assert(netconvert)

# filter header and projection clause
def filter(lines, start_element):
    skippedHeader = False
    result = []
    for l in lines:
        if start_element in l:
            skippedHeader = True
        if not skippedHeader:
            continue
        if '<location' in l:
            continue
        if '<projection' in l:
            continue
        result.append(l)
    return result

def get_filtered_lines(prefix):
    result = []
    for suffix, start_element in [
            ('.net.xml', '<net '),
            ('.nod.xml', '<nodes '),
            ('.edg.xml', '<edges '),
            ('.con.xml', '<connections '),
            ('.tll.xml', '<tlLogics ')]:
        result += filter(open(prefix + suffix, 'U').readlines(), start_element)
    return result

args1 = [netconvert,
        '--no-internal-links', 
        '--osm-files',  osm_input, '--proj.utm',
        '-R', '--ramps.guess', 
        '--tls.guess', '--tls.join',
        '--junctions.join',
        '--plain-output-prefix', net_output,
        '--output', net_output + '.net.xml']

args2 = [netconvert,
        '--sumo-net-file', net_output + '.net.xml',
        '--no-internal-links', 
        '--offset.disable-normalization',
        '--plain-output-prefix', net_output2,
        '--output', net_output2 + '.net.xml']

subprocess.call(args1)
subprocess.call(args2)

fromlines = get_filtered_lines(net_output)
tolines = get_filtered_lines(net_output2)
#with open('fromlines','w') as f: f.write('\n'.join(fromlines))
#with open('tolines','w') as f: f.write('\n'.join(tolines))
out = StringIO.StringIO()
fpdiff.fpfilter(fromlines, tolines, out, 0.0201)
out.seek(0)
tolines = out.readlines()
sys.stderr.writelines(difflib.unified_diff(fromlines, tolines))
