#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
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
# @date    2011-10-28

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

os.chdir('data')
if "taxi" in sys.argv:
    subprocess.call([sys.executable, "createNetTaxi.py"])
    if "cyber" in sys.argv:
        subprocess.call([sumolib.checkBinary("sumo"), "park15_cyber.sumocfg"])
    else:
        subprocess.call([sumolib.checkBinary("sumo"), "park15.sumocfg"])
else:
    subprocess.call([sys.executable, "createNet.py"])
    options = ["-t"]
    if "cyber" in sys.argv:
        options.append("-c")
    if "agent" in sys.argv:
        subprocess.call([sys.executable, "agentManager.py"] + options)
    else:
        subprocess.call([sys.executable, "simpleManager.py"] + options)
