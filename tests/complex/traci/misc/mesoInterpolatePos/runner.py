#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py

import os
import sys
import subprocess
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


def readQueue(path):
    root = ET.parse(path).getroot()
    result = []
    for data in root.findall("data"):
        lanes = data.find("lanes")
        entries = []
        if lanes is not None:
            for lane in lanes.findall("lane"):
                entries.append((lane.get("id"), lane.get("queueing_length"),
                                lane.get("queueing_count")))
        result.append((data.get("timestep"), tuple(sorted(entries))))
    return result


def run(name, fcd=False):
    queueFile = name + ".xml"
    args = [sumolib.checkBinary("sumo"), "-n", "net.net.xml", "-r", "routes.rou.xml",
            "--mesosim", "--meso-interpolate-pos", "--meso-junction-control",
            "--begin", "0", "--end", "300", "--seed", "1",
            "--queue-output", queueFile, "--queue-output.period", "1",
            "--no-step-log", "--no-warnings"]
    if fcd:
        args += ["--fcd-output", name + "_fcd.xml"]
    subprocess.run(args, check=True)
    return readQueue(queueFile)


try:
    withoutFcd = run("without_fcd")
    withFcd = run("with_fcd", True)
    print("queue timesteps: %s" % len(withoutFcd))
    print("queue output consistent: %s" % (withoutFcd == withFcd))
finally:
    for name in ("without_fcd.xml", "with_fcd.xml", "with_fcd_fcd.xml"):
        if os.path.exists(name):
            os.remove(name)
