#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-10-28
@version $Id$

This script is a test runner for the CityMobil scenario.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


import os, sys
import shutil
import vehicleControl, simpleManager, agentManager

# build/check network
import createNet
# perform simple scenario
vehicleControl.init(simpleManager.SimpleManager(), True)
# perform agent scenario
vehicleControl.init(agentManager.AgentManager(), True)
try: shutil.copy("all-the-results.txt", "../result2")
except: print "Missing 'all-the-results.txt'"
