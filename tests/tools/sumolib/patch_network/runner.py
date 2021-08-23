#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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
# @author  Jakob Erdmann
# @date

# test for patching network with random node position offset
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import random
from subprocess import call
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib.net  # noqa

random.seed(42)
netfile = sys.argv[1]
patchfile = 'nodes.nod.xml'
netconvert = sumolib.checkBinary('netconvert')
# disaggregate
call([netconvert, '-s', netfile, '--plain-output-prefix', 'plain'])
# create patch
with open(patchfile, 'w') as outf:
    # setting attrs is optional, it results in a cleaner patch file
    attrs = {'node': ['id', 'x', 'y']}  # other attrs are not needed for patching
    # parse always returns a generator but there is only one root element
    nodes = list(sumolib.xml.parse('plain.nod.xml', 'nodes', attrs))[0]
    for node in nodes.node:
        node.addChild("param", {"key": "origPos", "value": "%s %s" % (node.x, node.y)})
        node.x = float(node.x) + random.randint(-20, 20)
        node.y = float(node.y) + random.randint(-20, 20)
    outf.write(nodes.toXML())

# rebuild
call([netconvert, '-s', netfile, '-n', patchfile, '-o', 'net.net.xml'])
