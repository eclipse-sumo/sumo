#!/usr/bin/env python
"""
@file    addTaz.py
@author  Michael.Behrisch@dlr.de
@date    2009-05-20
@version $Id$

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
from xml.sax import make_parser, handler
from optparse import OptionParser

class RouteReader(handler.ContentHandler):

    def __init__(self):
        self._idToTaz = {}

    def startElement(self, name, attrs):
        if name == 'tripdef':
            self._idToTaz[attrs['id']] = (attrs['fromtaz'], attrs['totaz'])
        elif name != 'tripdefs':
            print '<' + name,
            for key in attrs.keys():
                print '%s="%s"' % (key, attrs[key]),
            if name == 'vehicle':
                print 'fromtaz="%s" totaz="%s"' % self._idToTaz[attrs['id']],
            print '>'

    def endElement(self, name):
        if name != 'tripdefs' and name != 'tripdef':
            print '</%s>' % name

optParser = OptionParser(usage="usage: %prog [options] <routefile>+")
optParser.add_option("-r", "--routes", help="routes file")
optParser.add_option("-t", "--trips", help="trips file")
(options, args) = optParser.parse_args()
parser = make_parser()
parser.setContentHandler(RouteReader())
parser.parse(options.trips)
parser.parse(options.routes)
