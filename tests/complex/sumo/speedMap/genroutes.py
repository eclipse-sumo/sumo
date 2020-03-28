#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    genroutes.py
# @author  Michael Behrisch
# @date    2014-12-16

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess
import numpy
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt  # noqa

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

numVehs = 25
time = 300
routeFile = "input_routes.rou.xml"
stateFile = "state.xml"
cfg = open("test.sumocfg", 'w')
cfg.write('''<?xml version="1.0" encoding="UTF-8"?>

<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation=\
"http://sumo.dlr.de/xsd/sumoConfiguration.xsd">

    <input>
        <net-file value="input_net.net.xml"/>
        <route-files value="%s"/>
        <additional-files value="input_additional.add.xml"/>
        <netstate-dump value="%s"/>
    </input>

    <time>
        <begin value="0"/>
        <end value="%s"/>
    </time>

    <report>
        <xml-validation value="never"/>
        <no-duration-log value="true"/>
        <no-step-log value="true"/>
    </report>

</configuration>''' % (routeFile, stateFile, time))
cfg.close()

for model in ["Krauss", "KraussPS", "KraussOrig1", "BKerner", "SmartSK", "IDM", "IDMM", "Daniel1", "PWagner2009",
              "Wiedemann"]:
    routes = open(routeFile, 'w')
    routes.write('''<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation=\
"http://sumo.dlr.de/xsd/routes_file.xsd">
        <vType id="leader" maxSpeed="13.88" color="red" carFollowModel="%s"/>
        <vType id="follower" color="green" carFollowModel="%s"/>
        <route id="r0" edges="1/1to2/1 2/1to3/1 3/1to4/1 4/1to5/1 5/1to6/1 6/1to7/1"/>
        <vehicle id="0" type="leader" route="r0" depart="0" departPos="500"/>
    ''' % (model, model))
    for i in range(1, numVehs):
        routes.write('''     <vehicle id="%s" type="leader" route="r0" depart="0" departPos="%s"/>
    ''' % (i, 500 - i * 8))
    routes.write('</routes>\n')
    routes.close()
    subprocess.call(
        [sumolib.checkBinary("sumo"), cfg.name], stdout=sys.stdout, stderr=sys.stderr)

    matrix = numpy.empty([numVehs, time])
    for count, vehicle in enumerate(sumolib.output.parse(stateFile, "vehicle")):
        matrix[int(vehicle.id)][count / numVehs] = 3.6 * float(vehicle.speed)
    plt.imshow(matrix)
    plt.colorbar(orientation='horizontal')
    plt.savefig("%s.png" % model)
    plt.clf()
