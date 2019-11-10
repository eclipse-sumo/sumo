#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Robert Hilbrich
# @date    2019-11-10
# @version $Id$

from __future__ import absolute_import

import os
import subprocess
import sys
toolDir = os.path.join(
    os.path.dirname(__file__), '..', '..', '..', '..', "tools")
sys.path.append(toolDir)
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')

# Set the example environment variable
os.environ["NETFILENAME"] = "input_net"

# file output direct
subprocess.call([sumoBinary, "-c", "sumo.sumocfg"])

