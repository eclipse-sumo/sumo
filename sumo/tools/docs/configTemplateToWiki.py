#!/usr/bin/env python
"""
@file    configTemplateToWiki.py
@author  Michael Behrisch
@version $Id$

Generate Wiki table from configuration template.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, sys
from xml.sax import parse, handler

from mirrorWiki import readParseEditPage

class ConfigReader(handler.ContentHandler):

    def __init__(self, mergeWikiTxt):
        self._level = 0
        self._mergeWiki = mergeWikiTxt
        self._intro = {}
        self._end = len(mergeWikiTxt)
        active = False
        currSect = ""
        for idx, line in enumerate(mergeWikiTxt):
            line = line.strip('\n\r')
            if line == "==Options==":
                active = True
            if active:
                if line[:3] == "===":
                    start = idx
                    currSect = line
                elif line[:2] == "{|":
                    self._intro[currSect] = (start, idx)
                elif line[:4] == "----" or line[:2] == "=S":
                    self._end = idx
                    break
            if currSect == "":
                print line

    def startElement(self, name, attrs):
        if self._level == 1:
            # subtopic
            title = "===%s===" % name.replace("_", " ").title()
            if title in self._intro:
                begin, end = self._intro[title]
                title = ("".join(self._mergeWiki[begin:end]))
            else:
                title += "\n"
            print """%s{| class="wikitable" style="width:90%%"
|-
! style="background:#ddffdd; vertical-align:top; width:350px" | Option
! style="background:#ddffdd; vertical-align:top" | Description""" % title
        if self._level == 2:
            # entry
            print '|-\n| style="vertical-align:top" |',
            a = ""
            for s in attrs.get('synonymes', '').split():
                if len(s) == 1:
                    a = s
            if a != "":
                print '{{Option|-%s {{DT_%s}}}}<br/>' % (a, attrs['type']),
            print '{{Option|--%s {{DT_%s}}}}' % (name, attrs['type'])
            suffix = ""
            if attrs['value']:
                suffix = "; ''default: '''%s'''''" % attrs['value']
            print '| style="vertical-align:top" | %s%s' % (attrs['help'], suffix)
        self._level += 1

    def endElement(self, name):
        self._level -= 1
        if self._level == 1:
            # subtopic end
            print "|-\n|}\n"

    def endDocument(self):
        print ("".join(self._mergeWiki[self._end:])).strip()

if __name__ == "__main__":
    if len(sys.argv) == 2:
        app = sys.argv[1].lower()
        if app == "netgenerate":
            app = "netgen"
        cfg = os.path.join(os.path.dirname(__file__), "..", "..", "tests", app, "meta", "write_template_full", "cfg." + app)
        parse(cfg, ConfigReader(readParseEditPage(sys.argv[1].upper()).splitlines(True)))
    elif len(sys.argv) == 3:
        parse(sys.argv[1], ConfigReader(open(sys.argv[2]).readlines()))
    else:
        print >> sys.stderr, "Usage: %s <template> <wikisrc>\n   or: %s <app>" % (os.path.basename(__file__), os.path.basename(__file__))
