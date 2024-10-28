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
# @date    2023-11-11


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
for dom in traci.DOMAINS:
    try:
        d = str(dom).replace("<traci._", "")
        d = d.replace("<class 'libsumo.libsumo.", "").replace("<class 'libtraci.libtraci.", "")
        d = d[:d.find(".")].replace("'", "")
        if d not in ('gui', 'simulation'):
            print(d)
            dom.getIDList()
    except traci.FatalTraCIError:
        print("Error as expected.")
traci.start([sumoBinary, "-c", "sumo.sumocfg"])
for dom in traci.DOMAINS:
    d = str(dom).replace("<traci._", "")
    d = d.replace("<class 'libsumo.libsumo.", "").replace("<class 'libtraci.libtraci.", "")
    d = d[:d.find(".")].replace("'", "")
    if d not in ('gui', 'simulation'):
        print(d)
        dom.getIDList()
traci.close()
