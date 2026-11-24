#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

import os
from pathlib import Path
import subprocess
import sys

tools = Path(os.environ["SUMO_HOME"]) / "tools"
sys.path.append(str(tools))
import sumolib  # noqa: E402

subprocess.run([sumolib.checkBinary("netconvert"), "--osm-files", "osm.xml",
                "--simple-projection", "--output.original-names", "--geometry.remove",
                "--junctions.join", "-o", "review.net.xml"], stdout=subprocess.DEVNULL, check=True)
Path("simulation.log").write_text("Error: Check lane '400_0' near junction 'cluster_2_3'.\n", encoding="utf-8")
subprocess.run([sys.executable, str(tools / "osmMapReview.py"), "--osm-file", "osm.xml",
                "--net-file", "review.net.xml", "--log-file", "simulation.log",
                "--latitude", "49.001", "--longitude", "8.001",
                "--observation", "Verify the connection at the selected junction.",
                "-o", "review.md"], check=True)
