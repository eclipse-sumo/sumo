#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
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
    #  'E3': 'multiEntryExitDetector'
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
                  help="app detector with the given probability ]0, 1]")
    ap.add_option("-t", "--detector-type", dest="dType", default="inductionLoop",
                  help="one of %s or the corresponding shortcut %s" % (
                      list(SHORT_NAMES.values()),
                      list(SHORT_NAMES.keys())))
    ap.add_option("--vclass", default="passenger",
                  help="only place detectors on lanes that permit the given vehicle class")
    ap.add_option("--length", type=float,
                  help="Set length for detector types that support it")
    ap.add_option("-s", "--seed", type=int, default=42, help="random seed")
    ap.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    options = ap.parse_args(args=args)

    options.dType = SHORT_NAMES.get(options.dType, options.dType)

    if options.dType not in SHORT_NAMES.values():
        sys.exit("Unsupported value '%s' for option --detector-type ")

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

        for edge in net.getEdges():
            for lane in edge.getLanes():
                if not lane.allows(options.vclass):
                    continue
                if options.probability < 1 and random.random() > options.probability:
                    continue
                numWritten += 1
                fout.write('    <%s id="%s%s" lane="%s" pos="%s" %s%s%s%sfile="%s"/>\n' % (
                    options.dType,
                    options.prefix, lane.getID(),
                    lane.getID(),
                    "%.2f" % options.getRelpos(lane),
                    period, length, endPos, friendlyPos,
                    options.results))

        fout.write('</additional>\n')

    if options.verbose:
        print("Wrote %s detectors." % numWritten)


if __name__ == "__main__":
    main(get_options())
