"""
@file    sumotripinfos.py
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
from __future__ import print_function


import cPickle
import sys
from xml.sax import saxutils, make_parser, handler

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
                    except:
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
