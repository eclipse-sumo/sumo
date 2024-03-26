#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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

from __future__ import absolute_import
from __future__ import print_function


import os
import sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib.net  # noqa


def printSorted(d):
    return ' '.join(["%s:%s" % (k, d[k]) for k in sorted(d.keys())])


net = sumolib.net.readNet(sys.argv[1], withInternal=True, withLatestPrograms=True)
net = sumolib.net.readNet(sys.argv[2], withPrograms=True, net=net)

print("tlsParams",      printSorted(net.getTLS("C").getPrograms()["0"].getParams()))
print("tlsExtraParams", printSorted(net.getTLS("C").getPrograms()["extra"].getParams()))

