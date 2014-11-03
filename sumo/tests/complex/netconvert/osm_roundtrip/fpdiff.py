#!/usr/bin/env python
"""
@file    fpdiff.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2011-06-01
@version $Id$

included as is from texttest
copyright texttest.org (LGPL according to google cache)

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys, difflib, StringIO, optparse

def _getNumberAt(l, pos):
    start = pos
    while start > 0 and l[start-1] in "1234567890.eE-":
        start -= 1
    end = pos
    while end < len(l) and l[end] in "1234567890.eE-":
        end += 1
    return l[start:end], l[end:]

def _fpequalAtPos(l1, l2, tolerance, relTolerance, pos):
    number1, l1 = _getNumberAt(l1, pos)
    number2, l2 = _getNumberAt(l2, pos)
    try:
        equal = False
        deviation = abs(float(number1) - float(number2))
        if tolerance != None and deviation <= tolerance:
            equal = True
        elif relTolerance != None:
            referenceValue = abs(float(number1))
            if referenceValue == 0:
                equal = (deviation == 0)
            elif deviation / referenceValue <= relTolerance:
                equal = True
    except ValueError:
        pass
    return equal, l1, l2

def _fpequal(l1, l2, tolerance, relTolerance):
    pos = 0
    while pos < min(len(l1), len(l2)):
        if l1[pos] != l2[pos]:
            equal, l1, l2 = _fpequalAtPos(l1, l2, tolerance, relTolerance, pos)
            if not equal:
                return False
            pos = 0
        else:
            pos += 1
    if len(l1) == len(l2):
        return True
    else:
        return _fpequalAtPos(l1, l2, tolerance, relTolerance, pos)[0]

def fpfilter(fromlines, tolines, outlines, tolerance, relTolerance=None):
    s = difflib.SequenceMatcher(None, fromlines, tolines)
    for tag, i1, i2, j1, j2 in s.get_opcodes():
        if tag == "replace" and i2 - i1 == j2 - j1:
            for fromline, toline in zip(fromlines[i1:i2], tolines[j1:j2]):
                if _fpequal(fromline, toline, tolerance, relTolerance):
                    outlines.write(fromline)
                else:
                    outlines.write(toline)
        else:
            outlines.writelines(tolines[j1:j2])

def main():
    parser = optparse.OptionParser("usage: %prog [options] fromfile tofile")
    parser.add_option("-t", "--tolerance", type="float",
                      help='Set absolute floating point tolerance')
    parser.add_option("-r", "--relative", type="float",
                      help='Set relative floating point tolerance')
    parser.add_option("-o", "--output",
                      help='Write filtered tofile to use external diff')
    (options, args) = parser.parse_args()
    if len(args) == 0: # pragma: no cover - not production code
        parser.print_help()
        sys.exit(1)
    if len(args) != 2: # pragma: no cover - not production code
        parser.error("need to specify both a fromfile and tofile")
    fromfile, tofile = args
    fromlines = open(fromfile, 'U').readlines()
    tolines = open(tofile, 'U').readlines()
    if options.output:
        out = open(options.output, 'w')
        fpfilter(fromlines, tolines, out, options.tolerance, options.relative)
        out.close()
    else: # pragma: no cover - not production code
        out = StringIO.StringIO()
        fpfilter(fromlines, tolines, out, options.tolerance, options.relative)
        out.seek(0)
        tolines = out.readlines()
        sys.stdout.writelines(difflib.unified_diff(fromlines, tolines, fromfile, tofile))

if __name__ == '__main__':
    main()
