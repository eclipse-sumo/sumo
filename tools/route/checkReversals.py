#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2014-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkReversals.py
# @author  Jakob Erdmann
# @date    2025-04-16

"""
Check for reversals in loaded routes
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    from sumolib.statistics import Statistics
    import sumolib  # noqa
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    ap.add_argument("-n", "--network", category="input", required=True, type=ap.net_file,
                    help="sumo network to use")
    ap.add_argument("-r", "--route-files", category='input', dest="routeFiles", required=True,
                    help="Input route files")
    ap.add_argument("-o", "--output-file", category='output', dest="output",
                    help="Output file for info on each reversing vehicle")
    options = ap.parse_args(args=args)
    options.routeFiles = options.routeFiles.split(',')
    return options


def main(options):
    revByVeh = Statistics("Reversals by vehicle")
    revByEdge = Statistics("Reversals by edge")
    nRevByEdge = defaultdict(lambda: 0)
    routes = {}  # id -> edges
    nVehs = 0

    outf = None
    if options.output:
        outf = sumolib.openz(options.output, 'w')
        sumolib.xml.writeHeader(outf, "$Id$", "reversals", options=options, rootAttrs=None)

    net = sumolib.net.readNet(options.network)
    for routefile in options.routeFiles:
        lastEdges = None
        for elem in sumolib.xml.parse(routefile, ['vehicle', 'flow', 'route'], heterogeneous=True):
            if elem.name == 'route':
                lastEdges = [net.getEdge(e) for e in elem.edges.split()]
                if elem.id:
                    routes[elem.id] = lastEdges
                else:
                    #  embedded route
                    continue
            else:
                nVehs += 1
                if type(elem.route) is str:
                    edges = routes[elem.route]
                else:
                    edges = lastEdges
                reversals = []
                for e, e2 in zip(edges[:-1], edges[1:]):
                    if e.getBidi() == e2:
                        reversals.append(e.getID())
                        nRevByEdge[e.getID()] += 1
                if reversals:
                    revByVeh.add(len(reversals), elem.id)
                    if outf:
                        outf.write('    <%s id="%s" reversalEdges="%s"/>\n' % (
                            elem.name, elem.id, " ".join(reversals)))
    for e, n in nRevByEdge.items():
        revByEdge.add(n, e)

    if routes:
        print("Loaded %s named routes" % len(routes))
    print("Loaded %s vehicles" % nVehs)
    print(revByVeh)
    print(revByEdge)

    if outf:
        outf.write('</reversals>\n')
        outf.close()


if __name__ == "__main__":
    main(get_options())
