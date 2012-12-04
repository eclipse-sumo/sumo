"""
@file    poi.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2010-02-18
@version $Id$

Library for reading and storing PoIs.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2010-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

from xml.sax import handler, parse
from .. import color

class PoI:
    def __init__(self, id, type, layer, color, x, y, lane=None, pos=None):
        self.id = id
        self.type = type
        self.color = color
        self.layer = layer
        self.x = x
        self.y = y
        self.lane = lane
        self.pos = pos

    def toXML(self):
        if self.lane:
            return '<poi id="%s" type="%s" color="%s" layer="%s" lane="%s" pos="%s"/>' % (self.id, self.type, self.color.toXML(), self.layer, self.lane, self.pos)
        else:
            return '<poi id="%s" type="%s" color="%s" layer="%s" x="%s" y="%s"/>' % (self.id, self.type, self.color.toXML(), self.layer, self.x, self.y)


class PoIReader(handler.ContentHandler):
    def __init__(self):
        self._id2poi = {}
        self._pois = []

    def startElement(self, name, attrs):
        if name == 'poi':
            c = color.RGBAColor.decodeXML(attrs['color'])
            if not attrs.has_key('lane'):
                poi = PoI(attrs['id'], attrs['type'], int(attrs['layer']), attrs['color'], float(attrs['x']), float(attrs['y']))
            else:
                poi = PoI(attrs['id'], attrs['type'], int(attrs['layer']), attrs['color'], None, None, attrs['lane'], float(attrs['pos']))
            self._id2poi[poi._id] = poi
            self._pois.append(poi)


def read(filename):
    pois = PoIReader()
    parse(filename, pois)
    return pois._pois
