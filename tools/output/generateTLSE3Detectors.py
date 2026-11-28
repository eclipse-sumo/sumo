#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateTLSE3Detectors.py
# @author  Daniel Krajzewicz
# @author  Karol Stosiek
# @author  Michael Behrisch
# @date    2007-10-25

from __future__ import absolute_import
from __future__ import print_function

import logging
import optparse
import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


def getOptions():
    option_parser = optparse.OptionParser()
    option_parser.add_option("-n", "--net-file",
                             dest="net_file",
                             help="Network file to work with. Mandatory.",
                             type="string")
    option_parser.add_option("-j", "--junction-ids",
                             dest="junctionIDs",
                             help="List of junctions that shall receive detectors (comma separated)",
                             type="string")
    option_parser.add_option("-l", "--detector-length",
                             dest="requested_detector_length",
                             help="Length of the detector in meters "
                             "(-1 for maximal length).",
                             type="int",
                             default=250)
    option_parser.add_option("-d", "--distance-to-TLS",
                             dest="requested_distance_to_tls",
                             help="Distance of the detector to the traffic "
                             "light in meters. Defaults to 0.1m.",
                             type="float",
                             default=.1)
    option_parser.add_option("-f", "--frequency",
                             dest="frequency",
                             help="Detector's frequency. Defaults to 60.",
                             type="int",
                             default=60)
    option_parser.add_option("-o", "--output",
                             dest="output",
                             help="The name of the file to write the detector "
                             "definitions into. Defaults to e3.add.xml.",
                             type="string",
                             default="e3.add.xml")
    option_parser.add_option("--prefix",
                             dest="prefix",
                             help="Prefix for generated detectors",
                             type="string",
                             default="e3_")
    option_parser.add_option("-r", "--results-file",
                             dest="results",
                             help="The name of the file the detectors write "
                             "their output into. Defaults to e3output.xml.",
                             type="string",
                             default="e3output.xml")
    option_parser.add_option("--min-pos",
                             dest="minPos",
                             help="minimum position of entry detectors light in meters. Defaults to 0.1m.",
                             type="float",
                             default=.1)

    option_parser.add_option(
        "--interior", action="store_true",
        default=False, help="Extend measurement area to the junction interior")
    option_parser.add_option(
        "--joined", action="store_true",
        default=False, help="Create one e3Detector per junction")
    option_parser.add_option(
        "--follow-turnaround", dest="followTurnaround", action="store_true",
        default=False, help="Extend entry detectors past turn-around connections")
    option_parser.set_usage("generateTLSE3Detectors.py -n example.net.xml "
                            "-l 250 -d .1 -f 60")

    (options, args) = option_parser.parse_args()
    if not options.net_file:
        print("Missing arguments")
        option_parser.print_help()
        exit()
    return options


def isVehicleLane(lane):
    return bool(lane.getPermissions() - set(["pedestrian"]))


def getVehicleLanes(edge):
    return [lane for lane in edge.getLanes() if isVehicleLane(lane)]


def getUpstreamVehicleEdges(net, edge, distance, stopOnTLS, stopOnTurnaround):
    ret = []
    seen = set()
    toProc = [[edge, 0, []]]
    while toProc:
        ie = toProc.pop()
        if ie[0] in seen:
            continue
        seen.add(ie[0])
        if ie[1] + ie[0].getLength() >= distance:
            ret.append([ie[0], ie[0].getLength() + ie[1] - distance, ie[2], False])
            continue
        vehIncoming = [
            ci for ci in ie[0]._incoming
            if getVehicleLanes(ci) and any(
                isVehicleLane(c.getFromLane()) and isVehicleLane(c.getToLane())
                for c in ie[0]._incoming[ci])
        ]
        if not vehIncoming:
            ret.append([ie[0], ie[0].getLength() + ie[1], ie[2], True])
            continue
        mn = []
        stop = False
        for ci in vehIncoming:
            if ci not in seen:
                prev = list(ie[2])
                vehConns = [c for c in ie[0]._incoming[ci]
                            if isVehicleLane(c.getFromLane()) and isVehicleLane(c.getToLane())]
                if stopOnTLS and ci._tls and ci != edge and not stop:
                    ret.append([ie[0], ie[1], prev, True])
                    stop = True
                elif (stopOnTurnaround
                      and vehConns[0].getDirection() == sumolib.net.connection.Connection.LINKDIR_TURN
                      and not stop):
                    ret.append([ie[0], ie[1], prev, True])
                    stop = True
                else:
                    prev.append(ie[0])
                    mn.append([ci, ie[0].getLength() + ie[1], prev])
        if not stop:
            toProc.extend(mn)
    return ret


def collectEntryExit(options, edge, seenExits=None):
    stopOnTLS = True
    stopOnTurnaround = not options.followTurnaround
    input_edges = getUpstreamVehicleEdges(
        network, edge, options.requested_detector_length, stopOnTLS, stopOnTurnaround)
    input_edges.sort(key=lambda vals: vals[0].getID())
    entries = []
    exits = []
    for firstEdge, position, intermediate, aborted in input_edges:
        if aborted:
            position = .1
        for lane in getVehicleLanes(firstEdge):
            lanePos = min(lane.getLength(), max(position, min(options.minPos, lane.getLength())))
            entries.append((lane.getID(), "%.2f" % lanePos))

    if options.interior:
        # exit just after leaving the intersection
        for e2 in sorted(edge.getOutgoing(), key=lambda e: e.getID()):
            if not any(isVehicleLane(c.getFromLane()) and isVehicleLane(c.getToLane())
                       for c in edge.getOutgoing()[e2]):
                continue
            for lane in getVehicleLanes(e2):
                if seenExits is not None:
                    if lane.getID() in seenExits:
                        continue
                    seenExits.add(lane.getID())
                exits.append((lane.getID(), "0"))
    else:
        # exit just before entering the intersection
        for lane in getVehicleLanes(edge):
            if lane.getLength() < options.requested_distance_to_tls:
                exitPos = "0"
            elif options.requested_distance_to_tls == .1:
                exitPos = "-.1"
            else:
                exitPos = "%.2f" % -options.requested_distance_to_tls
            exits.append((lane.getID(), exitPos))
    return entries, exits


def writeItems(detector_xml, entries, exits):
    for laneID, posStr in entries:
        detector_entry_xml = detector_xml.addChild("detEntry")
        detector_entry_xml.setAttribute("lane", laneID)
        detector_entry_xml.setAttribute("pos", posStr)
    for laneID, posStr in exits:
        detector_exit_xml = detector_xml.addChild("detExit")
        detector_exit_xml.setAttribute("lane", laneID)
        detector_exit_xml.setAttribute("pos", posStr)


if __name__ == "__main__":
    # pylint: disable-msg=C0103
    options = getOptions()

    logging.basicConfig(level="INFO")

    logging.info("Reading net...")
    network = sumolib.net.readNet(options.net_file)

    logging.info("Generating detectors...")
    detectors_xml = sumolib.xml.create_document("additional")
    generated_detectors = 0

    tlsList, getEdges = network._tlss, sumolib.net.TLS.getEdges
    if options.junctionIDs:
        tlsList = [network.getNode(n) for n in options.junctionIDs.split(',')]
        getEdges = sumolib.net.node.Node.getIncoming

    for tls in tlsList:
        vehEdges = [e for e in sorted(getEdges(tls), key=sumolib.net.edge.Edge.getID)
                    if getVehicleLanes(e)]
        if not vehEdges:
            continue
        if options.joined:
            seenExits = set() if options.interior else None
            perEdgeItems = [collectEntryExit(options, edge, seenExits) for edge in vehEdges]
            if any(entries for entries, _ in perEdgeItems) and any(exits for _, exits in perEdgeItems):
                detector_xml = detectors_xml.addChild("e3Detector")
                detector_xml.setAttribute("id", options.prefix + str(tls.getID()))
                detector_xml.setAttribute("freq", str(options.frequency))
                detector_xml.setAttribute("file", options.results)
                for entries, exits in perEdgeItems:
                    writeItems(detector_xml, entries, exits)
                generated_detectors += 1

        else:
            for edge in vehEdges:
                entries, exits = collectEntryExit(options, edge)
                if entries and exits:
                    detector_xml = detectors_xml.addChild("e3Detector")
                    detector_xml.setAttribute(
                        "id", options.prefix + str(tls.getID()) + "_" + str(edge.getID()))
                    detector_xml.setAttribute("freq", str(options.frequency))
                    detector_xml.setAttribute("file", options.results)
                    if options.interior:
                        detector_xml.setAttribute("openEntry", "true")
                    writeItems(detector_xml, entries, exits)
                    generated_detectors += 1

    detector_file = open(options.output, 'w')
    detector_file.write(detectors_xml.toXML())
    detector_file.close()

    logging.info("%d e3 detectors generated!" % (generated_detectors))
