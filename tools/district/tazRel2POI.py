#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    tazRel2POI.py
# @author  Jakob Erdmann
# @date    2025-10-02

"""
Creates a POIs for each TAZ where the size indicates attributes of corresponding
tazRelations
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import openz  # noqa


def get_options(args=None):
    op = sumolib.options.ArgumentParser(description="Generate trips between random locations",
                                        allowed_programs=['duarouter', 'marouter'])
    # input
    op.add_argument("-t", "--taz-file", category="input", dest="taz", required=True, type=op.additional_file,
                    help="define taz file to be loaded")
    op.add_argument("-d", "--taz-relation-file", category="input", dest="tazrel",
                    required=True, type=op.additional_file,
                    help="define taz file to be loaded")
    op.add_argument("-n", "--net-file", category="input", dest="netfile",
                    type=op.net_file,
                    help="define net file for determining taz centers")
    # output
    op.add_argument("-o", "--output-file", category="output", dest="outfile",
                    required=True, type=op.additional_file,
                    help="define the output poi filename")
    # processing
    op.add_argument("-a", "--attribute", category="processing", default="count",
                    help="define the tazRel attribute to read")
    op.add_argument("-s", "--size-metric", category="processing", dest="sizeMetric", default='all',
                    help="define the metric to visualize ('in', 'out', 'all')")

    options = op.parse_args(args=args)
    options.net = None
    return options


def addEdgeShape(net, edge, shape, tazID):
    if net.hasEdge(edge):
        e = net.getEdge(edge)
        shape.append(e.getFromNode().getCoord())
        shape.append(e.getToNode().getCoord())
    else:
        print("Edge '%s' in TAZ '%s' not found." % (edge, tazID), file=sys.stderr)


def main(options):
    relsFrom = defaultdict(lambda: 0)
    relsTo = defaultdict(lambda: 0)

    for tazrel in sumolib.xml.parse(options.tazrel, ['tazRelation']):
        if tazrel.hasAttribute(options.attribute):
            val = float(tazrel.getAttribute(options.attribute))
            relsFrom[tazrel.attr_from] += val
            relsTo[tazrel.to] += val

    with openz(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        for taz in sumolib.xml.parse(options.taz, ['taz']):
            if taz.center:
                x, y = taz.center.split(',')
            elif taz.shape:
                shape = []
                for xy in taz.shape.split():
                    x, y = xy.split(',')
                    shape.append((float(x), float(y)))
                bbox = sumolib.geomhelper.addToBoundingBox(shape)
                x = (bbox[0] + bbox[2]) / 2
                y = (bbox[1] + bbox[3]) / 2
            else:
                # compute shape from edges
                if options.net is None:
                    if options.netfile is None:
                        print(("TAZ '%s' because it doesn't define 'center' or 'shape'." % taz.id)
                              + " Option --net-file must be set to handle this input",
                              file=sys.stderr)
                        continue
                    options.net = sumolib.net.readNet(options.netfile)
                shape = []
                if taz.edges:
                    for edge in taz.edges.split():
                        addEdgeShape(options.net, edge, shape, taz.id)
                if taz.tazSource:
                    for ts in taz.tazSource:
                        addEdgeShape(options.net, ts.id, shape, taz.id)
                if taz.tazSink:
                    for ts in taz.tazSink:
                        addEdgeShape(options.net, ts.id, shape, taz.id)
                if not shape:
                    print(("Skipping TAZ '%s' because no edges were found." % taz.id), file=sys.stderr)
                    continue

                bbox = sumolib.geomhelper.addToBoundingBox(shape)
                x = (bbox[0] + bbox[2]) / 2
                y = (bbox[1] + bbox[3]) / 2

            if options.sizeMetric == 'in':
                val = relsTo[taz.id]
            elif options.sizeMetric == 'out':
                val = relsFrom[taz.id]
            else:
                val = relsTo[taz.id] + relsFrom[taz.id]
            outf.write('    <poi id="%s" x="%s" y="%s" width="%s">\n' % (
                taz.id, x, y, val))
            outf.write('        <param key="in" value="%s"/>\n' % relsTo[taz.id])
            outf.write('        <param key="out" value="%s"/>\n' % relsFrom[taz.id])
            outf.write('        <param key="all" value="%s"/>\n' % (relsFrom[taz.id] + relsTo[taz.id]))
            outf.write('    </poi>\n')
        outf.write('</additional>\n')


if __name__ == "__main__":
    try:
        main(get_options())
    except ValueError as e:
        print("Error:", e, file=sys.stderr)
        sys.exit(1)
