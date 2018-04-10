#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @date    2012-01-14
# @version $Id$

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


def main(bailOut=False):
    sys.stdout.flush()
    # create an INET, STREAMing socket
    serversocket = socket.socket(
        socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", PORT))
    serversocket.listen(5)
    (clientsocket, address) = serversocket.accept()
    while True:
        # accept connections from outside
        # now do something with the clientsocket
        msg = ''
        while len(msg) < 100:
            chunk = clientsocket.recv(100 - len(msg))
            if bailOut:
                return
            if chunk == '':
                sys.stdout.write(msg)
                return
            msg = msg + chunk
        sys.stdout.write(msg)

threading.Thread(target=main).start()
time.sleep(1)
subprocess.call([sumoBinary, "sumo.sumocfg"])

threading.Thread(target=lambda: main(True)).start()
time.sleep(1)
subprocess.call([sumoBinary, "sumo.sumocfg"])
