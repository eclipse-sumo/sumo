# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    color.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2012-12-04
# @version $Id$

from __future__ import absolute_import


class RGBAColor:

    def __init__(self, r, g, b, a=None):
        self.r = r
        self.g = g
        self.b = b
        self.a = a

    def toXML(self):
        if self.a is not None:
            return "%s,%s,%s,%s" % (self.r, self.g, self.b, self.a)
        else:
            return "%s,%s,%s" % (self.r, self.g, self.b)


def decodeXML(c):
    return RGBAColor(*[float(x) for x in c.split(",")])
