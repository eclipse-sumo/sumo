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

import os
import subprocess
import sys
import xml.etree.ElementTree as ET

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa

NETWORK = os.path.join(os.environ['SUMO_HOME'], 'tests', 'complex', 'sumo',
                       'insertionCapacity', 'input_net.net.xml')


def run_case(label, times, step):
    recording = ET.Element('fcd-export')
    for index, time in enumerate(times):
        timestep = ET.SubElement(recording, 'timestep', time=str(time / 1000))
        # Discrete position changes and zero speed make future-entry waiting
        # and the final position observable without ongoing vehicle motion.
        ET.SubElement(timestep, 'vehicle', id='v', x=str(10 + index), y='-1.6',
                      pos=str(10 + index), lane='E0_0', speed='0', angle='90')
    ET.ElementTree(recording).write('input_state.xml')
    subprocess.check_call([sumolib.checkBinary('sumo'), '-n', NETWORK,
                           '--device.fcd-replay.file', 'input_state.xml',
                           '--default.departspeed', '0',
                           '--step-length', str(step / 1000), '--end', str((times[-1] + 2 * step + 1000) / 1000),
                           '--fcd-output', 'replayed.xml', '--no-step-log', '--no-duration-log',
                           '--no-warnings', '--xml-validation', 'never'])
    final_step = (times[-1] + step - 1) // step * step
    observed = []
    for timestep in ET.parse('replayed.xml').getroot():
        time = round(float(timestep.attrib['time']) * 1000)
        vehicle = timestep.find('vehicle')
        if time > final_step:
            assert vehicle is None, (label, step, time, 'vehicle remained after final sample')
            continue
        assert vehicle is not None, (label, step, time, 'missing vehicle')
        latest = max(index for index, recorded in enumerate(times) if recorded <= time)
        actual = float(vehicle.attrib['pos'])
        assert abs(actual - (10 + latest)) < 0.01, (label, step, time, actual, 10 + latest)
        observed.append(time)
    assert observed == list(range(0, final_step + step, step)), (label, step, observed)
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
