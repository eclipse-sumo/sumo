#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    findAllRoutes.py
# @author  Michael Behrisch
# @date    2020-11-02

"""
determine all possible routes between source and destination edges
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import sumolib


def get_options(args=None):
    argParser = sumolib.options.ArgumentParser()
    argParser.add_argument("-n", "--net-file", help="the SUMO net filename")
    argParser.add_argument("-o", "--output-file", help="the route output filename")
    argParser.add_argument("-s", "--source-edges", help="comma separated list of source edge ids")
    argParser.add_argument("-t", "--target-edges", help="comma separated list of target edge ids")
    options = argParser.parse_args(args=args)
    if options.net_file is None or options.output_file is None:
        argParser.print_help()
        sys.exit()
    return options


def main(options):
    net = sumolib.net.readNet(options.net_file)
    if options.source_edges is None:
        stubs = [[e] for e in net.getEdges()]
    else:
        stubs = [[net.getEdge(e)] for e in options.source_edges.split(",")]
    if options.target_edges is None:
        targets = set(net.getEdges())
    else:
        targets = set([net.getEdge(e) for e in options.target_edges.split(",")])
    with open(options.output_file, 'w') as outf:
        sumolib.xml.writeHeader(outf, root="routes")
        idx = 0
        while stubs:
            s = stubs.pop(0)
            if s[-1] in targets:
                print('    <route id="%s" edges="%s"/>' % (idx, " ".join([e.getID() for e in s])), file=outf)
                idx += 1
            else:
                seen = set(s)
                for edge in sorted(s[-1].getOutgoing(), key=lambda e: e.getID()):
                    if edge not in seen:
                        stubs.append(s + [edge])
        outf.write('</routes>\n')


if __name__ == "__main__":
    main(get_options())
