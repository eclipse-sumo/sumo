#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    simpleManager.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2008-10-09

from __future__ import absolute_import
import vehicleControl
import statistics
from constants import DOUBLE_ROWS, WAIT_PER_PERSON


class SimpleManager(vehicleControl.Manager):

    def __init__(self):
        self.cyberCarLoad = {}
        self.personsWaitingAt = {}

    def personArrived(self, personID, edge, target):
        if edge not in self.personsWaitingAt:
            self.personsWaitingAt[edge] = []
        self.personsWaitingAt[edge].append((personID, target))

    def cyberCarArrived(self, vehicleID, edge):
        step = vehicleControl.getStep()
        footEdge = edge.replace("cyber", "footmain")
        wait = 0
        load = []
        for person, target in self.cyberCarLoad.get(vehicleID, []):
            if target == footEdge:
                statistics.personUnloaded(person, step)
                wait += WAIT_PER_PERSON
            else:
                load.append((person, target))
        while self.personsWaitingAt.get(footEdge, []) and len(load) < vehicleControl.getCapacity():
            person, target = self.personsWaitingAt[footEdge].pop(0)
            vehicleControl.leaveStop(person)
            statistics.personLoaded(person, step)
            load.append((person, target))
            wait += WAIT_PER_PERSON
        vehicleControl.leaveStop(vehicleID, delay=wait)
        if edge == "cyberout" or edge == "cyberin":
            row = -1
        else:
            row = int(edge[5])
        if row < DOUBLE_ROWS - 1:
            vehicleControl.stopAt(
                vehicleID, "cyber%sto%s" % (row + 1, row + 2))
        else:
            vehicleControl.stopAt(vehicleID, "cyberout")
        self.cyberCarLoad[vehicleID] = load


if __name__ == "__main__":
    vehicleControl.init(SimpleManager())
