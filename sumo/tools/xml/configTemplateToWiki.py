#!/usr/bin/env python
"""
@file    configTemplateToWiki.py
@author  Michael Behrisch
@version $Id$

Generate Wiki table from configuration template.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sys
from xml.sax import parse, handler

class ConfigReader(handler.ContentHandler):

    def __init__(self):
        self._level = 0

    def startElement(self, name, attrs):
        if self._level == 1:
            # subtopic
            print """===%s===
{| cellspacing="0" border="1" width="90%%" align="center"
|-
! style="background:#ddffdd;" valign="top" | Option
! style="background:#ddffdd;" valign="top" | Default Value
! style="background:#ddffdd;" valign="top" | Description""" % name.replace("_", " ").title()
        if self._level == 2:
            # entry
            print '|-\n| valign="top" |',;
            a = ""
            for s in attrs.get('synonymes', '').split():
                if len(s) == 1:
                    a = s
            if a != "":
                print '{{Option|-%s {{DT_%s}}}}<br/>' % (a, attrs['type']),
            print '{{Option|--%s {{DT_%s}}}}' % (name, attrs['type'])
            print '| valign="top" |', attrs['value'];
            print '| valign="top" |', attrs['help'];
        self._level += 1

    def endElement(self, name):
        self._level -= 1
        if self._level == 1:
            # subtopic end
            print "|-\n|}\n"

if __name__ == "__main__":
    parse(sys.argv[1], ConfigReader())
