#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @author  Mark Colley
# @date    2026-11-22

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib.sensors.inductive_loop as il  # noqa
import sumolib.xml as sxml  # noqa

elem = sxml.makeXMLObject("detector", {"id": "d1", "lane": "e0_0"}, pos=10.5)
print(elem.toXML().strip())
for loop in il.read(sys.argv[1]):
    print(loop.id, loop.lane, loop.pos, loop.frequency, sorted(loop.attributes.items()))
    print(loop.toXML().strip())
