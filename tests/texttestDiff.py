#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    texttestDiff.py
# @author  Jakob Erdmann
# @date    2015-11-03
# @version $Id$

"""
This is meant to be used as a diff tool for the sumo public and internal tests by adding the line
----------------------------
diff_program:texttestDiff.py
----------------------------
in ~/.texttest/config

It runs the textual diff tool (vim by default) and, if the differing files are
sumo networks, opens both of them with sumo-gui.
"""
import os
import sys
import subprocess

if len(sys.argv) == 4:
    basedir, new = sys.argv[2:]
    orig = os.path.join(basedir, new)
    os.environ["GUISIM_BINARY"] = "sumo-gui"
else:
    orig, new = sys.argv[1:]


subprocess.Popen(["tkdiff", orig, new])

if ("net.netgen" in orig
        or "net.netconvert" in orig
        or "net.scenario" in orig
        or "net2.scenario" in orig
        or "net.complex" in orig
        or "net.tools" in orig
        or "osmimport.tools" in orig
        or ".net.xml" in orig
    ):
    sumo = os.environ["GUISIM_BINARY"]
    extraArgs = []
    #extraArgs += ["--gui-settings-file", "/scr2/debug/000_view_settings/internal_junctions.xml"]
    #extraArgs += ["--gui-settings-file", "/scr2/debug/000_view_settings/junctions.xml"]
    #extraArgs += ["--gui-settings-file", "/scr2/debug/000_view_settings/junctionType.xml"]
    subprocess.Popen([sumo, "-n", orig, "-e", "3600"] + extraArgs)
    subprocess.Popen([sumo, "-n", new, "-e", "3600"] + extraArgs)
