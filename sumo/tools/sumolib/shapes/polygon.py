"""
@file    polygon.py
@author  Daniel Krajzewicz
@date    2012-12-04
@version $Id$

Library for reading and storing polygons.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2010-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

from xml.sax import handler, parse


class Polygon:
    def __init__(self, id, type, color, layer, fill, shape):
        self.id = id
        self.type = type
        self.color = color
        self.layer = layer
        self.x = fill
        self.y = shape
    def toXML(self):
        return '<poly id="%s" type="%s" color="%s" layer="%s" fill="%s" shape="%s"/>' % (self.id, self.type, self.color.toXML(), self.layer, self.fill, self.shape)
