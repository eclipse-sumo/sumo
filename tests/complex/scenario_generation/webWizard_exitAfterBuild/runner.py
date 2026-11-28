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
# @author  Josh
# @date    2026-11-26

import json
import os
import ssl
import sys
import threading
import time
from http.server import HTTPServer
from unittest.mock import patch
from urllib.request import Request, urlopen

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
import osmWebWizard  # noqa: E402


class OfflineBuilder:
    def __init__(self, data, local):
        self.origDir = os.getcwd()
        self.fail = data.get("fail", False)
        assert local

    def TL(self, message):
        return message

    def build(self):
        if self.fail:
            raise ssl.CertificateError("simulated download failure")
        self.report("Scenario built")

    def makeConfigFile(self):
        pass

    def createBatch(self):
        pass

    def openSUMO(self):
        pass


def readJSON(request):
    with urlopen(request, timeout=5) as response:
        return json.load(response)


def generate(url, oneShot, fail=False):
    data = json.dumps({"vehicles": {}, "fail": fail}).encode()
    reply = readJSON(Request(url + "build", data=data, headers={"Content-Type": "application/json"}))
    assert reply["status"] == "started" and reply["exitAfterBuild"] == oneShot
    expected = "complete failure" if fail else "complete success" if oneShot else "report Scenario built"
    deadline = time.monotonic() + 5
    while time.monotonic() < deadline:
        if readJSON(url + "progress")["data"] == expected:
            return
        time.sleep(0.01)
    raise AssertionError("Missing progress message: " + expected)


def checkServer(oneShot):
    options = osmWebWizard.get_options(["--exit-after-build"] if oneShot else [])
    started = threading.Event()
    servers = []

    def createServer(*args):
        server = HTTPServer(*args)
        servers.append(server)
        return server

    with patch.object(osmWebWizard, "Builder", OfflineBuilder), patch.object(
            osmWebWizard, "HTTPServer", side_effect=createServer), patch.object(
            osmWebWizard.webbrowser, "open", side_effect=lambda url: started.set()), patch.object(
            osmWebWizard, "watchdog"):
        serverThread = threading.Thread(target=osmWebWizard.main, args=(options,), daemon=True)
        serverThread.start()
        assert started.wait(5)
        url = osmWebWizard.webbrowser.open.call_args.args[0]
        try:
            if oneShot:
                generate(url, oneShot, fail=True)
                assert serverThread.is_alive()
                generate(url, oneShot)
                serverThread.join(5)
                assert not serverThread.is_alive()
                assert servers[0].fileno() == -1
                print("One-shot server exited after successful retry")
            else:
                generate(url, oneShot)
                generate(url, oneShot)
                assert serverThread.is_alive()
                print("Default server remained available after two scenarios")
        finally:
            if serverThread.is_alive():
                servers[0].shutdown()
                serverThread.join(5)


checkServer(False)
checkServer(True)
