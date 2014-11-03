"""
@file    poi.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2010-02-18
@version $Id$

Library for reading and storing PoIs.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from xml.sax import handler, parse
from .. import color

class InductiveLoop:
    def __init__(self, id, lane, pos, frequency, file, friendlyPos=True):
        self.id = id
        self.lane = lane
        self.pos = pos
        self.frequency = frequency
        self.file = file
        self.friendlyPos = friendlyPos

    def toXML(self):
        return '<e1Detector id="%s" lane="%s" pos="%s" freq="%s" file="%s" friendlyPos="%s"/>' % (self.id, self.lane, self.pos, self.frequency, self.file, self.friendlyPos)


class InductiveLoopReader(handler.ContentHandler):
    def __init__(self):
        self._id2il = {}
        self._ils = []
        self._lastIL = None
        self.attributes = {}

    def startElement(self, name, attrs):
        if name == 'e1Detector':
            poi = InductiveLoop(attrs['id'], attrs['lane'], float(attrs['pos']), float(attrs['freq']), attrs['file'])
            self._id2il[poi.id] = poi
            self._ils.append(poi)
            self._lastIL = poi
        if name == 'param' and self._lastIL!=None:
            self._lastIL.attributes[attrs['key']] = attrs['value']

    def endElement(self, name):
        if name == 'e1Detector':
            self._lastIL = None

    
def read(filename):
    ils = InductiveLoopReader()
    parse(filename, ils)
    return ils._ils
