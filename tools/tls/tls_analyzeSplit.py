#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    tls_check.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2009-08-01

"""
Compute the green split for all traffic light programs in the given file.
For actuated traffic lights, the <timedEvent>-output with type="SaveTLSProgram"
should be used.
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.xml import parse  # noqa


if len(sys.argv) < 2:
    print("Call: tls_check.py <FILE>", file=sys.stderr)
    sys.exit()

for tlLogic in parse(sys.argv[1], 'tlLogic'):
    durationsGg = None
    durationsG = None
    cycle = 0
    for phase in tlLogic.phase:
        if durationsG is None:
            durationsGg = [0] * len(phase.state)
            durationsG = [0] * len(phase.state)
        d = float(phase.duration)
        cycle += d
        for i,s in enumerate(phase.state):
            if s in 'Gg':
                durationsGg[i] += d
            if s == 'G':
                durationsG[i] += d
    relativeG = [int(d * 100 / cycle) for d in durationsG]
    relativeGg = [int(d * 100 / cycle) for d in durationsGg]
    print("tlLogic=%s program=%s cycle=%s G=%s Gg=%s" % (
        tlLogic.id, tlLogic.programID, cycle,
        relativeG,
        relativeGg))

