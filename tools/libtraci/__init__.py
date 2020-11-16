# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2018-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    __init__.py
# @author  Michael Behrisch
# @date    2020-10-08

from traci import connection, constants, exceptions, _vehicle, _person, _trafficlight, _simulation  # noqa
from traci.connection import StepListener  # noqa
from .libtraci import simulation
from .libtraci import *  # noqa


def isLibsumo():
    return False


def isLibtraci():
    return True


hasGUI = simulation.hasGUI
init = simulation.init
load = simulation.load
isLoaded = simulation.isLoaded
simulationStep = simulation.step
getVersion = simulation.getVersion
close = simulation.close
start = simulation.start
