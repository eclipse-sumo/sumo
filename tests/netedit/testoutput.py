#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2025-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    testoutput.py
# @author  Pablo Alvarez Lopez
# @date    2025-12-05

import os
import re
import subprocess
import sys


def main():
    sumo = os.environ.get("SUMO_BINARY")
    if not sumo:
        print("Error: SUMO_BINARY must be defined", file=sys.stderr)
        sys.exit(2)
    sumo_cfg_in = "sumo_B.netedit" if os.path.exists("sumo_B.netedit") else "sumo.netedit"
    if os.path.exists(sumo_cfg_in):
        with open(sumo_cfg_in) as cfg_in, open("sumo.sumocfg", "w") as cfg_out:
            # replace everything that ends with something similar to ".net.xml.gz" with "_net_xml_gz.netedit"
            cfg_out.write(re.sub(r'(A|B|\.(add|dat|dat\.add|net|rou))\.(txt|xml)(\.gz)?',
                                 lambda m: m.group(0).replace('.', '_') + ".netedit", cfg_in.read()))
    rc = subprocess.run([sumo, "-c", "sumo.sumocfg", "--no-step-log", "--no-duration-log"]).returncode
    sys.exit(rc)


if __name__ == "__main__":
    main()
