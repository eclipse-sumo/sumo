#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateDetectors.py
# @author  Jakob Erdmann
# @date    2025-04-13

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import random

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.miscutils import openz  # noqa

SHORT_NAMES = {
    'E1': 'inductionLoop',
    'E1I': 'instantInductionLoop',
    'E2': 'laneAreaDetector',
    'E3': 'entryExitDetector',
}

ALIASES = {
    'e3Detector': 'entryExitDetector',
    'multiEntryExitDetector': 'entryExitDetector',
}

NEED_EXTENT = ['laneAreaDetector',
               #  'multiEntryExitDetector'
               ]


def get_options(args=None):
    ap = ArgumentParser(description="Generate detectors on selected network edges")
    ap.add_option("-n", "--net-file", dest="net_file", type=ap.net_file, required=True,
                  help="Network file to work with. Mandatory.")
    ap.add_option("-o", "--output", required=True, type=ap.additional_file,
                  help="The name of the file to write the detectors to")
    ap.add_option("--period", type=int, default=60,
                  help="Aggregation period in s")
    ap.add_option("--prefix", default="",
                  help="prefix for the detectors ids")
    ap.add_option("-r", "--results-file", dest="results", type=ap.file, default="det.out.xml",
                  help="The name of the file the detectors write their output into. Defaults to det.out.xml.")
    ap.add_option("--relpos", default=0.5,
                  help="relative detector position along the edge [0,1] or 'random'")
    ap.add_option("--probability", type=float, default=1,
                  help="build detector with the given probability ]0, 1] (lanewise)")
    ap.add_option("--edge-probability", type=float, default=1, dest="edgeProbability",
                  help="build detector with the given probability ]0, 1] (edgewise)")
    ap.add_option("-t", "--detector-type", dest="dType", default="inductionLoop",
                  help="one of %s or the corresponding shortcut %s" % (
                      list(SHORT_NAMES.values()),
                      list(SHORT_NAMES.keys())))
    ap.add_option("--vclass", default="passenger",
                  help="only place detectors on lanes that permit the given vehicle class")
    ap.add_option("--length", type=float,
                  help="Set length for detector types that support it")
    ap.add_option("--next", action="store_true", default=False,
                  help="generate nextEdges attribute and additional detectors for lanes with multiple targets")
    ap.add_option("-s", "--seed", type=int, default=42, help="random seed")
    ap.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    options = ap.parse_args(args=args)

    options.dType = SHORT_NAMES.get(options.dType, options.dType)
    options.dType = ALIASES.get(options.dType, options.dType)

    if options.dType not in SHORT_NAMES.values():
        sys.exit("Unsupported value '%s' for option --detector-type" % options.dType)

    if options.relpos is not None:
        try:
            options.relpos = max(0, min(1, float(options.relpos)))
            options.getRelpos = lambda lane: lane.getLength() * options.relpos
        except ValueError:
            if options.relpos == 'random':
                options.getRelpos = lambda lane: lane.getLength() * random.random()
            else:
                sys.exit("option --relpos must be set to 'random' or to a float value from [0,1]")

    if options.length and options.dType == "instantInductionLoop":
        sys.exit("Unsupported option --length for detector-type %s" % options.dType)

    return options


def main(options):
    random.seed(options.seed)

    if options.verbose:
        print("Reading net '%s'..." % options.net_file)
    net = sumolib.net.readNet(options.net_file)

    if options.verbose:
        print("Generating detectors...")

    numWritten = 0
    with openz(options.output, 'w') as fout:
        sumolib.writeXMLHeader(fout, "$Id$", "additional", options=options)
        period = '' if options.dType == "instantInductionLoop" else 'period="%s" ' % options.period
        length = '' if options.length is None else 'length="%s" ' % options.length
        endPos = ''
        friendlyPos = ''
        if length != '':
            friendlyPos = 'friendlyPos="true" '
        elif options.dType in NEED_EXTENT:
            endPos = 'endPos="-1" '

        def writeDet(options, lane, numWritten, nextEdge=None):
            fout.write('    <%s id="%s%s%s" lane="%s" pos="%s" %s%s%s%s%sfile="%s"/>\n' % (
                options.dType,
                options.prefix, lane.getID(),
                '_%s' % nextEdge.getID() if nextEdge else '',
                lane.getID(),
                "%.2f" % options.getRelpos(lane),
                period, length, endPos, friendlyPos,
                'nextEdges="%s" ' % nextEdge.getID() if nextEdge else "",
                options.results))
            numWritten += 1

        def writeE3(detID, entryLanes, exitLanes, exitPos):
            fout.write('    <entryExitDetector id="%s%s" %sfile="%s" openEntry="true">\n' % (
                options.prefix, detID, period, options.results))
            for lane in entryLanes:
                ep = max(0.0, lane.getLength() - options.length) if options.length is not None else 0.0
                fout.write('        <detEntry lane="%s" pos="%.2f"/>\n' % (lane.getID(), ep))
            for lane in exitLanes:
                fout.write('        <detExit lane="%s" pos="%.2f"/>\n' % (lane.getID(), exitPos))
            fout.write('    </entryExitDetector>\n')

        for edge in net.getEdges():
            if options.edgeProbability < 1 and random.random() > options.edgeProbability:
                continue
            lanes = [lane for lane in edge.getLanes() if lane.allows(options.vclass)
                     and (options.probability >= 1 or random.random() <= options.probability)]
            if options.dType == "entryExitDetector":
                if not lanes:
                    continue
                if options.next and len(edge.getOutgoing()) > 1:
                    for nextEdge in sorted(edge.getOutgoing(), key=lambda e: e.getID()):
                        nextLanes = [nl for nl in nextEdge.getLanes() if nl.allows(options.vclass)]
                        if nextLanes:
                            writeE3("%s_%s" % (edge.getID(), nextEdge.getID()), lanes, nextLanes, 0.1)
                            numWritten += 1
                else:
                    writeE3(edge.getID(), lanes, lanes, -0.1)
                    numWritten += 1
                continue
            for lane in lanes:
                if options.next and len(lane.getOutgoing()) > 1:
                    for outEdge in lane.getOutgoingEdges():
                        writeDet(options, lane, numWritten, outEdge)
                        numWritten += 1
                else:
                    writeDet(options, lane, numWritten)
                    numWritten += 1

        fout.write('</additional>\n')

    if options.verbose:
        print("Wrote %s detectors." % numWritten)


if __name__ == "__main__":
    main(get_options())
