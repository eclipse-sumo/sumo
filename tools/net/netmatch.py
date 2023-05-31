#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    netmatch.py
# @author  Marek Heinrich
# @author  Michael Behrisch
# @date    2013-10-22


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import sumolib  # noqa
from sumolib.geomhelper import distance  # noqa


argParser = sumolib.options.ArgumentParser()
argParser.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                       default=False, category="processing", help="tell me what you are doing")
argParser.add_argument("-1", "--net1", dest="net1", required=True, category="input", type=argParser.net_file,
                       help="first SUMO network to use (mandatory)", metavar="FILE")
argParser.add_argument("-2", "--net2", dest="net2", required=True, category="input", type=argParser.net_file,
                       help="second SUMO network to use (mandatory)", metavar="FILE")
# argParser.add_argument("-t", "--trips", dest="trips",
#                    category="input", type=argParser.route_file, help="Trips to remap (mandatory)", metavar="FILE")
# argParser.add_argument("-a", "--nodes1", dest="nodes1",
#                     category="input", type=argParser.file, help="The first matching nodes", metavar="NODELIST")
# argParser.add_argument("-b", "--nodes2", dest="nodes2",
#                     category="input", type=argParser.file, help="The second matching nodes", metavar="NODELIST")
# argParser.add_argument("-d", "--dump", dest="dump",
#                     category="output", type=argParser.file, help="dump file to use", metavar="FILE")
argParser.add_argument("-d", "--delta", default=1,
                       type=float, category="processing", help="maximum distance between end points")
argParser.add_argument("-o", "--output", dest="output", required=True,
                       category="output", type=argParser.file, help="(base) name for the output", metavar="FILE")
argParser.add_argument("--edges1", category="output", type=argParser.file,
                       help="matched edges in net 1", metavar="FILE")
argParser.add_argument("--edges2", category="output", type=argParser.file,
                       help="matched edges in net 2", metavar="FILE")
options = argParser.parse_args()


# read networks
if options.verbose:
    print("Reading net#1...")
net1 = sumolib.net.readNet(options.net1)
net1.move(-net1.getLocationOffset()[0], -net1.getLocationOffset()[1])

if options.verbose:
    print("Reading net#2...")
net2 = sumolib.net.readNet(options.net2)
net2.move(-net2.getLocationOffset()[0], -net2.getLocationOffset()[1])


f = open(options.output, "w")

matchedEdges1 = set()
matchedEdges2 = set()

for e1 in net1.getEdges():
    from1 = e1.getFromNode().getCoord()
    to1 = e1.getToNode().getCoord()
    for e2, d in net2.getNeighboringEdges(from1[0], from1[1], options.delta):
        if distance(from1, e2.getFromNode().getCoord()) < options.delta \
                and distance(to1, e2.getToNode().getCoord()) < options.delta \
                and e2 not in matchedEdges2:
            print(e1.getID(), e2.getID(), file=f)
            matchedEdges1.add(e1)
            matchedEdges2.add(e2)
            break

f.close()

print("matched", len(matchedEdges1), "out of",
      len(net1.getEdges()), "in", options.net1)
print("matched", len(matchedEdges2), "out of",
      len(net2.getEdges()), "in", options.net2)

if options.edges1:
    print("\n".join(["edge:%s" % e.getID()
                     for e in matchedEdges1]), file=open(options.edges1, "w"))
if options.edges2:
    print("\n".join(["edge:%s" % e.getID()
                     for e in matchedEdges2]), file=open(options.edges2, "w"))

# adaptor = sumolib.net.netshiftadaptor.NetShiftAdaptor(
#        net1, net2, options.nodes1.split(","), options.nodes2.split(","))
# adaptor.reproject(options.verbose)
