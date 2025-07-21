#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    netduplicate.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2013-04-01

"""
Reads a sumo network and duplication descriptors (prefix:x-offset:y-offset)
and creates a disconnected network of duplicates.
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import subprocess
import tempfile
import shutil
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.options import ArgumentParser


def parseArgs():
    USAGE = "Usage: " + sys.argv[0] + " <net> <prefix:x:y> <prefix:x:y>+"
    optParser = ArgumentParser(usage=USAGE)
    sumolib.pullOptions("netconvert", optParser)
    optParser.add_option(
        "net", type=ArgumentParser.net_file, help="network to duplicate")
    optParser.add_option(
        "desc", nargs="+", help="List of descriptions Prefix:OffsetX:OffsetY+")
    optParser.add_option(
        "--drop-types", action="store_true", default=False, help="Remove edge types")
    options = optParser.parse_args()
    options.desc = [d.split(":") for d in options.desc]
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
    for prefix, xoff, yoff in options.desc:
        createPlain(
            netconvert, options.net, os.path.join(tmpDir, prefix), xoff, yoff)
        out = open(os.path.join(tmpDir, "%s_.nod.xml" % prefix), 'w')
        with open(os.path.join(tmpDir, "%s.nod.xml" % prefix)) as nfile:
            for line in nfile:
                if 'location' in line:
                    continue
                line = line.replace('id="', 'id="%s_' % prefix)
                line = line.replace('tl="', 'tl="%s_' % prefix)
                out.write(line)
        out.close()
        nodes.append(out.name)
        out = open(os.path.join(tmpDir, "%s_.edg.xml" % prefix), 'w')
        with open(os.path.join(tmpDir, "%s.edg.xml" % prefix)) as efile:
            for line in efile:
                line = line.replace('id="', 'id="%s_' % prefix)
                line = line.replace('from="', 'from="%s_' % prefix)
                line = line.replace('to="', 'to="%s_' % prefix)
                if options.drop_types:
                    typeStart = line.find('type="')
                    if typeStart >= 0:
                        typeEnd = line.find('"', typeStart + 6)
                        line = line[0:typeStart] + line[typeEnd + 1:]
                out.write(line)
        out.close()
        edges.append(out.name)
        out = open(os.path.join(tmpDir, "%s_.con.xml" % prefix), 'w')
        with open(os.path.join(tmpDir, "%s.con.xml" % prefix)) as cfile:
            for line in cfile:
                line = line.replace('from="', 'from="%s_' % prefix)
                line = line.replace('to="', 'to="%s_' % prefix)
                out.write(line)
        out.close()
        conns.append(out.name)
        out = open(os.path.join(tmpDir, "%s_.tll.xml" % prefix), 'w')
        with open(os.path.join(tmpDir, "%s.tll.xml" % prefix)) as tfile:
            for line in tfile:
                line = line.replace('id="', 'id="%s_' % prefix)
                line = line.replace('from="', 'from="%s_' % prefix)
                line = line.replace('to="', 'to="%s_' % prefix)
                line = line.replace('tl="', 'tl="%s_' % prefix)
                out.write(line)
        out.close()
        tlls.append(out.name)
    options.node_files = ",".join(nodes)
    options.edge_files = ",".join(edges)
    options.connection_files = ",".join(conns)
    options.tllogic_files = ",".join(tlls)
    if sumolib.call(netconvert, options) != 0:
        print("Something went wrong, check '%s'!" % tmpDir, file=sys.stderr)
    else:
        shutil.rmtree(tmpDir)


if __name__ == "__main__":
    main()
