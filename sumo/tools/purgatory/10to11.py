#!/usr/bin/env python
"""
@file    10to11.py
@author  Michael Behrisch
@date    2009-06-09
@version $Id$

Transfers configuration files from character data to attribute style.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import re

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
