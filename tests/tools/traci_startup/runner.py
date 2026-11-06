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
import tempfile
import threading
import unittest
from contextlib import contextmanager, redirect_stdout
from unittest.mock import patch

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'tools'))

import sumolib  # noqa
import traci  # noqa
from traci import connection, main  # noqa


@contextmanager
def server_peer():
    with socket.socket() as listener:
        listener.bind(('127.0.0.1', 0))
        listener.listen(1)
        listener.settimeout(5)

        def answer():
            peer, _ = listener.accept()
            with peer:
                peer.settimeout(2)
                try:
                    request = peer.recv(4096)
                    if request:
                        # A successful status followed by an invalid GETVERSION response command.
                        peer.sendall(struct.pack('!iBBBiBB', 13, 7, 0, 0, 0, 2, 255))
                except socket.timeout:
                    pass

        thread = threading.Thread(target=answer, daemon=True)
        thread.start()
        try:
            yield listener.getsockname()[1]
        finally:
            thread.join(timeout=6)
            if thread.is_alive():
                raise AssertionError('Protocol peer did not finish')


class StartupTest(unittest.TestCase):
    def setUp(self):
        self.processes = []
        self.label = 'startup-test'
        self.popen = subprocess.Popen

    def tearDown(self):
        con = connection._connections.get(self.label)
        if con is not None:
            if con._socket is not None:
                con._socket.close()
            if con._traceFile is not None:
                con._traceFile.close()
            for key in ('', self.label):
                if connection._connections.get(key) is con:
                    del connection._connections[key]
        for process in self.processes:
            if process.poll() is None:
                process.kill()
            process.wait(timeout=5)
            if process.stdout is not None:
                process.stdout.close()

    def launch(self, *args, **kwargs):
        process = self.popen(*args, **kwargs)
        self.processes.append(process)
        return process

    def start(self, command, **kwargs):
        with patch.object(main.subprocess, 'Popen', side_effect=self.launch), redirect_stdout(io.StringIO()):
            return traci.start(command, numRetries=0, label=self.label, **kwargs)

    def test_connection_deadline_stops_owned_launcher(self):
        launcher = 'import os,sys,time; time.sleep(2); os.execv(sys.argv[1], sys.argv[1:])'
        network = os.path.join(os.path.dirname(__file__), 'input_net.net.xml')
        command = [sys.executable, '-c', launcher, sumolib.checkBinary('sumo'), '-n', network, '--no-step-log']
        with self.assertRaises(traci.FatalTraCIError):
            self.start(command)
        self.assertIsNotNone(self.processes[0].poll())
        self.assertFalse(connection.has(self.label))

    def test_invalid_trace_path_stops_owned_process(self):
        with tempfile.TemporaryDirectory() as temporary, server_peer() as port:
            with self.assertRaises(traci.FatalTraCIError):
                self.start([sys.executable, '-c', 'import time; time.sleep(60)'], port=port,
                           traceFile=os.path.join(temporary, 'missing', 'trace.py'))
        self.assertIsNotNone(self.processes[0].poll())
        self.assertFalse(connection.has(self.label))

    def test_invalid_version_clears_connection_and_owned_process(self):
        with server_peer() as port:
            with self.assertRaises(traci.FatalTraCIError):
                self.start([sys.executable, '-c', 'import time; time.sleep(60)'], port=port)
        self.assertIsNotNone(self.processes[0].poll())
        self.assertFalse(connection.has(self.label))
        self.assertFalse(traci.isLoaded())

    def test_connect_failure_preserves_external_process(self):
        process = self.launch([sys.executable, '-c', 'import time; time.sleep(60)'])
        with socket.socket() as unused:
            unused.bind(('127.0.0.1', 0))
            port = unused.getsockname()[1]
        with self.assertRaises(traci.FatalTraCIError), redirect_stdout(io.StringIO()):
            traci.connect(port=port, numRetries=0, host='127.0.0.1', proc=process)
        self.assertIsNone(process.poll())

    def test_successful_start_preserves_running_server(self):
        network = os.path.join(os.path.dirname(__file__), 'input_net.net.xml')
        with patch.object(main.subprocess, 'Popen', side_effect=self.launch), redirect_stdout(io.StringIO()):
            traci.start([sumolib.checkBinary('sumo'), '-n', network, '--no-step-log',
                         '--duration-log.disable'], numRetries=3, label=self.label, stdout=subprocess.DEVNULL)
        con = traci.getConnection(self.label)
        self.assertIsNone(self.processes[0].poll())
        self.assertEqual(con.simulation.getTime(), 0.)
        con.close()
        self.assertIsNotNone(self.processes[0].poll())

    @unittest.skipUnless(os.name == 'posix', 'SIGTERM handling requires POSIX signals')
    def test_unresponsive_launcher_is_killed(self):
        launcher = ('import signal,time; signal.signal(signal.SIGTERM, signal.SIG_IGN); '
                    'print("ready", flush=True); time.sleep(60)')

        def ready_process(*args, **kwargs):
            kwargs['stdout'] = subprocess.PIPE
            process = self.launch(*args, **kwargs)
            self.assertEqual(process.stdout.readline(), b'ready\n')
            return process

        with patch.object(main.subprocess, 'Popen', side_effect=ready_process), redirect_stdout(io.StringIO()):
            with self.assertRaises(traci.FatalTraCIError):
                traci.start([sys.executable, '-c', launcher], numRetries=0, label=self.label)
        self.assertIsNotNone(self.processes[0].poll())


if __name__ == '__main__':
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(unittest.defaultTestLoader.loadTestsFromTestCase(StartupTest))
    if not result.wasSuccessful():
        sys.stderr.write(output.getvalue())
        sys.exit(1)
    print('TraCI startup tests passed (%s tests).' % result.testsRun)
