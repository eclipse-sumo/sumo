#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @author  Jakob Erdmann
# @date    2015-02-06


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:],
            traceFile="log.txt")
con = traci.getConnection()


def step():
    s = con.simulation.getTime()
    con.simulationStep()
    return s


def setGetParam(objectType, object, objectID):
    print(objectType, 'foo="%s"' % object.getParameter(objectID, "foo"))
    object.setParameter(objectID, "foo", "42")
    print(objectType, 'foo="%s"' % object.getParameter(objectID, "foo"))


print("step", step())

# XXX test PoI, Polygon
objects = [
    ("vehicle", con.vehicle, "veh0"),
    ("person", con.person, "ped0"),
    ("edge", con.edge, "1o"),
    ("lane", con.lane, "1o_0"),
    ("vType", con.vehicletype, "pType"),
    ("route", con.route, "horizontal"),
    ("trafficlight", con.trafficlight, "0"),
]

[setGetParam(*x) for x in objects]
print("step", step())

traci.close()
