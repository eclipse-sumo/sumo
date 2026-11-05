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

"""Exercise moveToXY numeric angles through the public vehicle and person API."""

import contextlib
import io
import math
import os
import subprocess
import sys
import tempfile
import threading

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib
import traci
from traci import constants as tc


def check_angles(kind):
    port = sumolib.miscutils.getFreeSocketPort()
    with tempfile.TemporaryFile(mode="w+t") as errors:
        process = subprocess.Popen([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml",
                                    "--no-step-log", "--remote-port", str(port)],
                                   stdout=subprocess.DEVNULL, stderr=errors)
        # Bound failures in an older simulator without leaving a test-owned server running.
        timeout = threading.Timer(10, process.kill)
        timeout.start()
        connection = None
        try:
            with contextlib.redirect_stdout(io.StringIO()):
                connection = traci.connect(port, proc=process, numRetries=10, waitBetweenRetries=0.05)
            domain = getattr(connection, kind)
            if kind == "vehicle":
                connection.route.add("route", ["edge"])
                domain.add("ego", "route", departPos="20")
            else:
                domain.add("ego", "edge", 20)
                domain.appendWalkingStage("ego", ["edge"], arrivalPos=90)
            connection.simulationStep()
            domain.setSpeed("ego", 0)

            def move(angle):
                x, y = domain.getPosition("ego")
                if kind == "vehicle":
                    domain.moveToXY("ego", "edge", 0, x, y, angle=angle)
                else:
                    domain.moveToXY("ego", "edge", x, y, angle=angle)

            for angle in (0, 90, 360, 450, -90, -720, 1080.5, 1e20, -1e20):
                move(angle)
                connection.simulationStep()
                assert math.isclose(domain.getAngle("ego"), angle % 360, abs_tol=1e-6), angle

            move(tc.INVALID_DOUBLE_VALUE)
            connection.simulationStep()
            assert math.isfinite(domain.getAngle("ego"))

            for angle in (float("nan"), float("inf"), -float("inf")):
                before = domain.getPosition("ego"), domain.getAngle("ego")
                try:
                    move(angle)
                except traci.TraCIException as error:
                    assert "finite" in str(error), error
                else:
                    raise AssertionError(f"Non-finite angle was accepted: {angle!r}")
                connection.simulationStep()
                assert domain.getPosition("ego") == before[0]
                assert domain.getAngle("ego") == before[1]
            connection.close(False)
            connection = None
            assert process.wait(timeout=5) == 0
            print(kind + ": wrapped finite angles, large angles, automatic heading and non-finite rejection passed")
        finally:
            timeout.cancel()
            if process.poll() is None:
                process.terminate()
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.wait()
            if connection is not None:
                try:
                    connection.close(False)
                except traci.FatalTraCIError:
                    pass


for entity in ("vehicle", "person"):
    check_angles(entity)
