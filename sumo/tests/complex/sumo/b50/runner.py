#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@author  Laura Bieker
@date    2011-07-22
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,subprocess,sys,time,shutil
sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
netconvertBinary = os.environ.get("NETCONVERT_BINARY", os.path.join(sumoHome, 'bin', 'netconvert'))

subprocess.call([netconvertBinary, "-n", "input_nodes.nod.xml", "-e", "input_edges.edg.xml"], stdout=sys.stdout, stderr=sys.stderr)
subprocess.call([sumoBinary, "-c", "sumo.sumocfg", "-v"], stdout=sys.stdout, stderr=sys.stderr)
