#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
import sumolib  # noqa


redirectStdout = False
redirectStderr = False
compare = []
if '--compare' in sys.argv:
    cmpIdx = sys.argv.index('--compare')
    for c in sys.argv[cmpIdx + 1].split(","):
        entry = c.split(":") + [0, 0]
        compare.append((entry[0], int(entry[1]), int(entry[2])))
        if entry[0] == "stdout":
            redirectStdout = True
        if entry[0] == "stderr":
            redirectStderr = True
    del sys.argv[cmpIdx:cmpIdx + 2]
idx = sys.argv.index(":")
saveParams = sys.argv[1:idx]
loadParams = sys.argv[idx + 1:]
# work around texttests limitation of removing duplicate options
if '--mesosim' in loadParams and '--mesosim' not in saveParams:
    saveParams.append('--mesosim')
if '--mesosim' in saveParams and '--mesosim' not in loadParams:
    loadParams.append('--mesosim')

# need to add runner.py again in options.complex.meso to ensure it is the
# last entry
saveParams = [p for p in saveParams if 'runner.py' not in p]
loadParams = [p for p in loadParams if 'runner.py' not in p]

# print("save:", saveParams)
# print("load:", loadParams)

sumoBinary = sumolib.checkBinary("sumo")
saveOut = open("save.out", "w") if redirectStdout else sys.stdout
loadOut = open("load.out", "w") if redirectStdout else sys.stdout
saveErr = open("save.err", "w") if redirectStderr else sys.stderr
loadErr = open("load.err", "w") if redirectStderr else sys.stderr
subprocess.call([sumoBinary] + saveParams,
                shell=(os.name == "nt"), stdout=saveOut, stderr=saveErr)
subprocess.call([sumoBinary] + loadParams,
                shell=(os.name == "nt"), stdout=loadOut, stderr=loadErr)
if compare:
    for f in (saveOut, loadOut, saveErr, loadErr):
        f.flush()
    for fileType, offsetSave, offsetLoad in compare:
        if fileType == "stdout":
            sys.stdout.write(open(saveOut.name).read())
            saveLines = open(saveOut.name).readlines()[offsetSave:]
            loadLines = open(loadOut.name).readlines()[offsetLoad:]
            sys.stdout.write("".join(sumolib.fpdiff.diff(saveLines, loadLines, 0.01)))
        elif fileType == "stderr":
            sys.stderr.write(open(saveErr.name).read())
            saveLines = open(saveErr.name).readlines()[offsetSave:]
            loadLines = open(loadErr.name).readlines()[offsetLoad:]
            sys.stderr.write("".join(sumolib.fpdiff.diff(saveLines, loadLines, 0.01)))
        else:
            with open(fileType + ".xml") as saved:
                saveLines = saved.readlines()
            saveLines = saveLines[saveLines.index("-->\n") + offsetSave:]
            with open(fileType + "2.xml") as loaded:
                loadLines = loaded.readlines()
            loadLines = loadLines[loadLines.index("-->\n") + offsetLoad:]
            sys.stdout.write("".join(sumolib.fpdiff.diff(saveLines, loadLines, 0.01)))
