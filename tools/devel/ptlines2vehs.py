#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    ptlines2vehs.py
# @author  Jakob Erdmann
# @date    2026-01-22


"""convert a ptLine into a vehicle that validate route/stop consistency
"""
import sys
import os

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools', 'import', 'gtfs'))
import sumolib
from sumolib.xml import parse
from sumolib.miscutils import openz
from gtfs2osm import OSM2SUMO_MODES

infile, outfile = sys.argv[1:]
with openz(outfile, 'w') as outf:
    sumolib.writeXMLHeader(outf, "$Id$", "routes")
    types = set()
    for ptLine in parse(infile, 'ptLine'):
        types.add(ptLine.type)
    for t in types:
        outf.write('    <vType id="%s" vClass="%s"/>\n' % (t, OSM2SUMO_MODES[t]))

    for ptLine in parse(infile, 'ptLine'):
        outf.write('    <vehicle id="%s" type="%s" depart="0">\n' % (ptLine.id, ptLine.type))
        outf.write(ptLine.route[0].toXML(initialIndent=' ' * 8))
        for stop in ptLine.busStop:
            outf.write(' ' * 8 + '<stop busStop="%s" duration="1"/> <!-- %s -->\n' % (
                stop.id, stop.attr_name))
        outf.write('    </vehicle>\n')
    outf.write('</routes>\n')
