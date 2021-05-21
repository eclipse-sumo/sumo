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

# @file    stateserver.py
# @author  Michael Behrisch
# @date    2021-05-20

import sys
import threading
import socket

PORT = 8088

def main(filename):
    # create an INET, STREAMing socket
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", PORT))
    serversocket.listen(5)
    while True:
        clientsocket, _ = serversocket.accept()
        def send():
            clientsocket.sendall(open(filename, "rb").read())
            clientsocket.close()
        threading.Thread(target=send).start()


if __name__ == "__main__":
    main(sys.argv[1])
