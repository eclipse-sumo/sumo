#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @author  Jakob Erdmann
# @date    2009-11-04

from __future__ import absolute_import

import os
import subprocess
import sys

idx = sys.argv.index(":")
saveParams = sys.argv[1:idx]
loadParams = sys.argv[idx + 1:]
if '--mesosim' in loadParams:
    saveParams.append('--mesosim')

# need to add runner.py again in options.complex.meso to ensure it is the
# last  entry
saveParams = [p for p in saveParams if 'runner.py' not in p]
loadParams = [p for p in loadParams if 'runner.py' not in p]

# print "save:", saveParams
# print "load:", loadParams

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', 'bin', 'sumo'))
# print "sumoBinary", sumoBinary
subprocess.call([sumoBinary] + saveParams,
                shell=(os.name == "nt"), stdout=sys.stdout, stderr=sys.stderr)
subprocess.call([sumoBinary] + loadParams,
                shell=(os.name == "nt"), stdout=sys.stdout, stderr=sys.stderr)
