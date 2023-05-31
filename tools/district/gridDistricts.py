#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    gridDistricts.py
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2019-01-02

"""
Generate a grid-based TAZ file
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import random
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import Colorgen  # noqa
from sumolib.options import ArgumentParser  # noqa


class TAZ:
    def __init__(self, id, shape, color):
        self.id = id
        self.shape = shape
        self.color = color
        self.edges = []

    def write(self, outf):
        outf.write('    <taz id="%s" shape="%s" color="%s" edges="%s"/>\n' % (
            self.id, ' '.join(["%s,%s" % (x, y) for x, y in self.shape]),
            self.color, ' '.join(self.edges)))


def getOptions():
    ap = ArgumentParser()
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    ap.add_argument("-n", "--net-file", dest="netfile", category="input", type=ArgumentParser.net_file,
                    required=True, help="the network to read lane and edge permissions")
    ap.add_argument("-o", "--output", category="output", type=ArgumentParser.file,
                    required=True, help="output taz file")
    ap.add_argument("-w", "--grid-width", dest="gridWidth", type=float, default=100.0,
                    help="width of gride cells in m")
    ap.add_argument("--vclass", type=str, help="Include only edges allowing VCLASS")
    ap.add_argument("-u", "--hue", default="random", type=str,
                    help="hue for taz (float from [0,1] or 'random')")
    ap.add_argument("-s", "--saturation", default=1, type=str,
                    help="saturation for taz (float from [0,1] or 'random')")
    ap.add_argument("-b", "--brightness", default=1, type=str,
                    help="brightness for taz (float from [0,1] or 'random')")
    ap.add_argument("--seed", type=int, default=42, help="random seed")
    options = ap.parse_args()
    if not options.netfile or not options.output:
        ap.print_help()
        ap.exit("Error! net-file and output file")
    options.colorgen = Colorgen((options.hue, options.saturation, options.brightness))
    return options


if __name__ == "__main__":
    options = getOptions()
    random.seed(options.seed)
    if options.verbose:
        print("Reading net")
    net = sumolib.net.readNet(options.netfile)
    xmin, ymin, xmax, ymax = net.getBoundary()
    odpairs = {}  # (x,y) -> TAZ
    centerCoords = {}  # edge -> center pos
    w = options.gridWidth
    w2 = w * 0.5 - 1
    for edge in net.getEdges():
        if options.vclass is not None and not edge.allows(options.vclass):
            continue
        x, y = sumolib.geomhelper.positionAtShapeOffset(edge.getShape(True), edge.getLength() / 2)
        xIndex = int((x - xmin + w2) / w)
        yIndex = int((y - ymin + w2) / w)
        ii = (xIndex, yIndex)
        x2 = xIndex * w
        y2 = yIndex * w
        if ii not in odpairs:
            odpairs[ii] = TAZ("%s_%s" % (xIndex, yIndex),
                              [(x2 - w2, y2 - w2),
                               (x2 + w2, y2 - w2),
                               (x2 + w2, y2 + w2),
                               (x2 - w2, y2 + w2),
                               (x2 - w2, y2 - w2)],
                              options.colorgen())
        odpairs[ii].edges.append(edge.getID())

    with open(options.output, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        for ii, taz in sorted(odpairs.items()):
            taz.write(outf)
        outf.write("</additional>\n")
