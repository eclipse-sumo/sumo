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
from unittest.mock import Mock, patch

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'tools'))

import traci  # noqa
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


class ProtocolSocket:
    """A bounded socket peer that accepts framed requests and supplies wire replies."""

    def __init__(self, responses, max_write=65536, max_read=65536, send_error=None):
        self.responses = list(responses)
        self.max_write = max_write
        self.max_read = max_read
        self.send_error = send_error
        self.pending = bytearray()
        self.incoming = bytearray()
        self.requests = []
        self.closed = False

    def setsockopt(self, *args):
        pass

    def connect(self, address):
        pass

    def send(self, data):
        if self.send_error is not None:
            raise self.send_error
        length = min(len(data), self.max_write)
        self.pending.extend(data[:length])
        return length

    def sendall(self, data):
        while data:
            data = data[self.send(data):]

    def recv(self, maximum):
        if not self.incoming:
            if len(self.pending) < 4:
                raise AssertionError('Receive attempted before complete request header')
            length = struct.unpack('!i', self.pending[:4])[0]
            if len(self.pending) < length:
                raise AssertionError('Receive attempted before complete request body')
            self.requests.append(bytes(self.pending[:length]))
            del self.pending[:length]
            self.incoming.extend(self.responses.pop(0))
        count = min(maximum, self.max_read, len(self.incoming))
        data = bytes(self.incoming[:count])
        del self.incoming[:count]
        return data

    def close(self):
        self.closed = True


class TransportTest(unittest.TestCase):
    def connect(self, peer, label=None, process=None):
        with patch.object(connection.socket, 'socket', return_value=peer):
            con = connection.Connection('localhost', 8813, process, None, True, label)
        self.addCleanup(peer.close)
        if label is not None:
            self.addCleanup(connection._connections.pop, label, None)
            self.addCleanup(connection._connections.pop, '', None)
        return con

    def test_partial_write_sends_complete_frame(self):
        peer = ProtocolSocket([response(tc.CMD_SETORDER)], max_write=2)
        con = self.connect(peer)
        con.setOrder(4)
        self.assertEqual(peer.requests, [struct.pack('!iBBi', 10, 6, tc.CMD_SETORDER, 4)])

    def test_invalid_color_does_not_poison_next_command(self):
        peer = ProtocolSocket([response(tc.CMD_SETORDER)])
        con = self.connect(peer)
        with self.assertRaises(ValueError):
            con.vehicle.setColor('veh0', ('invalid', 0, 0))
        con.setOrder(4)
        self.assertEqual(len(peer.requests), 1)

    def test_invalid_command_header_does_not_poison_next_command(self):
        peer = ProtocolSocket([response(tc.CMD_SETORDER)])
        con = self.connect(peer)
        with self.assertRaises(struct.error):
            con._sendCmd(tc.CMD_SET_VEHICLE_VARIABLE, 256, 'veh0')
        con.setOrder(4)
        self.assertEqual(peer.requests, [struct.pack('!iBBi', 10, 6, tc.CMD_SETORDER, 4)])

    def test_fragmented_header_and_large_reply(self):
        ids = tuple('vehicle_%04i' % i for i in range(2000))
        value = struct.pack('!Bi', tc.TYPE_STRINGLIST, len(ids)) + b''.join(map(string, ids))
        payload = command(tc.RESPONSE_GET_VEHICLE_VARIABLE, bytes([tc.TRACI_ID_LIST]) + string('') + value)
        peer = ProtocolSocket([response(tc.CMD_GET_VEHICLE_VARIABLE, payload)], max_read=3)
        con = self.connect(peer)
        self.assertEqual(con.vehicle.getIDList(), ids)

    def test_eof_during_reply_is_fatal(self):
        peer = ProtocolSocket([b''])
        con = self.connect(peer)
        with self.assertRaises(traci.FatalTraCIError):
            con.setOrder(4)
        self.assertTrue(peer.closed)

    def test_close_after_eof_releases_active_label(self):
        label = 'transport-recovery'
        peer = ProtocolSocket([b''])
        con = self.connect(peer, label)
        connection.switch(label)
        with self.assertRaises(traci.FatalTraCIError):
            con.close()
        self.assertFalse(traci.isLoaded())
        self.assertFalse(connection.has(label))
        replacement = self.connect(ProtocolSocket([response(tc.CMD_CLOSE)]), label)
        replacement.close()

    def test_close_after_send_failure_releases_label(self):
        label = 'transport-send-failure'
        peer = ProtocolSocket([], send_error=socket.error('peer reset'))
        con = self.connect(peer, label)
        with self.assertRaisesRegex(socket.error, 'peer reset'):
            con.close()
        self.assertTrue(peer.closed)
        self.assertFalse(connection.has(label))

    def test_successful_close_respects_wait(self):
        for wait in (True, False):
            with self.subTest(wait=wait):
                process = Mock()
                con = self.connect(ProtocolSocket([response(tc.CMD_CLOSE)]), process=process)
                con.close(wait=wait)
                self.assertEqual(process.wait.call_count, int(wait))


if __name__ == '__main__':
    suite = unittest.defaultTestLoader.loadTestsFromTestCase(TransportTest)
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(suite)
    if not result.wasSuccessful():
        sys.stderr.write(output.getvalue())
        sys.exit(1)
    print('TraCI transport tests passed (%s tests).' % result.testsRun)
