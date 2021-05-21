#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    stateclient.py
# @author  Michael Behrisch
# @date    2021-05-20

import os
import sys
import socket
import time
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib
import traci
import stateserver

sumoBinary = sumolib.checkBinary("sumo-gui")

def read(client):
    buf = b''
    while True:
        chunk = client.recv(100)
        if chunk:
            buf += chunk
        else:
            return buf

def main():
    parser = sumolib.options.ArgumentParser()
    parser.add_argument("--net-file", dest="net", default="net.net.xml", help="network file")
    parser.add_argument("--state-file", dest="state", default="state.xml", help="filename for the temporary local state file")
    parser.add_argument("--host", default="localhost", help="host of the state server to connect to")
    parser.add_argument("--port", type=int, default=stateserver.PORT, help="Port for the state server.")
    options = parser.parse_args()

    traci.start([sumoBinary, "-n", options.net, "-S"])
    while True:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((options.host, options.port))
        with open(options.state, "wb") as clientstate:
            clientstate.write(read(client))
        traci.simulation.loadState(options.state)
        for _ in range(10):
            traci.simulationStep()
        time.sleep(1)


if __name__ == "__main__":
    main()
