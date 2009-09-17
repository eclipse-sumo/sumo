#!/usr/bin/env python
"""
@file    10to11.py
@author  Michael.Behrisch@dlr.de
@date    2009-06-09
@version $Id$

Transfers configuration files from character data to attribute style.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
import os, sys, re

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
            print '    </%s>' % name
            self._parentWritten = ""
        elif self._element == name:
            if not self._parentWritten:
                print '    <%s>' % self._parent
                self._parentWritten = self._parent
            print '        <%s value="%s"/>' % (self._element, self._string)
        self._element = self._parent

    def startDocument(self):
        print '<configuration>'

    def endDocument(self):
        if self._parentWritten:
            print '    </%s>' % self._parentWritten
        print '</configuration>'

optParser = OptionParser(usage="usage: %prog <config>+")
(options, args) = optParser.parse_args()
if len(args) == 0:
    optParser.print_help()
    sys.exit()
for f in args:
    parser = make_parser()
    parser.setContentHandler(ConfigReader())
    parser.parse(f)
