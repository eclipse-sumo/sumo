#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateStationEdges.py
# @author  Jakob Erdmann
# @date    2018-08-07

"""
Generate pedestrian edges for every public transport stop to serve as access
edge in otherwise pure railway networks
"""
from __future__ import absolute_import
from __future__ import print_function
import sys
import os

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa


def parse_args():
    USAGE = "%(prog)s -n <netfile> -s <stopfile> -o <utput> [options]"
    ap = ArgumentParser(usage=USAGE)
    ap.add_option("-n", "--net-file", category="input", dest="netfile", required=True, type=ap.net_file,
                  help="input network file")
    ap.add_option("-s", "--stop-file", category="input", dest="stopfile", required=True, type=ap.additional_file,
                  help="input network file")
    ap.add_option("-o", "--output-prefix", category="output", dest="outfile", required=True, type=ap.file,
                  help="prefix of output files")
    ap.add_option("-l", "--length", type=float, default=10.,
                  help="length of generated solitary edges")
    ap.add_option("-w", "--width", type=float, default=8.,
                  help="width of generated solitary edges")
    # optParser.add_option("-d", "--cluster-dist", type=float, default=500., help="length of generated edges")
    options = ap.parse_args()
    return options


def main(options):
    net = sumolib.net.readNet(options.netfile, withConnections=False, withFoes=False)
    l2 = options.length / 2
    with open(options.outfile + ".edg.xml", 'w') as out_e:
        with open(options.outfile + ".nod.xml", 'w') as out_n:
            sumolib.writeXMLHeader(out_e, "$Id$")  # noqa
            sumolib.writeXMLHeader(out_n, "$Id$")  # noqa
            out_e.write('<edges>\n')
            out_n.write('<nodes>\n')
            for stop in sumolib.xml.parse(options.stopfile, 'busStop', heterogeneous=True):
                edge_id = stop.id + "_access"
                x, y = sumolib.geomhelper.positionAtShapeOffset(
                    net.getLane(stop.lane).getShape(),
                    (float(stop.startPos) + float(stop.endPos)) / 2)
                from_id = edge_id + '_from'
                to_id = edge_id + '_to'
                out_n.write('    <node id="%s" x="%s" y="%s"/>\n' % (from_id, x - l2, y))
                out_n.write('    <node id="%s" x="%s" y="%s"/>\n' % (to_id, x + l2, y))
                out_e.write('    <edge id="%s" from="%s" to="%s" allow="pedestrian" width="%s"/>\n' % (
                    edge_id, from_id, to_id, options.width))
            out_e.write('</edges>\n')
            out_n.write('</nodes>\n')


if __name__ == "__main__":
    options = parse_args()
    main(options)
