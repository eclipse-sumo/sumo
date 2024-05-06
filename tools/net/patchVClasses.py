#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    patchVClasses.py
# @author  Jakob Erdmann
# @date    2024-04-30

"""
Removes vclasses from the network so it can be loaded with older versions of SUMO
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import subprocess
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.xml import _open  # noqa


def parse_args():
    optParser = ArgumentParser()
    optParser.add_option("net", category="input", type=optParser.net_file,
                         help="the network to patch")
    optParser.add_option("-o", "--output-file", dest="outfile", category="output", type=optParser.net_file,
                         help="the output network file")
    optParser.add_option("-d", "--delete-classes", dest="deleteClasses",
                         help=("list of classes to remove from both allow and disallow (for downward compatibility), " +
                               "default: subway,cable_car,aircraft,wheelchair,scooter,drone,container"))
    optParser.add_option("-D", "--disallow", dest="disallow",
                         help="disallow the list of classes (remove from 'allow' or add to 'disallow'")
    options = optParser.parse_args()

    if options.deleteClasses and options.disallow:
        raise ValueError("Only one of the options --delete-classes and --disallow may be set")
    elif options.deleteClasses is None and options.disallow is None:
        options.deleteClasses = "subway,cable_car,aircraft,wheelchair,scooter,drone,container"

    if not options.outfile:
        options.outfile = "patched." + options.net

    return options


def main(options):
    net = sumolib.net.readNet(options.net)
    if options.disallow:
        disallowVClasses(options, net)
    else:
        removeVClasses(options, net)


def removeVClasses(options, net):
    classes = options.deleteClasses.split(',')
    with open(options.outfile, 'w') as outf:
        for line in _open(options.net):
            for c in classes:
                line = line.replace(c + " ", "")
                line = line.replace(" " + c, "")
                line = line.replace('allow="%s"' % c, 'disallow="all"')
                line = line.replace('disallow="%s"' % c, "")
            outf.write(line)


def disallowVClasses(options, net):
    classes = set(options.disallow.split(','))
    prefix = options.net
    if prefix.endswith(".net.xml.gz"):
        prefix = prefix[:-11]
    elif prefix.endswith(".net.xml"):
        prefix = prefix[:-8]
    edgePatch = prefix + ".patch.edg.xml"

    with open(edgePatch, 'w') as outfe:
        sumolib.writeXMLHeader(outfe, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        for e in net.getEdges():
            writeLanes = []
            for lane in e.getLanes():
                allowed = set(lane.getPermissions()).difference(classes)
                if allowed != lane.getPermissions():
                    allowed = allowed.difference(sumolib.net.lane.SUMO_VEHICLE_CLASSES_DEPRECATED)
                    writeLanes.append((lane.getIndex(), " ".join(allowed)))
            if writeLanes:
                outfe.write('    <edge id="%s">\n' % e.getID())
                for index, allow in writeLanes:
                    outfe.write('         <lane index="%s" allow="%s"/>\n' % (index, allow))
                outfe.write('    </edge>\n')
        outfe.write("</edges>\n")

    with open(options.outfile, 'w') as outf:
        for line in _open(options.net):
            for c in classes:
                line = line.replace(c + " ", "")
                line = line.replace(" " + c, "")
            outf.write(line)

    NETCONVERT = sumolib.checkBinary('netconvert')
    subprocess.call([NETCONVERT,
                     '-s', options.net,
                     '-e', edgePatch,
                     '-o', options.outfile])


if __name__ == "__main__":
    main(parse_args())
