#!/usr/bin/env python
"""
@file    netduplicate.py
@author  Michael Behrisch
@date    2013-04-01
@version $Id$

Reads a sumo network and duplication descriptors (prefix:x-offset:y-offset)
and creates a disconnected network of duplicates.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys, os, subprocess, optparse, tempfile, shutil
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib

def parseArgs():
    USAGE = "Usage: " + sys.argv[0] + " <net> <prefix:x:y> <prefix:x:y>+"
    optParser = optparse.OptionParser(usage=USAGE)
    sumolib.pullOptions("netconvert", optParser)
    optParser.add_option("--drop-types", action="store_true", default=False, help="Remove edge types")
    options, args = optParser.parse_args()
    if len(args) < 3:
        sys.exit(USAGE)
    options.net = args[0]
    options.desc = [d.split(":") for d in args[1:]]
    return options


def createPlain(netconvert, netfile, prefix, xOff, yOff):
    subprocess.call([netconvert, 
        "--sumo-net-file", netfile, 
        "--offset.x", xOff, 
        "--offset.y", yOff, 
        "--plain-output-prefix", prefix])


# run
def main():
    options = parseArgs()
    netconvert = sumolib.checkBinary("netconvert")
    nodes = []
    edges = []
    conns = []
    tlls = []
    tmpDir = tempfile.mkdtemp()
    for d in options.desc:
        createPlain(netconvert, options.net, os.path.join(tmpDir, d[0]), d[1], d[2])
        out = open(os.path.join(tmpDir, "%s_.nod.xml" % d[0]), 'w')
        for line in open(os.path.join(tmpDir, "%s.nod.xml" % d[0])):
            line = line.replace('id="', 'id="%s_' % d[0])
            line = line.replace('tl="', 'tl="%s_' % d[0])
            out.write(line);
        out.close()
        nodes.append(out.name)
        out = open(os.path.join(tmpDir, "%s_.edg.xml" % d[0]), 'w')
        for line in open(os.path.join(tmpDir, "%s.edg.xml" % d[0])):
            line = line.replace('id="', 'id="%s_' % d[0])
            line = line.replace('from="', 'from="%s_' % d[0])
            line = line.replace('to="', 'to="%s_' % d[0])
            if options.drop_types:
                typeStart = line.find('type="')
                if typeStart >= 0:
                    typeEnd = line.find('"', typeStart + 6)
                    line = line[0:typeStart] + line[typeEnd+1:]
            out.write(line);
        out.close()
        edges.append(out.name)
        out = open(os.path.join(tmpDir, "%s_.con.xml" % d[0]), 'w')
        for line in open(os.path.join(tmpDir, "%s.con.xml" % d[0])):
            line = line.replace('from="', 'from="%s_' % d[0])
            line = line.replace('to="', 'to="%s_' % d[0])
            out.write(line);
        out.close()
        conns.append(out.name)
        out = open(os.path.join(tmpDir, "%s_.tll.xml" % d[0]), 'w')
        for line in open(os.path.join(tmpDir, "%s.tll.xml" % d[0])):
            line = line.replace('id="', 'id="%s_' % d[0])
            line = line.replace('from="', 'from="%s_' % d[0])
            line = line.replace('to="', 'to="%s_' % d[0])
            line = line.replace('tl="', 'tl="%s_' % d[0])
            out.write(line);
        out.close()
        tlls.append(out.name)
    options.node_files = ",".join(nodes)
    options.edge_files = ",".join(edges)
    options.connection_files = ",".join(conns)
    options.tllogic_files = ",".join(tlls)
    if sumolib.call(netconvert, options) != 0:
        print >> sys.stderr, "Something went wrong, check '%s'!" % tmpDir
    else:
        shutil.rmtree(tmpDir)

if __name__ == "__main__":
    main()
