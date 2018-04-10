#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    sikulixTestRunner.py
# @author  Pablo Alvarez Lopez
# @date    2016-0-01
# @version $Id$

import socket
import os
import sys

# get enviroment values
SUMOFolder = os.environ.get('SUMO_HOME', '.')
neteditApp = os.environ.get('NETEDIT_BINARY', 'netedit')
textTestSandBox = os.environ.get('TEXTTEST_SANDBOX', '.')

# Write enviroment variables in currentEnvironment.tmp
file = open(SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "w")
file.write(neteditApp + "\n" + textTestSandBox)
file.close()

# Check status of sikulix Server
statusSocket = socket.socket()
try:
    statusSocket.connect(("localhost", 50001))
    statusSocket.send("GET / HTTP/1.1\n\n")
    statusReceived = statusSocket.recv(1024)
    statusSocket.close()
    # If status of server contains "200 OK", Sikulix server is ready, in other
    # case is ocuppied
    if "200 OK" not in statusReceived:
        sys.exit("Sikulix server not ready")
except:
    # Cannot connect to SikulixServer, then Sikulix Server isn't running
    sys.exit("Sikulix server isn't running")

# IMAGES
imagesSocket = socket.socket()
imagesSocket.connect(("localhost", 50001))
# From Sikuli 1.1.1 Path has to be fixed
SUMOFolderPathFixed = SUMOFolder.replace("c:\\", "")
imagesSocket.send("GET /images/" + SUMOFolderPathFixed +
                  "/tests/netedit/imageResources HTTP/1.1\n\n")
imagesReceived = (imagesSocket.recv(1024))
imagesSocket.close()
if "200 OK" not in imagesReceived:
    sys.exit("Error adding imageResources folder '" +
             SUMOFolderPathFixed + "/tests/netedit/imageResources'")

# SCRIPT
scriptSocket = socket.socket()
scriptSocket.connect(("localhost", 50001))
scriptSocket.send("GET /scripts/" + textTestSandBox + " HTTP/1.1\n\n")
scriptReceived = (scriptSocket.recv(1024))
scriptSocket.close()
if "200 OK" not in scriptReceived:
    sys.exit("Error adding script folder '" + textTestSandBox + "'")

# RUN
runSocket = socket.socket()
runSocket.connect(("localhost", 50001))
runSocket.send("GET /run/test.sikuli HTTP/1.1\n\n")
runReceived = (runSocket.recv(1024))
runSocket.close()
if "200 OK" not in runReceived:
    sys.exit("error running 'test.sikuli' %s" % runReceived)
