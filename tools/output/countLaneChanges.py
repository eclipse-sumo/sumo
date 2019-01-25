#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    countLaneChanges.py
# @author  Jakob Erdmann
# @date    2014-09-16
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from xml.sax import parse, handler  # noqa


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


def countLaneChanges(dumpfile):
    dr = DumpReader()
    parse(dumpfile, dr)
    print(dr.changes, dr.changes / float(len(dr.vehicles)))


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("call %s <netstate-dump>" % sys.argv[0])
    countLaneChanges(*sys.argv[1:])
