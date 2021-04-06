#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
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
import optparse
try:
    import xml.etree.cElementTree as ET
except ImportError as e:
    print("recovering from ImportError '%s'" % e)
    import xml.etree.ElementTree as ET


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
    optParser.add_option("-v", "--value", dest="value", help="value to update (deletes attribute if not specified)")
    (options, args) = optParser.parse_args(args=args)
    if not options.file:
        optParser.print_help()
        sys.exit()
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
            else:
                # delete attribute
                del node.attrib[options.attribute]
    # write modified tree
    tree.write(options.output)


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
