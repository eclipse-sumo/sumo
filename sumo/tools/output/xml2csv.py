#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    xml2csv.py
@author  Jakob Erdmann
@date    2013-12-08
@version $Id: xml2csv.py 14677 2013-09-11 08:30:08Z namdre $

Convert hierarchical xml files to csv. This only makes sense if the hirarchy has low depth.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,sys
from collections import defaultdict
from optparse import OptionParser
import xml.sax

class LevelHandler(xml.sax.handler.ContentHandler):
    """A handler which knows the current nesting level"""
    def __init__(self):
        self.lvl = -1

    def startElement(self, name, attrs):
        self.lvl += 1

    def endElement(self, name):
        self.lvl -= 1

class AttrFinder(LevelHandler):
    def __init__(self):
        LevelHandler.__init__(self)
        self.lvlmax = self.lvl
        self.attrs = []
        self.knownAttrs = set()
        self.tagAttrs = defaultdict(set) # tag -> set of attrs
        self.renamedAttrs = {} # (name, attr) -> renamedAttr

    def startElement(self, name, attrs):
        LevelHandler.startElement(self, name, attrs)
        self.lvlmax = max(self.lvl, self.lvlmax)
        if self.lvl > 0:
            for a in attrs.keys():
                if not a in self.tagAttrs[name]:
                    self.tagAttrs[name].add(a)
                    if a in self.knownAttrs:
                        if not (name, a) in self.renamedAttrs:
                            anew = "%s_%s" % (name, a)
                            print("warning: renaming attribute %s of tag %s to %s" % (
                                a, name, anew))
                            self.renamedAttrs[(name, a)] = anew
                            self.knownAttrs.add(anew)
                            self.attrs.append(anew)
                    else:
                        self.knownAttrs.add(a)
                        self.attrs.append(a)


class CSVWriter(LevelHandler):
    def __init__(self, attrs, renamedAttrs, output_level, outfile, options,
            quote):
        LevelHandler.__init__(self)
        self.attrs = attrs
        self.renamedAttrs = renamedAttrs
        self.output_level = output_level
        self.outfile = outfile
        self.options = options
        self.quote = quote
        self.currentValues = defaultdict(lambda : "")

    def startElement(self, name, attrs):
        LevelHandler.startElement(self, name, attrs)
        if self.lvl > 0:
            for a, v in attrs.items():
                a = self.renamedAttrs.get((name, a), a)
                self.currentValues[a] = v
            if self.lvl == self.output_level:
                self.outfile.write(self.options.separator.join(
                    [self.quote(self.currentValues[a]) for a in self.attrs]) + "\n")


def get_options():
    USAGE = "Usage: " + sys.argv[0] + " <input_file>"
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
            default=False, help="Give more output")
    optParser.add_option("-s", "--separator", default="\t",
             help="separating character for fields")
    optParser.add_option("-q", "--quotechar", default='',
             help="quoting character for fields")
    options, args = optParser.parse_args()
    if len(args) != 1:
        sys.exit(USAGE)
    options.source = args[0]
    return options 

def main():
    options = get_options()
    def quote(s):
        return "%s%s%s" % (options.quotechar, s, options.quotechar)
    # get attributes
    attrFinder = AttrFinder()
    xml.sax.parse(options.source, attrFinder)
    # write csv
    outfilename = os.path.splitext(options.source)[0] + ".csv"
    with open(outfilename, 'w') as f:
        # write header
        f.write(options.separator.join(map(quote,attrFinder.attrs)) + "\n")
        # write records
        handler = CSVWriter(attrFinder.attrs, attrFinder.renamedAttrs,
                attrFinder.lvlmax, f, options, quote)
        xml.sax.parse(options.source, handler)


if __name__ == "__main__":
    main()
