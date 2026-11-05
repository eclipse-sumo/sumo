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
import socket
import struct
import subprocess
import sys
import unittest
from contextlib import redirect_stdout

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', '..', 'tools'))

import sumolib  # noqa
import traci  # noqa
from traci import constants as tc  # noqa


def string(value):
    value = value.encode('utf8')
    return struct.pack('!i', len(value)) + value


def command(command_id, payload):
    return struct.pack('!BB', len(payload) + 2, command_id) + payload


def response(command_id, payload=b'', error=''):
    status = command(command_id, bytes([255 if error else 0]) + string(error))
    return struct.pack('!i', 4 + len(status) + len(payload)) + status + payload


class ReloadSubscriptionsTest(unittest.TestCase):
    def test_successful_reload_clears_all_cached_results(self):
        network = os.path.join(os.path.dirname(__file__), 'input_net.net.xml')
        args = ['-n', network, '--no-step-log', '--duration-log.disable']
        with redirect_stdout(io.StringIO()):
            traci.start([sumolib.checkBinary('sumo')] + args, stdout=subprocess.DEVNULL)
        con = traci.getConnection()
        try:
            con.route.add('route', ['edge'])
            con.vehicle.add('ego', 'route')
            con.simulationStep()
            con.vehicle.subscribe('ego', [tc.VAR_SPEED])
            con.vehicle.subscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 100., [tc.VAR_SPEED])
            con.simulation.subscribe([tc.VAR_TIME])
            vehicle_results = con.vehicle.getAllSubscriptionResults()
            context_results = con.vehicle.getAllContextSubscriptionResults()
            simulation_results = con.simulation.getAllSubscriptionResults()
            self.assertIn('ego', vehicle_results)
            self.assertIn('ego', context_results)
            self.assertIn('', simulation_results)
            con.load(args)
            self.assertEqual(con.simulation.getTime(), 0.)
            self.assertEqual(con.vehicle.getIDList(), ())
            self.assertEqual(con.vehicle.getAllSubscriptionResults(), {})
            self.assertEqual(con.vehicle.getAllContextSubscriptionResults(), {})
            self.assertEqual(con.simulation.getAllSubscriptionResults(), {})
            self.assertEqual(vehicle_results, {})
            self.assertEqual(context_results, {})
            self.assertEqual(simulation_results, {})
        finally:
            con.close()
            if con._process.poll() is None:
                con._process.kill()
                con._process.wait(timeout=5)

    def test_rejected_reload_preserves_cached_results(self):
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0))
            listener.listen(1)
            con = traci.connect(listener.getsockname()[1], host='127.0.0.1', numRetries=0)
            peer, _ = listener.accept()
        with peer:
            con._socket.settimeout(2)
            try:
                value = bytes([tc.VAR_SPEED, 0, tc.TYPE_DOUBLE]) + struct.pack('!d', 3.)
                variable_reply = command(tc.RESPONSE_SUBSCRIBE_VEHICLE_VARIABLE, string('ego') + b'\x01' + value)
                context_reply = command(tc.RESPONSE_SUBSCRIBE_VEHICLE_CONTEXT,
                                        string('ego') + struct.pack('!BBi', tc.CMD_GET_VEHICLE_VARIABLE, 1, 1)
                                        + string('ego') + value)
                peer.sendall(response(tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE, variable_reply)
                             + response(tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT, context_reply)
                             + response(tc.CMD_LOAD, error='Reload rejected'))
                con.vehicle.subscribe('ego', [tc.VAR_SPEED])
                con.vehicle.subscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 100., [tc.VAR_SPEED])
                with self.assertRaisesRegex(traci.TraCIException, 'Reload rejected'):
                    con.load(['-n', 'unavailable.net.xml'])
                self.assertEqual(con.vehicle.getAllSubscriptionResults(), {'ego': {tc.VAR_SPEED: 3.}})
                self.assertEqual(con.vehicle.getAllContextSubscriptionResults(), {'ego': {'ego': {tc.VAR_SPEED: 3.}}})
            finally:
                con._socket.close()


if __name__ == '__main__':
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(
        unittest.defaultTestLoader.loadTestsFromTestCase(ReloadSubscriptionsTest))
    if not result.wasSuccessful():
        sys.stderr.write(output.getvalue())
        sys.exit(1)
    print('TraCI reload subscription tests passed (%s tests).' % result.testsRun)
