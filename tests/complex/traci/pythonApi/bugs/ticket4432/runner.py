#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Leonhard Luecken
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Daniel Krajzewicz
# @date    2011-03-04
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))

import traci  # noqa
import sumolib

sumo = sumolib.checkBinary('sumo')

def countWrittenTrips(fname):
    return len(list(sumolib.xml.parse_fast(fname, 'tripinfo', ['id'])))

opts = [
    '-n', 'input_net.net.xml',
    '-r', 'input_routes.rou.xml',
    '--no-step-log',
    '-S', '-Q',
    ]

traci.start([sumo] + opts + ['--tripinfo-output', 'tripinfos.xml'])


while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep();

print("tripinfos at last step: %s" % countWrittenTrips('tripinfos.xml'))
traci.load(opts + ['--tripinfo-output', 'tripinfos2.xml'])
print("tripinfos after load: %s" % countWrittenTrips('tripinfos.xml'))

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep();
print("tripinfos2 at last step: %s" % countWrittenTrips('tripinfos2.xml'))

traci.close();
print("tripinfos2 after close: %s" % countWrittenTrips('tripinfos2.xml'))

# done
traci.close()
