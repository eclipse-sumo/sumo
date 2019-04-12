#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    attributeStats.py
# @author  Jakob Erdmann
# @date    2019-04-11
# @version $Id$

"""
compute statistics for a specific xml attribute (e.g. timeLoss in tripinfo-output)
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from optparse import OptionParser

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
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

    vals = {}
    stats = Statistics("%s %ss" % (options.element, options.attribute), histogram=True, scale=options.binwidth)
    for tripinfo in parse(options.tripinfos, options.element):
        val = float(tripinfo.getAttribute(options.attribute))
        vals[tripinfo.id] = val
        stats.add(val, tripinfo.id)

    print(stats)

    if options.hist_output is not None:
        with open(options.hist_output, 'w') as f:
            for bin, count in stats.histogram():
                f.write("%s %s\n" % (bin, count))

    if options.full_output is not None:
        with open(options.full_output, 'w') as f:
            data = [(v, k) for k, v in vals.items()]
            for val, id in sorted(data):
                f.write("%s %s\n" % (val, id))


if __name__ == "__main__":
    main()
