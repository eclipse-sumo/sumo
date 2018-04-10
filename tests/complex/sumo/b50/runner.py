#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @author  Laura Bieker
# @date    2011-07-22
# @version $Id$

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
import sumolib  # noqa

sumoBinary = os.environ.get(
    "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
netconvertBinary = os.environ.get(
    "NETCONVERT_BINARY", os.path.join(sumoHome, 'bin', 'netconvert'))

subprocess.call([netconvertBinary, "-n", "input_nodes.nod.xml",
                 "-e", "input_edges.edg.xml"], stdout=sys.stdout, stderr=sys.stderr)
subprocess.call(
    [sumoBinary, "-c", "sumo.sumocfg", "-v"], stdout=sys.stdout, stderr=sys.stderr)
