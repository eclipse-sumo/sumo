# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    storage.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09
# @version $Id$

from __future__ import print_function
from __future__ import absolute_import
import struct

from . import constants as tc

_DEBUG = False


class Storage:

    def __init__(self, content):
        self._content = content
        self._pos = 0

    def read(self, format):
        oldPos = self._pos
        self._pos += struct.calcsize(format)
        return struct.unpack(format, self._content[oldPos:self._pos])

    def readInt(self):
        return self.read("!i")[0]

    def readTypedInt(self):
        t, i = self.read("!Bi")
        assert(t == tc.TYPE_INTEGER)
        return i

    def readDouble(self):
        return self.read("!d")[0]

    def readTypedDouble(self):
        t, d = self.read("!Bd")
        assert(t == tc.TYPE_DOUBLE)
        return d

    def readLength(self):
        length = self.read("!B")[0]
        if length > 0:
            return length
        return self.read("!i")[0]

    def readString(self):
        length = self.read("!i")[0]
        return str(self.read("!%ss" % length)[0].decode("latin1"))

    def readTypedString(self):
        t = self.read("!B")[0]
        assert(t == tc.TYPE_STRING)
        return self.readString()

    def readStringList(self):
        n = self.read("!i")[0]
        return tuple([self.readString() for i in range(n)])

    def readTypedStringList(self):
        t = self.read("!B")[0]
        assert(t == tc.TYPE_STRINGLIST)
        return self.readStringList()

    def readShape(self):
        length = self.readLength()
        return tuple([self.read("!dd") for i in range(length)])

    def readCompound(self, expectedSize=None):
        t, s = self.read("!Bi")
        assert(t == tc.TYPE_COMPOUND)
        assert(expectedSize is None or s == expectedSize)
        return s

    def ready(self):
        return self._pos < len(self._content)

    def printDebug(self):
        if _DEBUG:
            for char in self._content[self._pos:]:
                print("%03i %02x %s" % (ord(char), ord(char), char))
