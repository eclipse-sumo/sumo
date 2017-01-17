#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@date    2012-01-14
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import os
import subprocess
import sys
import time
import threading
import socket
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"))
import sumolib

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
