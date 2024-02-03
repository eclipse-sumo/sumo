#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    changeAttribute.py
# @author  Pablo Alvarez Lopez
# @date    2021-03-09

"""
sets / removes attribute for the specified xml element
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from lxml import etree as ET

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Set or remove an attribute for the specified XML element.")
    optParser.add_argument("-f", "--file", category="input", required=True,
                           type=optParser.data_file, help="define the XML input file")
    optParser.add_argument("-o", "--output", category="output", required=True,
                           type=optParser.data_file, help="define the XML output file")
    optParser.add_argument("-t", "--tag", required=False, help="tag to edit")
    optParser.add_argument("-a", "--attribute", required=True, help="attribute to edit")
    optParser.add_argument("-v", "--value", help="value to update (deletes attribute if not specified)")
    optParser.add_argument("-u", "--upper-limit", dest="maximum",
                           help="updates to this maximum value (reduces all greater values)")
    options = optParser.parse_args(args=args)
    return options


def traverseNodes(parent):
    for node in parent:
        yield parent, node
        for x in traverseNodes(node):
            yield x


def main(options):
    # parse tree
    tree = ET.parse(options.file)
    # iterate over all XML elements
    for parent, node in traverseNodes(tree.getroot()):
        # check tag (take all tags if it is not specified)
        if options.tag is None or node.tag == options.tag:
            # continue depending of operation
            if options.value is not None:
                # set new attribute (or modify existent)
                node.set(options.attribute, options.value)
            elif options.maximum is not None:
                attribute_value = node.get(options.attribute)
                if attribute_value is not None:
                    if float(attribute_value) > float(options.maximum):
                        node.set(options.attribute, options.maximum)
            elif options.attribute in node.attrib:
                # delete attribute
                del node.attrib[options.attribute]
    # write modified tree
    tree.write(options.output)


if __name__ == "__main__":
    main(get_options())
