#!/usr/bin/env python
"""
@file    route_departOffset.py
@author  Daniel.Krajzewicz@dlr.de
@date    11.09.2009
@version $Id: route_departOffset.py 6858 2009-02-25 13:27:57Z behrisch $

Applies a given offset to the given route's departure time

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import sys, optparse, array
from xml.sax import make_parser, handler

class RouteReader(handler.ContentHandler):

    def __init__(self, offset, out):
        self._offset = offset
        self._out = out
        
    def startElement(self, name, attrs):
        self._out.write('<' + name)
        for a in attrs.keys():
            val = attrs[a]
            if a=="depart":
                val = str(int(val) + self._offset)
            if a=="id":
                val = val + "_" + self._offset
            self._out.write(' ' + a + '="' + val + '"')
        self._out.write('>')

    def endElement(self, name):
        self._out.write('</' + name + '>')

    def characters(self, content):
        self._out.write(content)


if len(sys.argv) < 4:
    print "Usage: route_departOffset.py <INPUT_FILE> <OUTPUT_FILE> <OFFSET>"
    sys.exit()
out = open(sys.argv[2], "w")
parser = make_parser()
parser.setContentHandler(RouteReader(int(sys.argv[3]), out))
parser.parse(sys.argv[1])

