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
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Set or remove an attribute for the specified XML element.")
    optParser.add_argument("-f", "--file", category="input", dest="file", required=True,
                           type=optParser.data_file, help="define the XML input file")
    optParser.add_argument("-o", "--output", category="output", dest="output",
                           type=optParser.data_file, help="define the XML output file")
    optParser.add_argument("-t", "--tag", category="processing", dest="tag", help="tag to edit")
    optParser.add_argument("-a", "--attribute", category="processing", dest="attribute", help="attribute to edit")
    optParser.add_argument("-v", "--value", category="processing", dest="value",
                           help="value to update (deletes attribute if not specified)")
    options = optParser.parse_args(args=args)
    return options


def main(options):
    # parse tree
    tree = ET.parse(options.file)
    # iterate over all XML elements
    for node in tree.getroot():
        # check tag
        if node.tag == options.tag:
            # continue depending of operation
            if options.value is not None:
                # set new attribute (or modify existent)
                node.set(options.attribute, options.value)
            elif options.attribute in node.attrib:
                # delete attribute
                del node.attrib[options.attribute]
    # write modified tree
    tree.write(options.output)


if __name__ == "__main__":
    options = get_options()
    main(options)
