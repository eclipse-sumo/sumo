#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    network_statistics.py
# @author  Ronald Nippold
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Matthias Schwamborn
# @date    2009-10-06

"""
Usage: python network_statistics.py network-file-to-be-analyzed.net.xml

Prints some information about a given network
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net  # noqa


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net-file> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--net-file", dest="netFile", help="The .net.xml file to read")
    argParser.add_argument("-w", "--write-html", action="store_true", default=True,
                           help="Write statistics to HTML output file")
    argParser.add_argument("-p", "--show-png", action="store_true", default=True,
                           help="Render PNG barplot with edge statistics")
    options = argParser.parse_args()
    if not options.netFile:
        print("Missing arguments")
        argParser.print_help()
        exit()
    return options


def renderHTML(values, filePrefix):
    fd = open(filePrefix + ".html", "w")
    fd.write("<html><body>")
    fd.write("<h1>" + values["netname"] + "</h1></br>")
    # network
    fd.write("<h2>Network</h2></br>")
    # edges
    fd.write("<h2>Edges</h2></br>")
    fd.write("Edge number: " + str(values["edgeNumber"]) + "</br>")
    fd.write("Edgelength sum: " + str(values["edgeLengthSum"]) + "</br>")
    fd.write("Lanelength sum: " + str(values["laneLengthSum"]) + "</br>")
    # nodes
    fd.write("<h2>Nodes</h2></br>")
    fd.write("Node number: " + str(values["nodeNumber"]) + "</br>")
    fd.write("</body></html>")
    fd.close()


def renderPNG(values):
    from pylab import bar, show
    bar([0], [values["edgeNumber"]], 1, color='r')
    show()


def render(net, filePrefix):
    values = {}
    values["netname"] = "hallo"
    values["edgesPerLaneNumber"] = {}
    values["edgeLengthSum"] = 0
    values["laneLengthSum"] = 0
    values["edgeNumber"] = len(net._edges)
    values["nodeNumber"] = len(net._nodes)
    for e in net._edges:
        values["edgeLengthSum"] = values["edgeLengthSum"] + e._length
        values["laneLengthSum"] = values["laneLengthSum"] + \
            (e._length * float(len(e._lanes)))
        if len(e._lanes) not in values["edgesPerLaneNumber"]:
            values["edgesPerLaneNumber"][len(e._lanes)] = 0
        values["edgesPerLaneNumber"][
            len(e._lanes)] = values["edgesPerLaneNumber"][len(e._lanes)] + 1

    if options.write_html:
        renderHTML(values, filePrefix)
    if options.show_png:
        renderPNG(values)


if __name__ == "__main__":
    options = parse_args()
    net = sumolib.net.readNet(options.netFile)

    # ----------
    narea = (net._ranges[0][1] - net._ranges[0][0]) * \
        (net._ranges[1][1] - net._ranges[1][0])
    if narea < 1000 * 1000:
        print("area of network:\t" + str(round(narea, 2)) + " m²" + "\n")
    else:
        print("area of network:\t" + str(round(narea / 1000000, 2)) + " km²" + "\n")
    print("total number of edges:\t" + str(len(net._edges)) + "\n")
    # ----------
    lengthOfEdgeSum = .0
    lane_1 = 0
    lane_2 = 0
    lane_3 = 0
    lane_4 = 0
    for n in net._edges:
        lengthOfEdgeSum = lengthOfEdgeSum + n._length
        if len(n._lanes) == 1:
            lane_1 = lane_1 + 1
        elif len(n._lanes) == 2:
            lane_2 = lane_2 + 1
        elif len(n._lanes) == 3:
            lane_3 = lane_3 + 1
        else:
            lane_4 = lane_4 + 1
    if lengthOfEdgeSum < 10000:
        print("total length of all edges:\t" +
              str(round(lengthOfEdgeSum, 2)) + " m" + "\n")
    else:
        print("total length of all edges:\t" +
              str(round(lengthOfEdgeSum / 1000, 2)) + " km" + "\n")
    print("number of edges with 1 lane:\t" + str(lane_1) + "\n")
    print("number of edges with 2 lanes:\t" + str(lane_2) + "\n")
    print("number of edges with 3 lanes:\t" + str(lane_3) + "\n")
    print("number of edges with 4 or more lanes:\t" + str(lane_4) + "\n")
    print("total number of nodes:\t" + str(len(net._nodes)) + "\n")

    if options.write_html or options.show_png:
        filePrefix = options.netFile[:options.netFile.rfind(".net.xml")]
        if options.netFile.rfind(".net.xml") == -1:
            filePrefix = options.netFile
        render(net, filePrefix)
