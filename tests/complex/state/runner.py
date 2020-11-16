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
from __future__ import print_function
import os
import subprocess
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib

compare = []
if '--compare' in sys.argv:
    cmpIdx = sys.argv.index('--compare')
    compare = dict(c.split(":") for c in sys.argv[cmpIdx + 1].split(","))
    del sys.argv[cmpIdx:cmpIdx + 2]
idx = sys.argv.index(":")
saveParams = sys.argv[1:idx]
loadParams = sys.argv[idx + 1:]
if '--mesosim' in loadParams:
    saveParams.append('--mesosim')

# need to add runner.py again in options.complex.meso to ensure it is the
# last entry
saveParams = [p for p in saveParams if 'runner.py' not in p]
loadParams = [p for p in loadParams if 'runner.py' not in p]

# print("save:", saveParams)
# print("load:", loadParams)

sumoBinary = sumolib.checkBinary("sumo")
saveOut = open("save.out", "w") if "stdout" in compare else sys.stdout
loadOut = open("load.out", "w") if "stdout" in compare else sys.stdout
saveErr = open("save.err", "w") if "stderr" in compare else sys.stderr
loadErr = open("load.err", "w") if "stderr" in compare else sys.stderr
subprocess.call([sumoBinary] + saveParams,
                shell=(os.name == "nt"), stdout=saveOut, stderr=saveErr)
subprocess.call([sumoBinary] + loadParams,
                shell=(os.name == "nt"), stdout=loadOut, stderr=loadErr)
if compare:
    for f in (saveOut, loadOut, saveErr, loadErr):
        f.flush()
    if "stdout" in compare:
        sys.stdout.write(open(saveOut.name).read())
        sys.stdout.write("".join(sumolib.fpdiff.fpfilter(open(saveOut.name).readlines(), open(loadOut.name).readlines()[int(compare["stdout"]):], 0.01)))
    if "stderr" in compare:
        sys.stderr.write(open(saveErr.name).read())
        sys.stderr.write("".join(sumolib.fpdiff.fpfilter(open(saveErr.name).readlines(), open(loadErr.name).readlines()[int(compare["stderr"]):], 0.01)))
