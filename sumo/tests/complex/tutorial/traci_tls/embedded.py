#!/usr/bin/env python
"""
@file    embedded.py
@author  Lena Kalleske
@author  Daniel Krajzewicz
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2009-03-26
@version $Id$

Tutorial for traffic light control via the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR/TS, Germany

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import os
import sys
import subprocess
# the embedded python does not add the current dir to the python path, so
# we need to do it
sys.path.append(os.path.dirname(__file__))
import runner


if runner.traci.isEmbedded():
    # this script has been called from the sumo-internal python interpreter
    # only execute the main control procedure
    runner.run()
else:
    options = runner.get_options()
    # this script has been called from the command line. It will start sumo with
    # this script as argument
    if options.nogui:
        sumoBinary = runner.checkBinary('sumo')
    else:
        # gui running probably does not work yet
        sumoBinary = runner.checkBinary('sumo-gui')

    # first, generate the route file for this simulation
    runner.generate_routefile()

    # call sumo with the request to run this very same script again in the internal interpreter
    # when this happens, the method traci.isEmbedded() in line 23 will evaluate to true
    # and then the run method will be called
    retCode = subprocess.call([sumoBinary, "-c", "data/cross.sumocfg", "--python-script", __file__],
                              stdout=sys.stdout, stderr=sys.stderr)
    sys.exit(retCode)
