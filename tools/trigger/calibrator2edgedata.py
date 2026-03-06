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

# @file    calibrator2edgedata.py
# @author  Jakob Erdmann
# @date    2026-03-06

"""
Converts calibrator definitions to edgeData. In principle this could also be done by running a simulation and letting
it write edgeData output. The main purpose of this tool is to fuse data from the sumo-live-data-loop inputs
https://github.com/DLR-TS/SUMOLiveDataLoop where calibrators flows overlap in time for intializing repeated predictive simulations
"""
from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime,humanReadableTime  # noqa
from sumolib.net import lane2edge  # noqa

def parse_args():
    op = sumolib.options.ArgumentParser(description="convert calibrators (possibly overlapping) to edgedata")
    op.add_argument("-o", "--output-file", dest="outfile", category="output", type=op.file, required=True,
                    help="name of output file")
    op.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    op.add_argument("calibratorfiles", nargs="+", category="input", type=op.route_file,
                    help="Set one or more input route files")

    options = op.parse_args()

    return options


def parseCalibrators(options):
    # (begin, end) -> edgeID -> attr -> value
    excludeEdgeAttrs = ('begin', 'end', 'route')
    intervals = defaultdict(lambda: defaultdict(dict))
    for cfile in options.calibratorfiles:
        if options.verbose:
            print("Parsing %s" % cfile)
        for calibrator in sumolib.xml.parse(cfile, 'calibrator'):
            edge = lane2edge(calibrator.lane)
            for flow in calibrator.flow:
                begin = parseTime(flow.begin)
                end = parseTime(flow.end)
                attrs = intervals[(begin, end)][edge]
                attrs.clear() # last version should count for overlapping calibrators
                for attr, value in flow.getAttributes():
                    if attr not in excludeEdgeAttrs:
                        attrs[attr] = value
                if 'vehsPerHour' in attrs:
                    attrs['count'] = float(attrs['vehsPerHour']) * (end - begin) / 3600
    return intervals          


def main():
    options = parse_args()
    intervals = parseCalibrators(options)
    with sumolib.openz(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "data", options=options, schemaPath="datamode_file.xsd")
        for begin, end in sorted(intervals.keys()):
            outf.write('    <interval id="fromCalibrators" begin="%s" end="%s">\n' % (
                humanReadableTime(begin), humanReadableTime(end)))
            for edge, attrs in intervals[(begin, end)].items():
                outf.write(' ' * 8 + '<edge id="%s" %s/>\n' % (
                    edge, ' '.join(['%s="%s"' % av for av in attrs.items()])))
            outf.write("    </interval>\n")
        outf.write("</data>\n")


if __name__ == "__main__":
    main()
