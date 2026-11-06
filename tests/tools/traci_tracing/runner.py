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
# @author  Mark Colley
# @date    2026-11-05

import io
import os
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stdout

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'tools'))

import sumolib  # noqa
import traci  # noqa


class TracingTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.connections = []
        self.simulation = traci.simulation
        self.vehicle = traci.vehicle
        self.route = traci.route

    def tearDown(self):
        for con in reversed(self.connections):
            try:
                con.close()
            except traci.FatalTraCIError:
                pass
            if con._process.poll() is None:
                con._process.kill()
                con._process.wait(timeout=5)
        self.temporary.cleanup()

    def start(self, label, traced=True, doSwitch=True, traceGetters=True):
        trace = os.path.join(self.temporary.name, label + '.py') if traced else None
        network = os.path.join(os.path.dirname(__file__), 'input_net.net.xml')
        with redirect_stdout(io.StringIO()):
            traci.start([sumolib.checkBinary('sumo'), '-n', network, '--no-step-log',
                         '--duration-log.disable'], label=label, traceFile=trace,
                        traceGetters=traceGetters, doSwitch=doSwitch, stdout=subprocess.DEVNULL)
        con = traci.getConnection(label)
        self.connections.append(con)
        return con

    def trace(self, con):
        con._traceFile.flush()
        with open(con._traceFile.name) as stream:
            return stream.read()

    def test_direct_connections_remain_isolated(self):
        a = self.start('traced-a')
        a.simulationStep(2.)
        b = self.start('traced-b')
        b.simulationStep(5.)
        self.assertEqual(a.simulation.getTime(), 2.)
        self.assertEqual(b.simulation.getTime(), 5.)
        traci.switch('traced-a')
        self.assertEqual(b.simulation.getTime(), 5.)
        self.assertEqual(a.simulation.getTime(), 2.)

    def test_retained_global_domains_trace_selected_connection_only(self):
        before_trace = self.simulation.getTime
        a = self.start('global-a')
        a.simulationStep(2.)
        during_trace = self.simulation.getTime
        b = self.start('global-b')
        b.simulationStep(5.)
        self.assertEqual(before_trace(), 5.)
        self.assertEqual(during_trace(), 5.)
        traci.switch('global-a')
        self.assertEqual(during_trace(), 2.)
        self.route.add('A-only', ['edge'])
        traci.switch('global-b')
        traci.route.add('B-only', ['edge'])
        self.simulation.getTime()
        self.assertEqual(self.vehicle.DOMAIN_ID, traci.constants.CMD_GET_VEHICLE_VARIABLE)
        self.assertIn('A-only', self.trace(a))
        self.assertNotIn('B-only', self.trace(a))
        self.assertIn('B-only', self.trace(b))
        self.assertNotIn('A-only', self.trace(b))

    def test_untraced_connection_after_trace_closes_without_switch(self):
        a = self.start('closed-trace')
        saved_global = self.simulation.getTime
        a.simulationStep(2.)
        a.close()
        self.connections.remove(a)
        with open(a._traceFile.name) as stream:
            previous = stream.read()
        b = self.start('untraced', traced=False, doSwitch=False)
        self.assertEqual(b.simulation.getTime(), 0.)
        b.simulationStep(4.)
        self.assertEqual(b.simulation.getTime(), 4.)
        traci.switch('untraced')
        self.assertEqual(self.simulation.getTime(), 4.)
        self.assertEqual(saved_global(), 4.)
        with open(a._traceFile.name) as stream:
            self.assertEqual(stream.read(), previous)

    def test_getters_can_be_excluded_after_switching(self):
        a = self.start('no-getters-a', traceGetters=False)
        a.simulationStep(2.)
        b = self.start('no-getters-b', traceGetters=False)
        b.simulationStep(5.)
        traci.switch('no-getters-a')
        self.assertEqual(self.simulation.getTime(), 2.)
        self.assertEqual(b.simulation.getTime(), 5.)
        self.assertNotIn('getTime', self.trace(a))
        self.assertNotIn('getTime', self.trace(b))


if __name__ == '__main__':
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(unittest.defaultTestLoader.loadTestsFromTestCase(TracingTest))
    if not result.wasSuccessful():
        sys.stderr.write(output.getvalue())
        sys.exit(1)
    print('TraCI tracing tests passed (%s tests).' % result.testsRun)
