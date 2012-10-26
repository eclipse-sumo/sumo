#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-10-28
@version $Id$

This script is a test runner for the CityMobil scenario.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2011-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
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
