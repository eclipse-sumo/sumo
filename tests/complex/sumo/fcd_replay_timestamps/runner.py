#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
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

import os
import subprocess
import sys
import xml.etree.ElementTree as ET

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib


def run_case(label, times, step):
    recording = ET.Element('fcd-export')
    for index, time in enumerate(times):
        timestep = ET.SubElement(recording, 'timestep', time=str(time / 1000))
        # Discrete position changes and zero speed make future-entry waiting
        # and the final position observable without ongoing vehicle motion.
        ET.SubElement(timestep, 'vehicle', id='v', x=str(10 + index), y='-1.6',
                      pos=str(10 + index), lane='road_0', speed='0', angle='90')
    ET.ElementTree(recording).write('input_state.xml')
    subprocess.check_call([sumolib.checkBinary('sumo'), '-n', 'input_net.net.xml',
                           '--device.fcd-replay.file', 'input_state.xml',
                           '--default.departspeed', '0',
                           '--step-length', str(step / 1000), '--end', str((times[-1] + 2 * step + 1000) / 1000),
                           '--fcd-output', 'replayed.xml', '--no-step-log', '--no-duration-log',
                           '--no-warnings', '--xml-validation', 'never'])
    observed = 0
    for timestep in ET.parse('replayed.xml').getroot():
        time = round(float(timestep.attrib['time']) * 1000)
        vehicle = timestep.find('vehicle')
        assert vehicle is not None, (label, step, time, 'missing vehicle')
        latest = max(index for index, recorded in enumerate(times) if recorded <= time)
        actual = float(vehicle.attrib['pos'])
        if label == 'single' and time > 0:
            # A single entry only initializes departure. The vehicle retains its
            # normal movement and removal behavior after that entry.
            assert actual >= 10, (label, step, time, actual)
        else:
            assert abs(actual - (10 + latest)) < 0.01, (label, step, time, actual, 10 + latest)
        observed += 1
    assert observed > 1, (label, step, 'no replay steps')
    print(f'{label} step={step / 1000} passed')


for label, times in [('aligned', list(range(0, 1001, 100))),
                     ('offgrid', [0, 70, 140, 490, 710, 1100]),
                     ('future', [0, 100, 1000, 2000]),
                     ('single', [0]),
                     ('duplicate', [0, 100, 500, 500, 1000])]:
    for step in [100, 500, 1000]:
        run_case(label, times, step)

# Existing default-step recordings retain every sample at its original time.
run_case('default', [0, 1000, 2000, 3000], 1000)
