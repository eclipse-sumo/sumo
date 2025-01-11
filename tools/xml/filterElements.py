#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
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
# @author  Johannes Rummel
# @date    2022-12-15

"""
removes elements by matching an attribute
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from lxml import etree as ET


if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(
        description="Remove elements in a XML file that match a certain attribute")
    optParser.add_argument("-f", "--file", category="input", dest="file", required=True, type=optParser.data_file,
                           help="define the XML input file")
    optParser.add_argument("-o", "--output", category="output", dest="output", type=optParser.data_file,
                           help="define the XML output file")
    optParser.add_argument("-t", "--tag", category="processing", dest="tag",
                           help="tag to edit")
    optParser.add_argument("-a", "--attribute", category="processing", dest="attribute",
                           help="attribute to edit")
    optParser.add_argument("-r", "--remove-values", category="processing", dest="values",
                           help="comma-separated list of values to filter by "
                                "(deletes all occurences of tag if not specified)")
    optParser.add_argument("-k", "--keep-values", category="processing", dest="keepValues",
                           help="comma-separated list of values to keep (deletes all non-matching elements")
    optParser.add_argument("-x", "--remove-interval", category="processing", dest="interval",
                           help="comma-separated begin and end values of interval to filter by "
                                "(deletes all occurences of tag if not specified)")
    optParser.add_argument("-i", "--keep-interval", category="processing", dest="keepInterval",
                           help="comma-separated begin an end values of interval to keep "
                                "(deletes all non-matching elements")
    options = optParser.parse_args(args=args)

    if options.values is not None:
        options.values = set(options.values.split(','))
    if options.keepValues is not None:
        options.keepValues = set(options.keepValues.split(','))
    if options.interval is not None:
        options.interval = [float(i) for i in options.interval.split(',')]
    if options.keepInterval is not None:
        options.keepInterval = [float(i) for i in options.keepInterval.split(',')]
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
            if options.attribute is not None:
                attribute_value = node.get(options.attribute)
                # continue depending of operation
                if options.keepValues is not None:
                    if attribute_value not in options.keepValues:
                        toRemove.append((parent, node))
                elif options.values is not None:
                    if attribute_value in options.values:
                        toRemove.append((parent, node))
                elif options.keepInterval is not None:
                    if (attribute_value is None or
                            not options.keepInterval[0] <= float(attribute_value) <= options.keepInterval[1]):
                        toRemove.append((parent, node))
                elif options.interval is not None:
                    if (attribute_value is not None and
                            options.interval[0] <= float(attribute_value) <= options.interval[1]):
                        toRemove.append((parent, node))
                else:  # nothing specified, delete all occurences of tag with the given attribute
                    toRemove.append((parent, node))
            else:  # nothing specified, delete all occurences of tag
                toRemove.append((parent, node))

    # write modified tree
    for parent, node in toRemove:
        parent.remove(node)
    tree.write(options.output)


if __name__ == "__main__":
    main(get_options())
