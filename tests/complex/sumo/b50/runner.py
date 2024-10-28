#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @author  Laura Bieker
# @date    2011-07-22

from __future__ import absolute_import

import os
import subprocess
import sys
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
netconvertBinary = sumolib.checkBinary('netconvert')

subprocess.call([netconvertBinary, "-n", "input_nodes.nod.xml",
                 "-e", "input_edges.edg.xml"], stdout=sys.stdout, stderr=sys.stderr)
subprocess.call(
    [sumoBinary, "-c", "sumo.sumocfg", "-v"], stdout=sys.stdout, stderr=sys.stderr)
