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
# @author  Laura Bieker
# @author  Michael Behrisch
# @date    2011-05-27

"""
import osm network
then import the built network again and check for idempotency
"""
from __future__ import absolute_import


import sys
import os
import subprocess
import difflib
import StringIO
sys.path.append(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "import", "osm"))
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
from sumolib import checkBinary  # noqa
import texttestlib.default.fpdiff  # noqa

osm_input = 'osm.xml'
net_output = 'from_osm'
net_output2 = 'net'

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
# with open('fromlines','w') as f: f.write('\n'.join(fromlines))
# with open('tolines','w') as f: f.write('\n'.join(tolines))
out = StringIO.StringIO()
texttestlib.default.fpdiff.fpfilter(fromlines, tolines, out, 0.0201)
out.seek(0)
tolines = out.readlines()
sys.stderr.writelines(difflib.unified_diff(fromlines, tolines))
