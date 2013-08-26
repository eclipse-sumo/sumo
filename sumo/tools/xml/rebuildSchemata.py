#!/usr/bin/env python
"""
@file    rebuildSchemata.py
@author  Michael Behrisch
@date    2011-07-11
@version $Id$

Let all SUMO binaries write the schema for their config

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, sys, subprocess
homeDir = os.environ.get("SUMO_HOME", os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
binDir = os.environ.get("SUMO_BINDIR", os.path.join(homeDir, "bin"))
for exe in "activitygen dfrouter duarouter marouter jtrrouter netconvert netgenerate od2trips polyconvert sumo".split():
    if os.path.exists(os.path.join(binDir, exe)):
        subprocess.call([os.path.join(binDir, exe), "--save-schema", os.path.join(homeDir, "docs", "internet", "xsd" , exe+"Configuration.xsd")])
    else:
        print "Warning! %s not found." % exe
