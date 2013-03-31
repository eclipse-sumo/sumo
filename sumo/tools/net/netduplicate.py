#!/usr/bin/env python
"""
@file    netduplicate.py
@author  Michael Behrisch
@date    2013-04-01
@version $Id$

Reads a sumo network and duplication descriptors (prefix:x-offset:y-offset)
and creates a disconnected network of duplicates.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import sys, os, subprocess, optparse
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', 'lib'))
from testUtil import checkBinary

def parseArgs():
    USAGE = "Usage: " + sys.argv[0] + " <net> <prefix:x:y> <prefix:x:y>+"
    optParser = optparse.OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
            default=False, help="Give more output")
    optParser.add_option("--path", dest="path",
            default=os.environ.get("SUMO_BINDIR", ""), help="Path to binaries")
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
    netconvert = checkBinary("netconvert", options.path)
    nodes = []
    edges = []
    conns = []
    for d in options.desc:
        createPlain(netconvert, options.net, d[0], d[1], d[2])
        out = open("%s_.nod.xml" % d[0], 'w')
        for line in open("%s.nod.xml" % d[0]):
            line = line.replace('id="', 'id="%s_' % d[0])
            out.write(line);
        out.close()
        nodes.append(out.name)
        out = open("%s_.edg.xml" % d[0], 'w')
        for line in open("%s.edg.xml" % d[0]):
            line = line.replace('id="', 'id="%s_' % d[0])
            line = line.replace('from="', 'from="%s_' % d[0])
            line = line.replace('to="', 'to="%s_' % d[0])
            out.write(line);
        out.close()
        edges.append(out.name)
        out = open("%s_.con.xml" % d[0], 'w')
        for line in open("%s.con.xml" % d[0]):
            line = line.replace('from="', 'from="%s_' % d[0])
            line = line.replace('to="', 'to="%s_' % d[0])
            out.write(line);
        out.close()
        conns.append(out.name)
    subprocess.call([netconvert, 
        "--node-files", ",".join(nodes),
        "--edge-files", ",".join(edges),
        "--connection-files", ",".join(conns)])

if __name__ == "__main__":
    main()
