#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @author  Leonhard Luecken
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa


def countWrittenTrips(fname):
    return len(list(sumolib.xml.parse_fast(fname, 'tripinfo', ['id'])))


def lastLine(fname):
    lines = open(fname).readlines()
    return None if len(lines) == 0 else lines[-1]


sumo = sumolib.checkBinary('sumo')
opts = [
    '-n', 'input_net.net.xml',
    '-r', 'input_routes.rou.xml',
    '--no-step-log',
    '--duration-log.statistics',
    '-S', '-Q',
]
traci.start([sumo] + opts + ['--tripinfo-output', 'tripinfos.xml', '-l', 'log']
            # + ['--save-configuration', 'debug.sumocfg']
            )


while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()

print("tripinfos at last step: %s" % countWrittenTrips('tripinfos.xml'))
print("logfile at last step: %s" % lastLine('log'))
traci.load(opts + ['--tripinfo-output', 'tripinfos2.xml', '-l', 'log2'])
traci.simulationStep()
print("tripinfos after load: %s" % countWrittenTrips('tripinfos.xml'))
print("logfile after load: %s" % lastLine('log'))

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
print("tripinfos2 at last step: %s" % countWrittenTrips('tripinfos2.xml'))
print("logfile2 at last step: %s" % lastLine('log2'))

traci.close()
print("tripinfos2 after close: %s" % countWrittenTrips('tripinfos2.xml'))
print("logfile2 after close: %s" % lastLine('log2'))
# done
