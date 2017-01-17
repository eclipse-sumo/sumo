#!/usr/bin/env python
"""
@file    texttestDiff.py
@author  Jakob Erdmann
@date    2015-11-03
@version $Id$

This is meant to be used as a diff tool for the sumo public and internal tests by adding the line
----------------------------
diff_program:texttestDiff.py
----------------------------
in ~/.texttest/config

It runs the textual diff tool (vim by default) and, if the differing files are
sumo networks, opens both of them with sumo-gui.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR/TS, Germany

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
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
