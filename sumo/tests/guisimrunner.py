#!/usr/bin/env python
"""
@file    guisimrunner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@version $Id$

Wrapper script for GUI runs of the sumo tests.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, subprocess, sys, shutil

sumoBinary = os.path.join(os.path.dirname(sys.argv[1]), "sumo")
sumoBinary = os.environ.get("SUMO_BINARY", sumoBinary)
guisimBinary = sys.argv[1]

#start sumo to generate config
subprocess.call(sumoBinary + " --message-log guisim.stdout --error-log guisim.stderr --save-configuration guisim.sumocfg " + " ".join(sys.argv[2:]), 
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
ret = subprocess.call(guisimBinary + " -Q -c guisim.sumocfg", 
                      shell=True, stdout=sys.stdout, stderr=sys.stderr)
if os.path.exists("guisim.stderr"):
    f = open("guisim.stderr")
    shutil.copyfileobj(f, sys.stderr)
    f.close()
sys.exit(ret)
