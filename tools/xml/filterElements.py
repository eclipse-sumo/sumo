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

# @file    filterElements.py
# @author  Jakob Erdmann
# @date    2022-12-15

"""
removes elements by matching an attribute
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import optparse
from lxml import etree as ET


if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-f", "--file", dest="file", help="define the XML input file")
    optParser.add_option("-o", "--output", dest="output", help="define the XML output file")
    optParser.add_option("-t", "--tag", dest="tag", help="tag to edit")
    optParser.add_option("-a", "--attribute", dest="attribute", help="attribute to edit")
    optParser.add_option("-r", "--remove-values", dest="values",
                         help="comma-separated list of values to filter by (deletes all occurences of tag if not specified)")  # noqa
    optParser.add_option("-k", "--keep-values", dest="keepValues",
                         help="comma-separated list of values to keep (deletes all non-matching elements")  # noqa
    (options, args) = optParser.parse_args(args=args)
    if not options.file:
        optParser.print_help()
        sys.exit()

    if options.values is not None:
        options.values = set(options.values.split(','))
    if options.keepValues is not None:
        options.keepValues = set(options.keepValues.split(','))
    return options


def traverseNodes(parent):
    for node in parent:
        yield parent, node
        for x in traverseNodes(node):
            yield x


def main(options):
    # parse tree
    tree = ET.parse(options.file)

    toRemove = []
    for parent, node in traverseNodes(tree.getroot()):
        # check tag
        if node.tag == options.tag:
            # continue depending of operation
            if options.keepValues is not None:
                if node.get(options.attribute) not in options.keepValues:
                    toRemove.append((parent, node))
            elif options.values is None or node.get(options.attribute) in options.values:
                toRemove.append((parent, node))
    # write modified tree
    for parent, node in toRemove:
        parent.remove(node)
    tree.write(options.output)


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
