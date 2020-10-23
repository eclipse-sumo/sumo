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

# @file    sumodetectors.py
# @author  Daniel Krajzewicz
# @date    2013-10-19


from __future__ import absolute_import


from xml.sax import handler, make_parser


class ArealReader(handler.ContentHandler):

    def __init__(self, toCollect):
        self._values = {}
        self._toCollect = toCollect
        self._lastInterval = -1
        for v in toCollect:
            self._values[v] = []

    def startElement(self, name, attrs):
        if name == 'interval':
            t = float(attrs['begin'])
            if self._lastInterval != t:
                self._lastInterval = t
                for a in self._toCollect:
                    self._values[a].append({})
            id = attrs['id']
            for a in self._toCollect:
                self._values[a][-1][id] = float(attrs[a])

    def get(self, name):
        return self._values[name]


def readAreal(inputFile, toCollect):
    parser = make_parser()
    areal = ArealReader(toCollect)
    parser.setContentHandler(areal)
    parser.parse(inputFile)
    return areal
