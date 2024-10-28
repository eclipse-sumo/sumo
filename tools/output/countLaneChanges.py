#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    countLaneChanges.py
# @author  Jakob Erdmann
# @date    2014-09-16

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from xml.sax import parse, handler
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.options import ArgumentParser  # noqa


class DumpReader(handler.ContentHandler):

    """Reads the dump file"""

    def __init__(self):
        self._edge = None
        self._lane = None
        self.vehicles = {}  # vehicle -> last edge id
        self.changes = 0

    def startElement(self, name, attrs):
        if name == 'edge':
            self._edge = attrs['id']
        elif name == 'lane':
            self._lane = attrs['id']
        elif name == 'vehicle':
            veh = attrs['id']
            prevEdge, prevLane = self.vehicles.get(veh, (None, None))
            if self._edge == prevEdge and self._lane != prevLane:
                self.changes += 1
            self.vehicles[veh] = (self._edge, self._lane)


def parse_args():
    optParser = ArgumentParser()
    optParser.add_argument("dumpfile", help="dump file path")
    return optParser.parse_args()


def countLaneChanges(dumpfile):
    dr = DumpReader()
    parse(dumpfile, dr)
    print(dr.changes, dr.changes / float(len(dr.vehicles)))


if __name__ == "__main__":
    countLaneChanges(parse_args().dumpfile)
