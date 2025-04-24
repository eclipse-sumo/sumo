#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2015-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generate_vscode_tasks.py
# @author  Michael Behrisch
# @date    2025-02-28

"""
This script creates the tasks.json and launch.json for easier debugging of SUMO with Visual Studio Code.
"""

import json
import os
from os.path import join, dirname

BINARIES = ("activitygen", "emissionsDrivingCycle", "emissionsMap",
            "dfrouter", "duarouter", "jtrrouter", "marouter",
            "netconvert", "netedit", "netgenerate",
            "od2trips", "polyconvert", "sumo", "sumo-gui",
            "testcommon", "testfoxtools", "testgeom", "testlibsumo", "testlibsumostatic",
            "testlibtraci", "testmicrosim", "testnetbuild", "testthreadpool",
            "TraCITestClient")

TASK_TEMPLATE = {
    "type": "cmake",
    "command": "build",
    "group": {
            "kind": "build",
            "isDefault": False
    }
}

LAUNCH_TEMPLATE = {
    "type": "cppdbg",
    "request": "launch",
    "args": [],
    "stopAtEntry": False,
    "cwd": "${workspaceFolder}/tests/",
    "environment": [],
    "externalConsole": False,
    "MIMode": "gdb",
    "setupCommands": [
        {
            "text": "-enable-pretty-printing",
            "ignoreFailures": True
        },
        {
            "text": "-gdb-set disassembly-flavor intel",
            "ignoreFailures": True
        }
    ]
}

LAUNCH_TEMPLATE_VS = {
    "type": "cppvsdbg",
    "request": "launch",
    "args": [],
    "stopAtEntry": False,
    "cwd": "${workspaceFolder}/tests/",
    "environment": [],
    "console": "integratedTerminal"
}


if __name__ == "__main__":
    print("This script will overwrite any existing tasks.json and launch.json."
          " If you don't want that abort now, otherwise press enter.")
    input()
    tasks_content = {"version": "2.0.0", "tasks": []}
    launch_content = {"version": "0.2.0", "configurations": []}
    for app in BINARIES:
        t = dict(TASK_TEMPLATE)
        t.update(label="build " + app, targets=[app])
        tasks_content["tasks"].append(t)
        lc = dict(LAUNCH_TEMPLATE_VS if os.name == "nt" else LAUNCH_TEMPLATE)
        lc.update(name=app + "D", program="${workspaceFolder}/bin/%sD" % app, preLaunchTask="build " + app)
        if "test" not in app.lower():
            lc.update(args=["test.%scfg" % app[:4]])
        launch_content["configurations"].append(lc)
    vscode_dir = join(dirname(__file__), '..', '..', '.vscode')
    os.makedirs(vscode_dir, exist_ok=True)
    with open(join(dirname(__file__), '..', '..', '.vscode', "tasks.json"), "w") as tasks:
        json.dump(tasks_content, tasks, indent=2)
    with open(join(dirname(__file__), '..', '..', '.vscode', "launch.json"), "w") as launch:
        json.dump(launch_content, launch, indent=2)
