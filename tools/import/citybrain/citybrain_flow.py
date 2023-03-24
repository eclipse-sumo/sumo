#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    citybrain_flow.py
# @author  Jakob Erdmann
# @date    2021-05-07

import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools', 'route'))
import sumolib  # noqa
from sort_routes import sort_departs  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Import citybrains traffic demand")
    optParser.add_argument("-f", "--flow-file", dest="flowfile",
                           help="citybrains flow file to import")
    optParser.add_argument("-o", "--output", dest="output",
                           help="define the output sumo route filename")
    optParser.add_argument("-p", "--prefix", dest="prefix",
                           default="", help="prefix generated flow ids")
    options = optParser.parse_args(args=args)
    if not options.flowfile or not options.output:
        optParser.print_help()
        sys.exit(1)

    return options


def main(options):
    # unsorted temporary file
    tmpfile = options.output + ".tmp"

    with open(tmpfile, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes")  # noqa
        outf.write('    <vType id="DEFAULT_VEHTYPE" length="4" minGap="1"/>\n\n')
        flowLine = 0
        flowIndex = 0
        for i, line in enumerate(open(options.flowfile)):
            if i > 0:
                if flowLine == 0:
                    flowID = "%s%s" % (options.prefix, flowIndex)
                    begin, end, period = line.split()
                    outf.write('    <flow id="%s" begin="%s" end="%s" period="%s">\n' % (
                        flowID, begin, end, period))
                elif flowLine == 2:
                    edges = line.strip()
                    outf.write('        <route edges="%s"/>\n' % edges)
                    outf.write('    </flow>\n')
                    flowIndex += 1
                flowLine = (flowLine + 1) % 3
        outf.write('</routes>\n')

    with open(options.output, "w") as outf:
        sort_departs(tmpfile, outf)
    os.remove(tmpfile)


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
