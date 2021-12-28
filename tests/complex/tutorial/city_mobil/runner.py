#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2021 German Aerospace Center (DLR) and others.
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
# @author  Daniel Krajzewicz
# @date    2011-10-28

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess

os.chdir('data')
subprocess.call([sys.executable, "createNet.py"])
subprocess.call([sys.executable, "simpleManager.py", "-t"])
subprocess.call([sys.executable, "agentManager.py", "-t"])
subprocess.call([sys.executable, "createNetTaxi.py"])
