#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

osm_input = 'osm.xml'
net_output = 'from_osm'
net_output2 = 'net'

netconvert = sumolib.checkBinary('netconvert')

# filter header and projection clause


def filtered(file_name, start_element):
    skippedHeader = False
    with open(file_name) as f:
        for line in f:
            if start_element in line:
                skippedHeader = True
            if not skippedHeader:
                continue
            if '<location' in line:
                continue
            if '<projection' in line:
                continue
            yield line


def get_filtered_lines(prefix):
    result = []
    for suffix, start_element in [
            ('.net.xml', '<net '),
            ('.nod.xml', '<nodes '),
            ('.edg.xml', '<edges '),
            ('.con.xml', '<connections '),
            ('.tll.xml', '<tlLogics ')]:
        result += list(filtered(prefix + suffix, start_element))
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
sys.stderr.writelines(difflib.unified_diff(fromlines, sumolib.fpdiff.fpfilter(fromlines, tolines, 0.0201)))
