#!/usr/bin/env python
"""
@file    typemap.py
@author  Michael Behrisch
@date    2015-07-06
@version $Id$

This script rebuilds "../../src/netimport/typemap.h", the file 
representing the default typemaps.
It does this by parsing the data from the sumo data dir.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2015-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import sys
from os.path import dirname, exists, getmtime, join


def writeTypeMap(typemapFile, typemap):
    with open(typemapFile, 'w') as f:
        for format, mapFile in sorted(typemap.iteritems()):
            print("const std::string %sTypemap =" % format, file=f)
            for line in open(mapFile):
                print('"%s"' %
                      line.replace('"', r'\"').replace('\n', r'\n'), file=f)
            print(";", file=f)


def main():
    typemapDir = join(dirname(__file__), '..', '..', 'data', 'typemap')
    if len(sys.argv) == 1:
        typemapFile = join(
            dirname(__file__), '..', '..', 'src', 'netimport', 'typemap.h')
        formats = ("opendrive", "osm")
        suffix = "Netconvert.typ.xml"
    else:
        typemapFile = join(
            dirname(__file__), '..', '..', 'src', 'polyconvert', 'pc_typemap.h')
        formats = ("navteq", "osm", "visum")
        suffix = "Polyconvert.typ.xml"
    # determine output file
    typemap = {}
    maxTime = 0
    for format in formats:
        typemap[format] = join(typemapDir, format + suffix)
        if exists(typemap[format]):
            maxTime = max(maxTime, getmtime(typemap[format]))
    if not exists(typemapFile) or maxTime > getmtime(typemapFile):
        writeTypeMap(typemapFile, typemap)


if __name__ == "__main__":
    main()
