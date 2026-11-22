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

# @file    runner.py
# @author  Mark Colley
# @date    2026-11-05

"""Exercise moveToXY numeric angles through the public vehicle and person API."""

import math
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
from traci import constants as tc  # noqa


def check_angles(kind):
    traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "--no-step-log"])
    try:
        domain = getattr(traci, kind)
        if kind == "vehicle":
            traci.route.add("route", ["edge"])
            domain.add("ego", "route", departPos="20")
        else:
            domain.add("ego", "edge", 20)
            domain.appendWalkingStage("ego", ["edge"], arrivalPos=90)
        traci.simulationStep()
        domain.setSpeed("ego", 0)

        def move(angle):
            x, y = domain.getPosition("ego")
            if kind == "vehicle":
                domain.moveToXY("ego", "edge", 0, x, y, angle=angle)
            else:
                domain.moveToXY("ego", "edge", x, y, angle=angle)

        for angle in (0, 90, 360, 450, -90, -720, 1080.5, 1e20, -1e20):
            move(angle)
            traci.simulationStep()
            assert math.isclose(domain.getAngle("ego"), angle % 360, abs_tol=1e-6), angle

        move(tc.INVALID_DOUBLE_VALUE)
        traci.simulationStep()
        assert math.isfinite(domain.getAngle("ego"))

        for angle in (float("nan"), float("inf"), -float("inf")):
            before = domain.getPosition("ego"), domain.getAngle("ego")
            try:
                move(angle)
            except traci.TraCIException as error:
                assert "finite" in str(error), error
            else:
                raise AssertionError(f"Non-finite angle was accepted: {angle!r}")
            traci.simulationStep()
            assert domain.getPosition("ego") == before[0]
            assert domain.getAngle("ego") == before[1]
        print(kind + ": wrapped finite angles, large angles, automatic heading and non-finite rejection passed")
    finally:
        traci.close()


for entity in ("vehicle", "person"):
    check_angles(entity)
