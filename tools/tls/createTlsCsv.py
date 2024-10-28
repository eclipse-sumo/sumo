#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    createTlsCsv.py
# @author  Harald Schaefer
# @author  Michael Behrisch
# @date    2017-04-22

"""
Create tls links from sumo net as needed by tls_csv2SUMO.py.
You have to edit the link number field (preset with g).
 The comment gives the link number shown on demand in SUMO-GUI.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from lxml import etree
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))
import sumolib  # noqa

op = sumolib.options.ArgumentParser(
    description='Create tls links from sumo net as needed by tls_csv2SUMO.py. You have to edit the link number ' +
    'field (preset with g). The comment gives the link number shown on demand in SUMO-GUI')

op.add_argument("-n", "--net-file", category="input", type=op.net_file,
                dest="net", required=True, help='Input file name')

try:
    options = op.parse_args()
except (NotImplementedError, ValueError) as e:
    print(e, file=sys.stderr)
    sys.exit(1)

doc = etree.parse(options.net)

connections = {}

for conn in doc.xpath('//connection'):
    if 'linkIndex' in conn.attrib:
        # use traffic light id and right adjusted number for sorting and as
        # comment
        numIndex = conn.attrib['linkIndex']
        index = conn.attrib['tl'] + ';' + numIndex.zfill(3)
        connections[index] = conn.attrib['from'] + '_' + conn.attrib['fromLane'] + \
            ';' + conn.attrib['to'] + '_' + conn.attrib['toLane']
        # print record
        # print conn.attrib['from'], conn.attrib['to'],
        # conn.attrib['linkIndex']

for conn in sorted(connections):
    # print conn, connections[conn]
    print("link;g;{};0".format(connections[conn]).ljust(50) + '#' + str(conn).rjust(3))
