#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    vissim_parseRoutes.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2009-05-27

"""

Parses routes given in the Vissim file (first parameter) as (in-)flows and
 route decisions.

The read flows are saved as <OUTPUT_PREFIX>.flows.xml
The read routes are saved as <OUTPUT_PREFIX>.rou.xml

(Starting?) edges of the route may be renamed by setting them with "edgemap"
 option (see below).
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import random
import codecs
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def getName(vals, beg):
    name = vals[beg]
    while name.count('"') != 2:
        beg = beg + 1
        name = name + " " + vals[beg]
    return name.replace('"', '')


def parseInFlow(inflow, suggested_name):
    vals = inflow.split()
    i = vals.index("NAME")
    name = getName(vals, i + 1)
    if (name == ""):
        name = suggested_name  # SUMO can't cope with empty IDs, unlike VISSIM
    i = vals.index("STRECKE", i)
    strecke = vals[i + 1]
    i = vals.index("Q", i + 1)
    if vals[i + 1] == "EXAKT":
        q = float(vals[i + 2])
    else:
        q = float(vals[i + 1])
    i = vals.index("ZUSAMMENSETZUNG", i + 1)
    i = vals.index("ZEIT", i)
    i = vals.index("VON", i)
    von = float(vals[i + 1])
    i = vals.index("BIS")
    bis = float(vals[i + 1])
    return (name, strecke, q, von, bis)


def parseRouteDecision(rd):
    vals = rd.split()
    i = vals.index("STRECKE")
    strecke = vals[i + 1]
    # probably, we would normally already here to iterate over time spans...
    i = vals.index("ZEIT", i)
    i = vals.index("VON", i + 1)
    von = float(vals[i + 1])
    i = vals.index("BIS", i + 1)
    bis = float(vals[i + 1])
    if "ROUTE" in vals:
        i = vals.index("ROUTE", i + 1)
    else:
        i = 0
    sumAnteil = 0
    routes = []
    while i > 0:
        r_id = vals[i + 1]
        i = vals.index("STRECKE", i + 1)
        r_ziel = vals[i + 1]
        i = vals.index("ANTEIL", i + 1)
        r_anteil = float(vals[i + 1])
        r_edges = []
        if vals[i + 2] == "UEBER":
            i = i + 3
            while i < len(vals) and vals[i] != "ROUTE":
                if vals[i] in edgemap:
                    r_edges.append(edgemap[vals[i]])
                else:
                    r_edges.append(vals[i])
                i = i + 1
        else:
            try:
                i = vals.index("ROUTE", i)
            except Exception:
                i = -1
        if r_ziel in edgemap:
            r_edges.append(edgemap[r_ziel])
        else:
            r_edges.append(r_ziel)
        routes.append((r_id, r_anteil, r_edges))
        sumAnteil = sumAnteil + r_anteil
        if i >= len(vals):
            i = -1
    return (strecke, sumAnteil, von, bis, routes)


def sorter(idx):
    def t(i, j):
        if i[idx] < j[idx]:
            return -1
        elif i[idx] > j[idx]:
            return 1
        else:
            return 0


# MAIN
if __name__ == '__main__':
    op = sumolib.options.ArgumentParser(description="parse routes from a VISSIM network")
    op.add_option("vissimNet", category="input", type=op.file, help="the VISSIM network file")
    op.add_option("-o", "--output", default="out", category="output", help="output filename prefix")
    op.add_option("-e", "--edgemap", category="input", type=op.edge_list,
                  help="mapping of edge names for renamed edges (orig1:renamed1,orig2:renamed2,...)")
    op.add_option("-s", "--seed", type=int, default=42, category="input", help="random seed")
    options = op.parse_args()

    random.seed(options.seed)
    edgemap = {}
    if options.edgemap:
        for entry in options.edgemap.split(","):
            orig, renamed = entry.split(":")
            edgemap[orig] = renamed
    print("Parsing Vissim input...")
    fd = codecs.open(options.vissimNet, encoding='latin1')
    routeDecisions = []
    haveRouteDecision = False
    currentRouteDecision = ""
    inflows = []
    haveInFlow = False
    currentInFlow = ""
    for line in fd:
        # put all route decision ("ROUTENENTSCHEIDUNG") to a list (routeDecisions)
        if line.find("ROUTENENTSCHEIDUNG") == 0:
            if haveRouteDecision:
                routeDecisions.append(" ".join(currentRouteDecision.split()))
            haveRouteDecision = True
            currentRouteDecision = ""
        elif line[0] != ' ':
            if haveRouteDecision:
                routeDecisions.append(" ".join(currentRouteDecision.split()))
            haveRouteDecision = False
        if haveRouteDecision:
            currentRouteDecision = currentRouteDecision + line
        # put all inflows ("ZUFLUSS") to a list (inflows)
        if line.find("ZUFLUSS") == 0:
            if haveInFlow:
                inflows.append(" ".join(currentInFlow.split()))
            haveInFlow = True
            currentInFlow = ""
        elif line[0] != ' ':
            if haveInFlow:
                inflows.append(" ".join(currentInFlow.split()))
            haveInFlow = False
        if haveInFlow:
            currentInFlow = currentInFlow + line
    fd.close()

    # process inflows
    print("Writing flows...")
    fdo = open(options.output + ".flows.xml", "w")
    sumolib.writeXMLHeader(fdo, "$Id$", "routes", options=options)
    flow_sn = 0
    for inflow in inflows:
        (name, strecke, q, von, bis) = parseInFlow(inflow, str(flow_sn))
        fdo.write('    <flow id="' + name + '" from="' + strecke + '" begin="' +
                  str(von) + '" end="' + str(bis) + '" no="' + str(int(q)) + '"/>\n')
        flow_sn = flow_sn + 1
    fdo.write("</routes>\n")
    fdo.close()

    # process combinations
    #  parse route decision
    print("Building routes...")
    edges2check = {}
    edgesSumFlows = {}
    for rd in routeDecisions:
        (strecke, sumAnteil, von, bis, routes) = parseRouteDecision(rd)
        if strecke not in edges2check:
            edgesSumFlows[strecke] = sumAnteil
            edges2check[strecke] = (von, bis, routes)
    # compute emissions with routes
    emissions = []
    flow_sn = 0
    for inflow in inflows:
        (name, strecke, q, von, bis) = parseInFlow(inflow, str(flow_sn))
        flow_sn = flow_sn + 1
        if strecke in edges2check:
            routes = edges2check[strecke]
            for vi in range(0, int(q)):
                t = von + float(bis - von) / float(q) * float(vi)
                fi = random.random() * edgesSumFlows[strecke]
                edges = []
                ri = 0
                rid = ""
                while len(edges) == 0 and ri < len(routes[2]) and fi >= 0:
                    fi = fi - routes[2][ri][1]
                    if fi < 0:
                        edges = routes[2][ri][2]
                        rid = routes[2][ri][0]
                    ri = ri + 1
                id = str(name.replace(" ", "_")) + "_" + str(rid) + "_" + str(vi)
                if strecke in edgemap:
                    if edgemap[strecke] not in edges:
                        edges.insert(0, edgemap[strecke])
                else:
                    if strecke not in edges:
                        edges.insert(0, strecke)
                emissions.append((int(t), id, edges))
    # sort emissions
    print("Sorting routes...")
    emissions.sort(key=sorter(0))

    # save emissions
    print("Writing routes...")
    fdo = open(options.output + ".rou.xml", "w")
    sumolib.writeXMLHeader(fdo, "$Id$", "routes", options=options)
    for emission in emissions:
        if len(emission[2]) < 2:
            continue
        fdo.write('    <vehicle id="' + emission[1] + '" depart="' + str(
            emission[0]) + '"><route edges="' + " ".join(emission[2]) + '"/></vehicle>\n')
    fdo.write("</routes>\n")
    fdo.close()
