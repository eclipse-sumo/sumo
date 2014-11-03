#!/usr/bin/env python
"""
@file    prepareVISUM.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-09-01
@version $Id$


Parses a VISUM-file and writes a modified
 version in "<FILENAME>_mod.net".
Removes all roads which type is not within
 types2import, patches lane number information
 for all other using the values stored in types2import.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys, optparse

optParser = optparse.OptionParser(usage="usage: %prog [options] <visum.net>")
optParser.add_option("-s", "--sumo-netconvert", dest="netconvert",
                     default="netconvert",
                     help="netconvert executable", metavar="FILE")
(options, args) = optParser.parse_args()
if len(args) != 1:
    print >> sys.stderr, "Net argument missing."
    optParser.print_help()
    sys.exit(1)

types2import = { 2:1, 10:2, 11:4, 12:3, 13:2, 14:2, 15:3, 16:3, 17:3, 18:3, 19:1,
                20:2, 21:3, 22:2, 23:2, 24:1, 25:1, 26:1, 27:2, 28:2, 29:2,
                30:3, 31:2, 32:2, 33:2, 34:1, 35:1, 36:1, 37:2, 38:1, 39:3,
                40:3, 41:1, 42:1, 43:2, 44:3, 45:2, 46:1, 47:1, 49:3,
                50:2, 51:3, 52:3, 53:1, 54:3, 55:3, 56:3, 57:3, 58:3, 59:3,
                60:1, 61:1, 62:1, 63:1, 64:1, 65:1, 66:1, 68:2, 69:2,
                70:1, 71:2, 72:3, 73:3, 74:3, 75:1, 76:1, 77:3, 78:3, 79:1,
                80:1, 81:1, 82:1, 83:1, 84:1, 85:3, 86:3, 87:3, 88:3, 89:3,
                90:1, 91:1, 92:1, 93:1, 94:2, 95:1, 96:1, 97:1, 98:2, 99:2}

fdi = open(args[0])
modifiedVisumNet = os.path.basename(args[0])[:-4] + "_mod.net"
sumoNet = modifiedVisumNet + ".xml"
fdo = open(modifiedVisumNet, "w")
process = False
skip = False
for line in fdi:
    skip = False
    if line.find("*")==0 or len(line)<2:
        process = False
    if process:
        vals = line.split(";")
        typ = int(vals[5])
        if typ not in types2import:
            skip = True
        else:
            vals[8] = str(types2import[typ])
            line = ";".join(vals)
    if line.find("$STRECKE:")==0:
        process = True
    if not skip:
        fdo.write(line)
fdi.close()
fdo.close()

os.system("%s --visum-file %s --dismiss-loading-errors -o %s"\
          % (options.netconvert, modifiedVisumNet, sumoNet))
