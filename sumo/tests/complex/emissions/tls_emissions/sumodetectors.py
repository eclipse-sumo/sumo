"""
@file    sumodetectors.py
@author  Daniel Krajzewicz
@date    2013-10-19
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

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
