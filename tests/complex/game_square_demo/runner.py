#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-19

from __future__ import absolute_import
from __future__ import print_function
import os
import shutil
import subprocess
import sys

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..")))
GAME_DIR = os.path.join(SUMO_HOME, "tools", "game")
sys.path.insert(0, os.path.join(SUMO_HOME, "tools"))
sys.path.insert(0, GAME_DIR)
sys.argv.append("noupload")

import sumolib  # noqa
import runner as game_runner  # noqa

work_dir = os.path.join(os.getcwd(), "game_work")
os.makedirs(os.path.join(work_dir, "output"), exist_ok=True)
shutil.copytree(os.path.join(GAME_DIR, "square"), os.path.join(work_dir, "square"))
for fname in ("square.sumocfg", "square_demo.sumocfg", "input_additional.add.xml"):
    shutil.copy2(os.path.join(GAME_DIR, fname), os.path.join(work_dir, fname))
game_runner.BASE = work_dir


def run_game(scen):
    subprocess.check_call([
        sumolib.checkBinary("sumo"),
        "-c", scen + ".sumocfg",
        "--output-prefix", os.path.join("output", scen + "."),
        "--tripinfo-output.write-unfinished",
        "--no-step-log",
    ], cwd=work_dir)
    return game_runner.computeScore(scen)


game_runner.updateLocalMessages()
assert "square_demo" in game_runner._LANGUAGE_CAPTIONS
assert game_runner._SCORING_FUNCTION["square_demo"] is game_runner.computeScoreSquare

base_score, base_arrived, base_ok = run_game("square")
demo_score, demo_arrived, demo_ok = run_game("square_demo")
print("square: score=%s arrived=%s complete=%s" % (base_score, base_arrived, base_ok))
print("square_demo: score=%s arrived=%s complete=%s" % (demo_score, demo_arrived, demo_ok))
assert demo_ok and base_ok and demo_score > base_score
