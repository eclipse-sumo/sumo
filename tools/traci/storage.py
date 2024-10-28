# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    storage.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09

from __future__ import print_function
from __future__ import absolute_import
import struct

from . import constants as tc


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
        assert t == tc.TYPE_INTEGER
        return i

    def readTypedByte(self):
        t, b = self.read("!BB")
        assert t == tc.TYPE_BYTE
        return b

    def readDouble(self):
        return self.read("!d")[0]

    def readTypedDouble(self):
        t, d = self.read("!Bd")
        assert t == tc.TYPE_DOUBLE
        return d

    def readLength(self):
        length = self.read("!B")[0]
        if length > 0:
            return length
        return self.read("!i")[0]

    def readString(self):
        length = self.read("!i")[0]
        s = self.read("!%ss" % length)[0].decode("utf8")
        try:
            return str(s)
        except UnicodeEncodeError:
            return s

    def readTypedString(self):
        t = self.read("!B")[0]
        assert t == tc.TYPE_STRING, "expected TYPE_STRING (%02x), found %02x." % (tc.TYPE_STRING, t)
        return self.readString()

    def readStringList(self):
        n = self.read("!i")[0]
        return tuple([self.readString() for i in range(n)])

    def readTypedStringList(self):
        t = self.read("!B")[0]
        assert t == tc.TYPE_STRINGLIST
        return self.readStringList()

    def readShape(self):
        length = self.readLength()
        return tuple([self.read("!dd") for i in range(length)])

    def readCompound(self, expectedSize=None):
        t, s = self.read("!Bi")
        assert t == tc.TYPE_COMPOUND
        assert expectedSize is None or s == expectedSize
        return s

    def ready(self):
        return self._pos < len(self._content)

    def getDebugString(self):
        return " ".join(["%02x" % (c if type(c) is int else ord(c)) for c in self._content[self._pos:]])
