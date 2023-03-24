#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @author  Mirko Barthauer
# @date    2023-02-08


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa
import simpla  # noqa
from simpla import SimplaException  # noqa

binary = 'sumo-gui'
if 'nogui' in sys.argv:
    binary = 'sumo'

traci.start([sumolib.checkBinary(binary),
             '-c', 'sumo.sumocfg',
             '--step-length', '0.5'])

simpla.load("simpla.cfg.xml")

step = 0
platoonID = None
while traci.simulation.getMinExpectedNumber() > 0:
    if step % 50 == 0:
        if platoonID is None:
            platoonIDs = simpla.getPlatoonIDList("left0A0")
            if len(platoonIDs) > 0:
                platoonID = platoonIDs[0]
        else:
            print("Step %d" % step)
            print("\tsimpla platoon count: %d" % len(platoonIDs))
            print("\tsimpla platoon leaders: %s" % ", ".join(simpla.getPlatoonLeaderIDList()))
            print("\tsimpla platoon average length: %.2f" % simpla.getAveragePlatoonLength())
            print("\tsimpla platoon average speed: %.2f" % simpla.getAveragePlatoonSpeed())
            platoonInfo = simpla.getPlatoonInfo(platoonID)
            print("\tsimpla platoon info ID %d: %s" % (platoonID, str(platoonInfo)))
    traci.simulationStep()
    step += 1

traci.close()
