"""
@file    poi.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2010-02-18
@version $Id$

Library for reading and storing POIs.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2010-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

from xml.sax import handler, parse


class PoI:
    def __init__(self, id, type, layer, color, pos, lane):
        self._id = id
        self._type = type
        self._layer = layer
        self._color = color
        self._pos = pos
        self._lane = lane


class PoIReader(handler.ContentHandler):
    def __init__(self):
        self._id2poi = {}
        self._pois = []

    def startElement(self, name, attrs):
        if name == 'poi':
            if not attrs.has_key('lane'):
                poi = PoI(attrs['id'], attrs['type'], int(attrs['layer']), attrs['color'], (float(attrs['x']), float(attrs['y'])), None)
            else:
                poi = PoI(attrs['id'], attrs['type'], int(attrs['layer']), attrs['color'], float(attrs['pos']), attrs['lane'])
            self._id2poi[poi._id] = poi
            self._pois.append(poi)


def readPois(filename):
    pois = PoIReader()
    parse(filename, pois)
    return pois._pois
