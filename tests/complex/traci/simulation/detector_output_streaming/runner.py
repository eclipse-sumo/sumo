#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

# @file    runner.py
# @date    2026-11-23

import csv
import os
from pathlib import Path
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


def check_sparse_values(filename, columns):
    with open(filename, newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream, delimiter=";"))
    for column in columns:
        assert rows[0][column] == "", (filename, column)
        assert any(row[column] != "" for row in rows[1:]), (filename, column)


traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "-a", "input_additional.add.xml",
             "--no-step-log", "--no-warnings", "--output.column-header", "full"] + sys.argv[1:])
files = [Path(name + ".csv") for name in ("traffic", "emissions", "noise", "periodic")]
try:
    sizes = {file: 0 for file in files}
    for step in (11, 21, 31, 51, 81):
        traci.simulationStep(step)
        for file in files:
            size = file.stat().st_size
            assert size > sizes[file], (file, step, size, sizes[file])
            sizes[file] = size
finally:
    traci.close()

check_sparse_values("traffic.csv", ("interval_edge_speed",))
check_sparse_values("emissions.csv", ("interval_edge_traveltime", "interval_edge_CO2_perVeh"))
check_sparse_values("noise.csv", ("interval_edge_traveltime",))
check_sparse_values("instant.csv", ("instantOut_gap", "instantOut_occupancy"))
print("Tabular detector output streamed with nullable fields.")
