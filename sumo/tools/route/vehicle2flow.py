#!/usr/bin/env python
"""
@file    vehicle2flow.py
@author  Michael Behrisch
@date    2012-11-15
@version $Id$

This script replaces all vehicle definitions in a route file by
flow definitions, adding an XML ntity for the repeat interval for
easy later modification.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sys, os, re
from optparse import OptionParser

def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <routefile> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    optParser.add_option("-r", "--repeat", default=1000, type="float", help="repeater interval")
    optParser.add_option("-e", "--end", default=2147483, type="float", help="end of the flow")
    optParser.add_option("-w", "--with-entities", action="store_true", default=False, help="store repeat and end as entities")
    options, args = optParser.parse_args()
    try:
        options.routefile = args[0]
    except:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.routefile + ".rou.xml"
    return options 

def main():
    options = parse_args()
    with open(options.routefile) as f:
        with open(options.outfile, 'w') as outf:
            headerSeen = False
            for line in f:
                if options.with_entities:
                    if "<routes " in line or "<routes>" in line:
                        outf.write("""<!DOCTYPE routes [
        <!ENTITY RepeatInterval "%s">
        <!ENTITY RepeatEnd "%s">
    ]>
    """ % (options.repeat, options.end))
                    line = re.sub(r'<vehicle(.*)depart( ?= ?"[^"]*")', r'<flow\1begin\2 end="&RepeatEnd;" period="&RepeatInterval;"', line)
                else:
                    line = re.sub(r'<vehicle(.*)depart( ?= ?"[^"]*")', r'<flow\1begin\2 end="%s" period="%s"' % (options.end, options.repeat), line)
                line = re.sub(r'</vehicle>', '</flow>', line)
                outf.write(line)

if __name__ == "__main__":
    main()
