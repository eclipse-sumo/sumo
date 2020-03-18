#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    attributeStats.py
# @author  Jakob Erdmann
# @date    2019-04-11

"""
compute statistics for a specific xml attribute (e.g. timeLoss in tripinfo-output)
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict
from optparse import OptionParser

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    import sumolib
    from sumolib.xml import parse  # noqa
    from sumolib.miscutils import Statistics  # noqa
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    USAGE = """Usage %prog [options] <tripinfos..xml>"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--element", type="string",
                         default="tripinfo", help="element to analyze")
    optParser.add_option("--attribute", type="string",
                         default="timeLoss", help="attribute to analyze")
    optParser.add_option("--binwidth", type="float",
                         default=50, help="binning width of result histogram")
    optParser.add_option("--hist-output", type="string",
                         default=None, help="output file for histogram (gnuplot compatible)")
    optParser.add_option("--full-output", type="string",
                         default=None, help="output file for full data dump")
    options, args = optParser.parse_args()

    if len(args) != 1:
        sys.exit(USAGE)

    options.tripinfos = args[0]
    return options


def main():
    options = get_options()
    attribute_retriever = None

    def attribute_retriever(tripinfo):
        return

    vals = defaultdict(list)
    stats = Statistics("%s %ss" % (options.element, options.attribute), histogram=True, scale=options.binwidth)
    for tripinfo in parse(options.tripinfos, options.element):
        try:
            val = float(tripinfo.getAttribute(options.attribute))
        except:
            val = sumolib.miscutils.parseTime(tripinfo.getAttribute(options.attribute))
        vals[tripinfo.id].append(val)
        stats.add(val, tripinfo.id)

    print(stats)

    if options.hist_output is not None:
        with open(options.hist_output, 'w') as f:
            for bin, count in stats.histogram():
                f.write("%s %s\n" % (bin, count))

    if options.full_output is not None:
        with open(options.full_output, 'w') as f:
            for id, data in vals.items():
                for x in data:
                    f.write("%s %s\n" % (x, id))


if __name__ == "__main__":
    main()
