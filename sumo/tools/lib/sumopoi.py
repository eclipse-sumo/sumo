#!/usr/bin/env python
"""
@file    sumopoi.py
@author  Daniel.Krajzewicz@dlr.de
@date    2010-02-18
@version $Id$

Library for reading and storing POIs.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""

import os, sys
from xml.sax import saxutils, make_parser, handler
from itertools import *


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



