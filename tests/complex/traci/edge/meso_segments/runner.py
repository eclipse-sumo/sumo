#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2024-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-19

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

# 1. Microscopic mode
traci.start([
    sumolib.checkBinary("sumo"),
    "--no-step-log",
    "-n", "net.net.xml",
] + sys.argv[1:])

first_edge = [e for e in traci.edge.getIDList() if not e.startswith(":")][0]
print("micro edge=%s num=%s offset0=%s" % (
    first_edge,
    traci.edge.getSegmentNumber(first_edge),
    traci.edge.getSegmentOffset(first_edge, 0) == traci.constants.INVALID_DOUBLE_VALUE,
))
traci.close()

# 2. Mesoscopic mode
traci.start([
    sumolib.checkBinary("sumo"),
    "--no-step-log",
    "-n", "net.net.xml",
    "--mesosim",
    "--meso-edgelength", "100",
] + sys.argv[1:])

for edge in [e for e in traci.edge.getIDList() if not e.startswith(":")][:4]:
    num = traci.edge.getSegmentNumber(edge)
    offsets = [round(traci.edge.getSegmentOffset(edge, idx), 2) for idx in range(num)]
    print("meso edge=%s num=%s offsets=%s" % (edge, num, offsets))

for bad_idx in (-1, traci.edge.getSegmentNumber(first_edge)):
    try:
        traci.edge.getSegmentOffset(first_edge, bad_idx)
    except traci.TraCIException as err:
        print("caught:", err)

traci.close()
