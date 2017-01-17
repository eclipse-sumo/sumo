"""
@file    polygon.py
@author  Daniel Krajzewicz
@author  Melanie Knocke
@author  Michael Behrisch
@date    2012-12-04
@version $Id$

Library for reading and storing polygons.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

from xml.sax import handler, parse
from .. import color


class Polygon:

    def __init__(self, id, type=None, color=None, layer=None, fill=None, shape=None):
        self.id = id
        self.type = type
        self.color = color
        self.layer = layer
        self.fill = fill
        self.shape = shape
        self.attributes = {}

    def getBoundingBox(self):
        xmin = self.shape[0][0]
        xmax = self.shape[0][0]
        ymin = self.shape[0][1]
        ymax = self.shape[0][1]
        for p in self.shape[1:]:
            xmin = min(xmin, p[0])
            xmax = max(xmax, p[0])
            ymin = min(ymin, p[1])
            ymax = max(ymax, p[1])
        assert(xmin != xmax or ymin != ymax)
        return xmin, ymin, xmax, ymax

    def getShapeString(self):
        return " ".join([",".join(map(str, e)) for e in self.shape])

    def toXML(self):
        ret = '<poly id="%s"' % self.id
        if type is not None:
            ret += ' type="%s"' % self.type
        if color is not None:
            ret += ' color="%s"' % self.color.toXML()
        if layer is not None:
            ret += ' layer="%s"' % self.layer
        if fill is not None:
            ret += ' fill="%s"' % self.fill
        if shape is not None:
            ret += ' shape="%s"' % self.getShapeString()
        if len(self.attributes) == 0:
            ret += '/>'
        else:
            ret += '>'
            for a in self.attributes:
                ret += '<param key="%s" value="%s"/>' % (a, self.attributes[a])
            ret += '</poly>'
        return ret

    def __lt__(self, other):
        return self.id < other.id


class PolygonReader(handler.ContentHandler):

    def __init__(self, includeTaz=False):
        self._includeTaz = includeTaz
        self._id2poly = {}
        self._polys = []
        self._lastPoly = None

    def startElement(self, name, attrs):
        if name == 'poly' or (self._includeTaz and name == 'taz'):
            cshape = []
            for e in attrs['shape'].split():
                p = e.split(",")
                cshape.append((float(p[0]), float(p[1])))
            if name == 'poly' and not self._includeTaz:
                c = color.decodeXML(attrs['color'])
                poly = Polygon(attrs['id'], attrs['type'], c, float(
                               attrs['layer']), attrs['fill'], cshape)
            else:
                poly = Polygon(attrs['id'], shape=cshape)
            self._id2poly[poly.id] = poly
            self._polys.append(poly)
            self._lastPoly = poly
        if name == 'param' and self._lastPoly != None:
            self._lastPoly.attributes[attrs['key']] = attrs['value']

    def endElement(self, name):
        if name == 'poly':
            self._lastPoly = None

    def getPolygons(self):
        return self._polys


def read(filename, includeTaz=False):
    polys = PolygonReader(includeTaz)
    parse(filename, polys)
    return polys.getPolygons()
