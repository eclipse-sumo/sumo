# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    sumodetectors.py
# @author  Daniel Krajzewicz
# @date    2013-10-19
# @version $Id$


from __future__ import absolute_import


import cPickle
import sys
from xml.sax import saxutils, make_parser, handler


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
