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

# @file    turnFile2EdgeRelations.py
# @author  Jakob Erdmann
# @date    2020-02-09

"""
Converts turn-count data into edgeData
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    ap.add_argument("-t", "--turn-file", dest="turnFile", required=True, type=ap.file,
                    help="Input turn-count file")
    ap.add_argument("-o", "--output-file", dest="out", required=True, type=ap.file,
                    help="Output edgeRelations file")
    ap.add_argument("--turn-attribute", dest="turnAttr", default="probability",
                    help="Write turning 'probability' to the given attribute")

    options = ap.parse_args(args=args)
    if options.turnFile is None or options.out is None:
        ap.print_help()
        sys.exit()
    return options


def main(options):
    with open(options.out, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "data", "datamode_file.xsd")  # noqa
        for interval in sumolib.xml.parse(options.turnFile, 'interval'):
            outf.write('    <interval begin="%s" end="%s">\n' % (
                interval.begin, interval.end))
            for c in interval.getChildList(True):
                if c.isComment():
                    outf.write('        <!-- %s -->\n' % c.getText())
                elif c.name == 'fromEdge':
                    fromEdge = c
                    for toEdge in fromEdge.toEdge:
                        outf.write(' ' * 8 + '<edgeRelation from="%s" to="%s" %s="%s"/>\n' % (
                            fromEdge.id, toEdge.id, options.turnAttr, toEdge.probability))
            outf.write('    </interval>\n')
        outf.write('</data>\n')


if __name__ == "__main__":
    main(get_options())
