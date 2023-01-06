#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
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
# @date    2022-10-27

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess

TOOLS = os.path.join(os.environ["SUMO_HOME"], "tools")

subprocess.call([sys.executable,
                 os.path.join(TOOLS, "osmWebWizard.py"), "-b=7:0:0", "-e=8:0:0",
                 "--bbox=13.381507,52.511801,13.417790,52.527748",
                 "--test-output", "test", "-n=--aggregate-warnings=0", "--remote"])
subprocess.call([sys.executable,
                 os.path.join(TOOLS, "import", "gtfs", "gtfs2pt.py")] + sys.argv[1:])
