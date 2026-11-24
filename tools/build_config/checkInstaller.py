#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkInstaller.py
# @author  Josh
# @date    2026-11-26

"""Run a bounded simulation with the supplied installed SUMO executable."""

import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
from argparse import ArgumentParser
from pathlib import Path


def main():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    executable = parser.parse_args().executable.resolve()
    source = Path(__file__).resolve().parents[2]
    config = source / "tests/complex/tutorial/quickstart/data/quickstart.sumocfg"
    with tempfile.TemporaryDirectory() as directory:
        tripinfo = Path(directory) / "tripinfo.xml"
        subprocess.run(
            [
                str(executable),
                "-c",
                str(config),
                "--no-step-log",
                "--tripinfo-output",
                str(tripinfo),
            ],
            cwd=directory,
            check=True,
            timeout=60,
        )
        trips = ET.parse(tripinfo).getroot().findall("tripinfo")
        if not trips:
            sys.exit("SUMO quickstart simulation completed without any trips.")
        print(f"SUMO quickstart simulation completed {len(trips)} trips.")


if __name__ == "__main__":
    main()
