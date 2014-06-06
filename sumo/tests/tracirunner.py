#!/usr/bin/env python
"""
@file    tracirunner.py
@author  Friedemann Wesner
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2008-05-12
@version $Id$

Wrapper script for running TraCI tests with TextTest.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,subprocess,sys,time,random,socket

while True:
    try:
        p = random.randint(8000, 50000)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind(("localhost", p))
        s.close()
        break
    except socket.error:
        pass
PORT = str(p)
server_args = sys.argv[1:] + ["--remote-port", PORT]
binaryDir, server = os.path.split(server_args[0])
#server_args[0] = "sumoD"

client = "TraCITestClient"
if "64" in server:
    client += "64"
if server[-1] == "D":
    client += "D"
if os.name != 'posix':
    client += ".exe"
client_args = [os.path.join(binaryDir, client), "-def", "testclient.prog", "-o", "testclient_out.txt", "-p", PORT]

#start sumo as server
serverprocess = subprocess.Popen(server_args, stdout=sys.stdout, stderr=sys.stderr)
success = False
for retry in range(7):
    time.sleep(retry*retry)
    clientProcess = subprocess.Popen(client_args, stdout=sys.stdout, stderr=sys.stderr)
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
