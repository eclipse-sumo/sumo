#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @date    2020-08-21

from __future__ import absolute_import
from __future__ import print_function
from io import StringIO
import os
import sys
from xml.etree import ElementTree
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib.xml  # noqa


def parse_shape(value):
    return [tuple(map(int, pair.split(","))) for pair in value.split()]


def format_shape(points):
    return " ".join(",".join(map(str, point)) for point in points)


sample = '<edge from="a&amp;b" shape="0,1 2,3"><lane shape="4,5 6,7"/></edge>'
parsed = next(sumolib.xml.parse(StringIO(sample), outputLevel=0, attr_conversions={"shape": parse_shape}))
rendered = ElementTree.fromstring(parsed.toXML(attr_conversions={"shape": format_shape, "from": str.upper}))
assert rendered.attrib == {"from": "A&B", "shape": "0,1 2,3"}
assert rendered.find("lane").get("shape") == "4,5 6,7"
assert parsed.shape == [(0, 1), (2, 3)]


infile, outfile = sys.argv[1:]

xmlTree = list(sumolib.xml.parse(infile, outputLevel=0))[0]

with open(outfile, 'w') as outf:
    outf.write(xmlTree.toXML(indent='  '))
