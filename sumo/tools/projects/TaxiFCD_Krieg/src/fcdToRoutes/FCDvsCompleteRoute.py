#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    FCDvsCompleteRoute.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-08
@version $Id$

Compares the FCD-route of an taxi with the generated route which is used in the simulation.
The output data can be visualized with the script My_mpl_dump_onNet based on mpl_dump_onNet from Daniel.
(avg=False)

Secondly the average of added Edges per route will be calculate (avg=True). 

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import util.Path as path

#global vars

edgeList = []
routeDict = {}
taxi = "351_11"  # the Taxi for which the output should be generated
avg = False


def main():
    print("start program")
    readRoutes()
    if avg:
        clacAvg()
    else:
        writeOutput()
    print("end")


def readRoutes():
    """reads the t1CompletePath and the taxiRoutesPath. 
        and sets for each edge in t1CompletePath the color=green
        and if the edge is also in taxiRoutesPath changes the color to red.
    """

    def countEdges(line):
        # get Taxi-Id
        id = line.split('"')[1]
        line = next(inputFile)  # go to next line with edges

        words = line[line.find(">") + 1:line.find("</")].split(" ")
        lastEdge = words[0]
        no = 0
        for edge in words[1:]:
            if edge.find('/') != -1:  # ignore edges with are used only in SUMO
                continue

            if lastEdge != edge:
                no += 1
            lastEdge = edge
        # no=len(line[line.find(">")+1:line.find("</")].split(" ")) #splited in
        # single edges
        routeDict.setdefault(id, []).append(no)

    inputFile = open(path.taxiRoutesComplete, 'r')
    for line in inputFile:
        if line.find("<vehicle id=\"" + taxi + "\"") != -1 and not avg:
            line = next(inputFile)
            #         delete edges tag at start and end
            words = line[line.find(">") + 1:line.find("</")].split(" ")
            for edge in words:
                edgeList.append("id=\"" + edge + "\" no=\"0.9\"")
            print(edgeList)

        # for calc of avg
        if line.find("<vehicle id=") != -1 and avg:
            countEdges(line)
    inputFile.close()

    # read taxiRoutesPath
    inputFile = open(path.taxiRoutes, 'r')
    for line in inputFile:
        if line.find("<vehicle id=\"" + taxi + "\"") != -1 and not avg:
            line = next(inputFile)
            #         delete edges tag at start and end
            words = line[line.find(">") + 1:line.find("</")].split(" ")
            for edge in words:
                if "id=\"" + edge + "\" no=\"0.9\"" in edgeList:
                    edgeList[edgeList.index(
                        "id=\"" + edge + "\" no=\"0.9\"")] = "id=\"" + edge + "\" no=\"0.1\""
                elif "id=\"" + edge + "\" no=\"0.1\"" not in edgeList:
                    # if edge only is in the taxiRoutesPath file (FCD) color
                    # =yellow
                    edgeList.append("id=\"" + edge + "\" no=\"0.5\"")
            print(edgeList)

        # for calc of avg
        if line.find("<vehicle id=") != -1 and avg:
            countEdges(line)
    inputFile.close()


def clacAvg():
    diffList = []
    orgList = []
    compList = []
    ids = set()
    diffList200 = []
    orgList200 = []

    for id, noList in routeDict.iteritems():
        if len(noList) < 2:
            continue
        orgList.append(noList[1])
        compList.append(noList[0])
        diffList.append(noList[0] - noList[1])
        if noList[0] - noList[1] > 0:
            # print id,noList
            ids.add(id)
            diffList200.append(noList[0] - noList[1])
            orgList200.append(noList[1])
        # In Prozent
        # Kanten die nur in Sumo sind mit "/" bei berechnung entfernen
    # Zeige Routen bei denen etwas hinzugfuegt wurde

    print("len Routen gesamt", len(diffList))
    print("avg (Absolut) Kanten hinzugefuegt",
          sum(diffList) / (len(diffList) + 0.0))
    print("Relavg", sum(diffList) / (len(diffList) + 0.0) /
          (sum(orgList) / (len(orgList) + 0.0)) * 100, "%")
    print("avgOrg", sum(orgList) / (len(orgList) + 0.0), " edges")
    print("avgCompleted", sum(compList) / (len(compList) + 0.0), " edges")

    print()
    print(
        "Betrachtung der hinzugefuegten Kanten nur fuer die Taxis bei denen tatsaechlich auch Kanten hinzugefuegt wurden")
    print("ids (Taxis/Routen)", len(ids))
    print("% der Gesamtrouten", 100.0 * len(ids) / len(diffList))
    print("avg 200 (ids)", sum(diffList200) / (len(diffList200) + 0.0))
    print("Relavg 200", sum(diffList200) / (len(diffList200) + 0.0) /
          (sum(orgList200) / (len(orgList200) + 0.0)) * 100, "%")


def writeOutput():
    """Writes an XML-File with the extracted results"""
    outputFile = open(path.fcdVsCompleteRoute + str(taxi) + ".out.xml", 'w')
    outputFile.write("<netstats>\n")
    outputFile.write("\t<interval begin=\"0\" end=\"899\" id=\"dump_900\">\n")
    for k in edgeList:
        outputFile.write("\t\t<edge " + k + " color=\"1.0\"/>\n")
    outputFile.write("\t</interval>\n")
    outputFile.write("</netstats>")


# start the program
main()
