#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
# @author  Michael Behrisch
# @date    2012-01-14

from __future__ import absolute_import

import os
import subprocess
import sys
import time
import threading
import socket
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"))
import sumolib  # noqa

PORT = 8088
sumoBinary = sumolib.checkBinary('sumo')


def main(bailOut=False, family=socket.AF_INET):
    sys.stdout.flush()
    # create an INET, STREAMing socket
    serversocket = socket.socket(family, socket.SOCK_STREAM)
    serversocket.bind(("localhost", PORT))
    serversocket.listen(5)
    clientsocket, _ = serversocket.accept()
    while True:
        # accept connections from outside
        # now do something with the clientsocket
        msg = b''
        while len(msg) < 100:
            chunk = clientsocket.recv(100 - len(msg))
            if bailOut:
                clientsocket.close()
                serversocket.close()
                return
            if chunk == b'':
                sys.stdout.write(msg.decode("UTF8"))
                clientsocket.close()
                serversocket.close()
                return
            msg += chunk
        sys.stdout.write(msg.decode("UTF8"))


threading.Thread(target=main).start()
time.sleep(1)
subprocess.call([sumoBinary, "sumo.sumocfg"])

threading.Thread(target=lambda: main(family=socket.AF_INET6)).start()
time.sleep(1)
subprocess.call([sumoBinary, "-c", "sumo.sumocfg", "-a", "input_additional2.add.xml"])

threading.Thread(target=lambda: main(True)).start()
time.sleep(1)
subprocess.call([sumoBinary, "sumo.sumocfg"])
