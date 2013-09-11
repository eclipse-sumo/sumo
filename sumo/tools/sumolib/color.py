"""
@file    color.py
@author  Daniel Krajzewicz
@date    2012-12-04
@version $Id$

Library for reading and encoding of colors.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2010-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from xml.sax import handler, parse


class RGBAColor:
    def __init__(self, r, g, b, a=None):
        self.r = r
        self.g = g
        self.b = b
        self.a = a
    def toXML(self):
        if self.a!=None:
            return "%s,%s,%s,%s" % (self.r, self.g, self.b, self.a)
        else:
            return "%s,%s,%s" % (self.r, self.g, self.b)
    def decodeXML(c):
        return [float(x) for x in c.split(",")]
        
          
