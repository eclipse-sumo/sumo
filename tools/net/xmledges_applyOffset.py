#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    xmledges_applyOffset.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2009-08-01

"""
Applies a given offset to edges given in an xml-edge-file.
The results are written into <XMLEDGES>.mod.xml.
Call: xmledges_applyOffset.py <XMLEDGES> <X-OFFSET> <Y-OFFSET>
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
from xml.sax import saxutils, make_parser, handler


class XMLNodesReader(handler.ContentHandler):

    def __init__(self, outFileName, xoff, yoff):
        self._out = open(outFileName, 'w')
        self._xoff = xoff
        self._yoff = yoff

    def endDocument(self):
        self._out.close()

    def startElement(self, name, attrs):
        self._out.write('<' + name)
        for (key, value) in attrs.items():
            if key == "shape":
                shape = value.split(" ")
                nshape = []
                for s in shape:
                    (x, y) = s.split(",")
                    nshape.append(
                        str(float(x) + self._xoff) + "," + str(float(y) + self._yoff))
                self._out.write(' %s="%s"' %
                                (key, saxutils.escape(" ".join(nshape))))
            else:
                self._out.write(' %s="%s"' % (key, saxutils.escape(value)))
        self._out.write('>')

    def endElement(self, name):
        self._out.write('</' + name + '>')

    def characters(self, content):
        self._out.write(saxutils.escape(content))

    def ignorableWhitespace(self, content):
        self._out.write(content)

    def processingInstruction(self, target, data):
        self._out.write('<?%s %s?>' % (target, data))


if len(sys.argv) < 4:
    print("Usage: " + sys.argv[0] + " <XMLEDGES> <X-OFFSET> <Y-OFFSET>")
    sys.exit()
parser = make_parser()
reader = XMLNodesReader(
    sys.argv[1] + ".mod.xml", float(sys.argv[2]), float(sys.argv[3]))
parser.setContentHandler(reader)
parser.parse(sys.argv[1])
