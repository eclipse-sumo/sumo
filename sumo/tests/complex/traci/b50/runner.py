#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@author  Laura Bieker
@date    2011-07-22
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import os
import subprocess
import sys
import time
import shutil
sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib
import traci
PORT = 8765

sumoBinary = os.environ.get(
    "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
netconvertBinary = os.environ.get(
    "NETCONVERT_BINARY", os.path.join(sumoHome, 'bin', 'netconvert'))

subprocess.call([netconvertBinary, "-n", "input_nodes.nod.xml",
                 "-e", "input_edges.edg.xml"], stdout=sys.stdout, stderr=sys.stderr)
p = subprocess.Popen(
    [sumoBinary, "-c", "sumo.sumocfg", "-v", "-S", "-Q", "--remote-port", str(PORT)], stdout=sys.stdout, stderr=sys.stderr)
traci.init(PORT)
traci.simulationStep(200000)
for i in range(10):
    for j in range(10):
        traci.gui.screenshot('View #0', "test_%s_%s.png" % (i, j))
    traci.simulationStep()
traci.close()
p.wait()
