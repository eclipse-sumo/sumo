"""
@file    polygon.py
@author  Daniel Krajzewicz
@author  Melanie Knocke
@author  Michael Behrisch
@date    2012-12-04
@version $Id$

Library for reading and storing polygons.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from xml.sax import handler, parse
from .. import color


class Polygon:
    def __init__(self, id, type, color, layer, fill, shape):
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

    def toXML(self):
        s = []
        for e in self.shape:
            s.append("%s,%s" % (e[0], e[1]))
        ret = '<poly id="%s" type="%s" color="%s" layer="%s" fill="%s" shape="%s"' % (self.id, self.type, self.color.toXML(), self.layer, self.fill, " ".join(s))
        if len(self.attributes)==0:
            ret += '/>'
        else:
            ret += '>'
            for a in self.attributes:
                ret += '<param key="%s" value="%s"/>' % (a, self.attributes[a])
            ret += '</poly>'
        return ret

        
class PolygonReader(handler.ContentHandler):
    def __init__(self):
        self._id2poly = {}
        self._polys = []
        self._lastPoly = None

    def startElement(self, name, attrs):
        if name == 'poly':
            c = color.decodeXML(attrs['color'])
            s1 = attrs['shape'].strip().split(" ")
            cshape = []
            for e in s1:
                p = e.split(",")
                cshape.append((float(p[0]), float(p[1])))
            poly = Polygon(attrs['id'], attrs['type'], c, float(attrs['layer']), attrs['fill'], cshape)
            self._id2poly[poly.id] = poly
            self._polys.append(poly)
            self._lastPoly = poly
        if name == 'param' and self._lastPoly!=None:
            self._lastPoly.attributes[attrs['key']] = attrs['value']

    def endElement(self, name):
        if name == 'poly':
            self._lastPoly = None

    
def read(filename):
    polys = PolygonReader()
    parse(filename, polys)
    return polys._polys
