#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2019-2020 German Aerospace Center (DLR) and others.
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
# @date    2019-05-01

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time
import xml.etree.ElementTree as ET

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
from sumolib import checkBinary  # noqa

traasJar = os.path.join(os.environ['SUMO_HOME'], "bin", "TraaS.jar")
assert(os.path.exists(traasJar))

webservice = subprocess.Popen(["java", "-jar", traasJar, "data/ws_config.xml"])
config = ET.parse("data/ws_config.xml")
time.sleep(5)  # give webservice some time to start
url = "http://%s:%s/%s?wsdl" % (config.find("host").attrib["value"], config.find("port").attrib["value"],
                                config.find("name").attrib["value"])
subprocess.call(["wsimport", "-keep", "-clientjar", "webservice.jar", url])
subprocess.call(["javac", "-cp", os.pathsep.join(["webservice.jar", traasJar]), "data/%s.java" % sys.argv[1]])
subprocess.call(["java", "-cp", os.pathsep.join(["webservice.jar", traasJar, "data"]), sys.argv[1]])
webservice.kill()
