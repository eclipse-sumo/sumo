#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import traci  # noqa
import sumolib  # noqa

sumoPort = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg", "--remote-port", str(sumoPort)])
conn = traci.connect(sumoPort, proc=sumoProcess)
conn.simulationStep()
conn.close()

sumoPort = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg", "--remote-port", str(sumoPort)])
conn = traci.connect(sumoPort, label="yo", proc=sumoProcess)
conn.simulationStep()
conn.close()
