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
import sys
import unittest
from contextlib import contextmanager

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', '..', 'tools'))

from traci import connection, constants as tc  # noqa


def string(value):
    value = value.encode('utf8')
    return struct.pack('!i', len(value)) + value


def command(command_id, payload):
    length = len(payload) + 2
    if length <= 255:
        return struct.pack('!BB', length, command_id) + payload
    return struct.pack('!BiB', 0, length + 4, command_id) + payload


def response(command_id, payload=b''):
    status = command(command_id, b'\x00' + string(''))
    return struct.pack('!i', 4 + len(status) + len(payload)) + status + payload


def receive_exact(peer, size):
    result = bytearray()
    while len(result) < size:
        part = peer.recv(size - len(result))
        if not part:
            raise AssertionError('Client closed before completing its request')
        result.extend(part)
    return bytes(result)


@contextmanager
def connected_peer():
    with socket.socket() as listener:
        listener.bind(('127.0.0.1', 0))
        listener.listen(1)
        con = connection.Connection('127.0.0.1', listener.getsockname()[1], None, None, True)
        peer, _ = listener.accept()
    with peer:
        peer.settimeout(2)
        con._socket.settimeout(2)
        try:
            yield con, peer
        finally:
            con._socket.close()


class SubscriptionParametersTest(unittest.TestCase):
    def assert_request(self, peer, command_id, payload):
        length = struct.unpack('!i', receive_exact(peer, 4))[0]
        self.assertEqual(receive_exact(peer, length - 4), command(command_id, payload))

    def context_request(self, variable_bytes):
        return (struct.pack('!dd', tc.INVALID_DOUBLE_VALUE, tc.INVALID_DOUBLE_VALUE)
                + string('ego') + struct.pack('!Bd', tc.CMD_GET_VEHICLE_VARIABLE, 50.) + variable_bytes)

    def context_response(self, num_variables, values=b''):
        payload = (string('ego') + struct.pack('!BBi', tc.CMD_GET_VEHICLE_VARIABLE,
                                            num_variables, int(bool(values))) + values)
        return response(tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT,
                        command(tc.RESPONSE_SUBSCRIBE_VEHICLE_CONTEXT, payload))

    def test_context_string_parameter_and_result(self):
        values = string('nearby') + bytes([tc.VAR_PARAMETER, 0, tc.TYPE_STRING]) + string('answer')
        with connected_peer() as (con, peer):
            peer.sendall(self.context_response(1, values))
            con.vehicle.subscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 50.,
                                         [tc.VAR_PARAMETER], parameters={tc.VAR_PARAMETER: 'bridge.key'})
            self.assert_request(peer, tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT,
                                self.context_request(bytes([1, tc.VAR_PARAMETER, tc.TYPE_STRING])
                                                     + string('bridge.key')))
            self.assertEqual(con.vehicle.getContextSubscriptionResults('ego'),
                             {'nearby': {tc.VAR_PARAMETER: 'answer'}})

    def test_context_mixed_variables_and_parameter_types(self):
        variables = [tc.VAR_POSITION, tc.VAR_LEADER, tc.VAR_PARAMETER_WITH_KEY]
        parameters = {tc.VAR_LEADER: 25., tc.VAR_PARAMETER_WITH_KEY: ('s', 'bridge.key')}
        with connected_peer() as (con, peer):
            peer.sendall(self.context_response(len(variables)))
            con.vehicle.subscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 50., variables,
                                         parameters=parameters)
            expected = (bytes([3, tc.VAR_POSITION, tc.VAR_LEADER])
                        + struct.pack('!Bd', tc.TYPE_DOUBLE, 25.)
                        + bytes([tc.VAR_PARAMETER_WITH_KEY, tc.TYPE_STRING]) + string('bridge.key'))
            self.assert_request(peer, tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT, self.context_request(expected))

    def test_context_compound_distance_parameter(self):
        parameters = {tc.DISTANCE_REQUEST: ('tou', 2, (10., 20.), tc.REQUEST_DRIVINGDIST)}
        with connected_peer() as (con, peer):
            peer.sendall(self.context_response(1))
            con.vehicle.subscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 50.,
                                         [tc.DISTANCE_REQUEST], parameters=parameters)
            expected = (bytes([1, tc.DISTANCE_REQUEST]) + struct.pack('!BiBddB', tc.TYPE_COMPOUND, 2,
                        tc.POSITION_2D, 10., 20., tc.REQUEST_DRIVINGDIST))
            self.assert_request(peer, tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT, self.context_request(expected))

    def test_context_unicode_parameter_with_extended_command(self):
        key = 'bridge.\u03b4' * 40
        with connected_peer() as (con, peer):
            peer.sendall(self.context_response(1))
            con.vehicle.subscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 50.,
                                         [tc.VAR_PARAMETER], parameters={tc.VAR_PARAMETER: key})
            self.assert_request(peer, tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT,
                                self.context_request(bytes([1, tc.VAR_PARAMETER, tc.TYPE_STRING]) + string(key)))

    def test_context_without_parameters_and_unsubscribe(self):
        with connected_peer() as (con, peer):
            peer.sendall(self.context_response(1))
            con.vehicle.subscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 50., [tc.VAR_POSITION])
            self.assert_request(peer, tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT,
                                self.context_request(bytes([1, tc.VAR_POSITION])))
            peer.sendall(response(tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT))
            con.vehicle.unsubscribeContext('ego', tc.CMD_GET_VEHICLE_VARIABLE, 50.)
            self.assert_request(peer, tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT, self.context_request(b'\x00'))

    def test_variable_subscription_parameters_are_unchanged(self):
        values = (string('ego') + bytes([1, tc.VAR_PARAMETER, 0, tc.TYPE_STRING]) + string('answer'))
        with connected_peer() as (con, peer):
            peer.sendall(response(tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE,
                                  command(tc.RESPONSE_SUBSCRIBE_VEHICLE_VARIABLE, values)))
            con.vehicle.subscribe('ego', [tc.VAR_PARAMETER], parameters={tc.VAR_PARAMETER: 'bridge.key'})
            expected = (struct.pack('!dd', tc.INVALID_DOUBLE_VALUE, tc.INVALID_DOUBLE_VALUE)
                        + string('ego') + bytes([1, tc.VAR_PARAMETER, tc.TYPE_STRING]) + string('bridge.key'))
            self.assert_request(peer, tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE, expected)
            self.assertEqual(con.vehicle.getSubscriptionResults('ego'), {tc.VAR_PARAMETER: 'answer'})


if __name__ == '__main__':
    suite = unittest.defaultTestLoader.loadTestsFromTestCase(SubscriptionParametersTest)
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(suite)
    if not result.wasSuccessful():
        sys.stderr.write(output.getvalue())
        sys.exit(1)
    print('TraCI subscription parameter tests passed (%s tests).' % result.testsRun)
