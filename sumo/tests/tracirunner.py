#!/usr/bin/env python
"""
@file    tracirunner.py
@author  Friedemann Wesner
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2008-05-12
@version $Id$

Wrapper script for running TraCI tests with TextTest.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os
import subprocess
import sys
import time
THIS_DIR = os.path.abspath(os.path.dirname(__file__))
sys.path.append(os.path.join(THIS_DIR, '..', "tools"))
import sumolib

PORT = str(sumolib.miscutils.getFreeSocketPort())
server_args = sys.argv[1:] + ["--remote-port", PORT]
binaryDir, server = os.path.split(server_args[0])
#server_args[0] = "sumoD"

client = "TraCITestClient"
if server[-1] == "D":
    client += "D"
if os.name != 'posix':
    client += ".exe"
client_args = [os.path.join(binaryDir, client), "-def",
               "testclient.prog", "-o", "testclient_out.txt", "-p", PORT]

# start sumo as server
serverprocess = subprocess.Popen(
    server_args, stdout=sys.stdout, stderr=sys.stderr)
success = False
for retry in range(7):
    time.sleep(retry * retry)
    clientProcess = subprocess.Popen(
        client_args, stdout=sys.stdout, stderr=sys.stderr)
    if serverprocess.poll() != None and clientProcess.poll() == None:
        time.sleep(10)
        if serverprocess.poll() != None and clientProcess.poll() == None:
            print >> sys.stderr, "Client hangs but server terminated for unknown reason"
            clientProcess.kill()
            break
    if clientProcess.wait() == 0:
        success = True
        break

if success:
    serverprocess.wait()
else:
    print >> sys.stderr, "Server hangs and does not answer connection requests"
    serverprocess.kill()
