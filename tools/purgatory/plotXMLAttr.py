#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

# @file    plotXMLAttr.py
# @author  Jakob Erdmann
# @date    2017-12-04

"""generate boxplot for an aribitrary xml attribute"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse  # noqa
from sumolib.miscutils import Statistics, parseTime  # noqa
from sumolib.options import ArgumentParser  # noqa


def parse_args():
    optParser = ArgumentParser()
    optParser.add_argument("tag", help="XML tag containing the attribute to be plotted")
    optParser.add_argument("attr", help="XML attribute to be plotted")
    optParser.add_argument("xmlfiles", help="XML file(s)", nargs='*')
    return optParser.parse_args()


def main(tag, attr, xmlfiles):
    data = []
    for xmlfile in xmlfiles:
        stats = Statistics('%s %s' % (tag, attr))
        for elem in parse(xmlfile, tag):
            stats.add(parseTime(elem.getAttribute(attr)), elem.id)
        print(stats)
        data.append(stats.values)
    try:
        import matplotlib.pyplot as plt  # noqa
        plt.figure()
        plt.xticks(range(len(xmlfiles)), xmlfiles)
        plt.ylabel("%s %s" % (tag, attr))
        plt.boxplot(data)
        plt.show()
    except ImportError:
        print("Matplotlib not found, cannot generate plot.", file=sys.stderr)


if __name__ == "__main__":
    options = parse_args()
    main(options.tag, options.attr, options.xmlfiles)
