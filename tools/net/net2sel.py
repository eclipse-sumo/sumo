#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    net2sel.py
# @author  Jakob Erdmann
# @date    2025-07-16

"""
This script converts the network to a selection file of junctions and edges
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--net-file", dest="netFile", required=True,
                           category="input", type=argParser.net_file, help="The .net.xml file to convert")
    argParser.add_argument("-o", "--output-file", dest="outFile", category="output",
                           type=argParser.file, help="The polygon output file name")
    argParser.add_argument("-l", "--lanes", action="store_true", default=False,
                           category="processing", help="Write lanes to selection instead of edges")
    return argParser.parse_args()


if __name__ == "__main__":
    options = parse_args()
    net = sumolib.net.readNet(options.netFile)

    with open(options.outFile, 'w') as outf:
        for junction in net.getNodes():
            outf.write("junction:%s\n" % junction.getID())
        for edge in net.getEdges():
            if options.lanes:
                for lane in edge.getLanes():
                    outf.write("lane:%s\n" % lane.getID())
            else:
                outf.write("edge:%s\n" % edge.getID())
