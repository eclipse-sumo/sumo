#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    10to11.py
# @author  Michael Behrisch
# @date    2009-06-09
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import sys

from xml.sax import make_parser, handler
from optparse import OptionParser


class ConfigReader(handler.ContentHandler):

    def __init__(self):
        self._parent = None
        self._parentWritten = ""
        self._element = None
        self._string = ''

    def startElement(self, name, attrs):
        self._parent = self._element
        self._element = name
        self._string = ''

    def characters(self, content):
        self._string += content

    def endElement(self, name):
        if self._parent == name:
            print('    </%s>' % name)
            self._parentWritten = ""
        elif self._element == name:
            if not self._parentWritten:
                print('    <%s>' % self._parent)
                self._parentWritten = self._parent
            print('        <%s value="%s"/>' % (self._element, self._string))
        self._element = self._parent

    def startDocument(self):
        print('<configuration>')

    def endDocument(self):
        if self._parentWritten:
            print('    </%s>' % self._parentWritten)
        print('</configuration>')

optParser = OptionParser(usage="usage: %prog <config>+")
(options, args) = optParser.parse_args()
if len(args) == 0:
    optParser.print_help()
    sys.exit()
for f in args:
    parser = make_parser()
    parser.setContentHandler(ConfigReader())
    parser.parse(f)
