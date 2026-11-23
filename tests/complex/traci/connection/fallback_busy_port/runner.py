#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @author  Josh
# @date    2026-11-26

from contextlib import redirect_stdout
from io import StringIO
import os
import socket
import subprocess
import sys
from unittest.mock import patch
import warnings

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
from traci import main as traci  # noqa


def check_fallback():
    popen = subprocess.Popen
    get_free_port = traci.getFreeSocketPort
    processes = []
    ports = []

    def launch(args, **kwargs):
        process = popen(args, stderr=subprocess.PIPE, **kwargs)
        processes.append(process)
        return process

    with socket.socket() as reserved:
        if hasattr(socket, "SO_EXCLUSIVEADDRUSE"):
            reserved.setsockopt(socket.SOL_SOCKET, socket.SO_EXCLUSIVEADDRUSE, 1)
        reserved.bind(("", 0))
        busy_port = reserved.getsockname()[1]

        def choose_port():
            port = get_free_port() if ports else busy_port
            ports.append(port)
            return port

        try:
            with patch.object(traci, "getFreeSocketPort", side_effect=choose_port), \
                    patch.object(traci.subprocess, "Popen", side_effect=launch), \
                    redirect_stdout(StringIO()), warnings.catch_warnings(record=True) as recorded:
                warnings.simplefilter("always")
                traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml"] + sys.argv[1:],
                            numRetries=5, stdout=subprocess.DEVNULL)
            assert len(ports) >= 2 and ports[-1] != busy_port
            assert processes[0].returncode not in (None, 0)
            assert any("Retrying with different port" in str(warning.message) for warning in recorded)
            time_before_step = traci.simulation.getTime()
            traci.simulationStep()
            assert traci.simulation.getTime() > time_before_step
        finally:
            try:
                if traci.isLoaded():
                    traci.close()
            finally:
                for process in processes:
                    if process.poll() is None:
                        process.kill()
                    process.communicate()


check_fallback()
print("TraCI stepped the simulation after choosing a new port")
