#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2011-07-01
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, subprocess, sys, time, shutil, random

sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import traci

PORT = random.randint(8000, 50000)

if sys.argv[1]=="sumo":
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = "--remote-port %s" % PORT
else:
    sumoBinary = os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-S -Q --remote-port %s" % PORT

sumoProcess = subprocess.Popen("%s -c sumo.sumocfg %s" % (sumoBinary, addOption), shell=True, stdout=sys.stdout, stderr=sys.stderr)
traci.init(PORT)
time.sleep(10)
step = 0
while not step>100:
    traci.simulationStep()
    vehs = traci.vehicle.getIDList()
    if vehs.index("horiz")<0 or len(vehs)>1:
        print "Something is false"
    step += 1
traci.close()
sumoProcess.wait()
sys.stdout.flush()
