# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    sumotripinfos.py
# @author  Daniel Krajzewicz
# @date    2013-10-19

from __future__ import absolute_import
from __future__ import print_function


from xml.sax import handler, make_parser

NOT_FLOAT_TRIPINFO_ATTRS = [
    "departLane", "arrivalLane", "devices", "vtype", "vaporized"]


class TripinfosReader(handler.ContentHandler):

    def __init__(self, toCollect):
        self._values = {}
        self._toCollect = toCollect
        for v in toCollect:
            self._values[v] = {}

    def startElement(self, name, attrs):
        if name == 'tripinfo':
            id = attrs['id']
            for a in self._toCollect:
                val = attrs[a]
                if a not in NOT_FLOAT_TRIPINFO_ATTRS:
                    try:
                        val = float(val)
                    except ValueError:
                        print("%s=%s" % (val, a))
                self._values[a][id] = val

    def get(self, name):
        return self._values[name]


def readTripinfos(inputFile, toCollect):
    parser = make_parser()
    tripinfos = TripinfosReader(toCollect)
    parser.setContentHandler(tripinfos)
    parser.parse(inputFile)
    return tripinfos
