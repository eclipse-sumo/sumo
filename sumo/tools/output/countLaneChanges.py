#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    countLaneChanges.py
@author  Jakob Erdmann
@date    2014-09-16
@version $Id$

Count the number of lane changes

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from xml.sax import parse, handler


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
