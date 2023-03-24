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

# @file    tls_csvSignalGroups.py
# @author  Mirko Barthauer (Technische Universitaet Braunschweig)
# @date    2017-10-17

"""
This script helps with converting a CSV input file with green times per signal group into the SUMO format.
Additionally, it supports creating template CSV input files from a SUMO network file. The input CSV file(s)
contain input blocks divided by titles in brackets. The block [general] sets general information relating to the
signal program like the TLS ID, the program ID, the cycle time [s] and the offset time [s]. Additional 0..n
optional parameters can also be supplied.

[general]
cycle time;<CYCLE TIME [s]>
key;<TLS ID>
subkey;<PROGRAM ID>
offset;<OFFSET [s]>
actuated;<CYCLESEC START>;<CYCLESEC END>[;<CYCLESEC START>;<CYCLESEC END>]
param;<KEY>;<VALUE>
param;<KEY>;<VALUE>

The [links] block lists the relations between signal groups and junction connections in SUMO. The relation is
build from the edges/lanes controlled by the respective signal group. The target edge/lane can be omitted.

[links]
<SIGNAL_GROUP_ID>;<FROM_LANE/EDGE_ID>;
<SIGNAL_GROUP_ID>;<FROM_LANE/EDGE_ID>;<TARGET_LANE/EDGE_ID>

The last block [signal groups] contains the table of green times and signal group properties. The table uses
a header row with the following keywords and their meanings:

id = signal group id, see block [links]
on1 = start time (signal state green) [s] for the first green time in the cycle
off1 = end time (signal state green) [s] for the first green time in the cycle
on2 = optional start time (signal state green) [s] for the second green time in the cycle
off2 = optional end time (signal state green) [s] for the second green time in the cycle
transOn = duration [s] for special signal state before green (usually red-yellow)
transOff = duration [s] for special signal state after green (usually yellow)

The keywords may have a different order than shown above. The definition of a second green time per cycle
(keywords "on2" and "off2") is optional, so are the keywords.
Call the script with --help to see all the available command line parameters.
"""

from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import io
import re
import csv
import argparse
from collections import OrderedDict
try:
    import xml.etree.cElementTree as ET
except ImportError as e:
    print("recovering from ImportError '%s'" % e)
    import xml.etree.ElementTree as ET

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa


def getEdgeID(laneOrEdgeID):
    sep = laneOrEdgeID.rfind('_')
    if sep > 0 and laneOrEdgeID[sep + 1:].isdigit():
        return laneOrEdgeID[:sep]
    else:
        return laneOrEdgeID


def isLaneID(laneOrEdgeID):
    return laneOrEdgeID != getEdgeID(laneOrEdgeID)


class TlLogic(sumolib.net.TLSProgram):

    def __init__(self, tlID, programID, cycleTime, offset, actuated, parameters, net=None, debug=False):
        if not isinstance(cycleTime, int) or cycleTime < 1:
            print("Invalid cycle time = %s" % str(cycleTime))

        sumolib.net.TLSProgram.__init__(self, tlID, str(offset), "actuated" if actuated else "static")
        self._cycleTime = cycleTime  # cycle time [s]
        self._programID = programID
        self._actuated = actuated
        self._parameters = parameters
        self.net = net
        self._signalGroups = OrderedDict()
        self.__signalGroupOrder = []
        self._allTimes = [0]
        self._debug = debug
        self._tlIndexToSignalGroup = {}

    @staticmethod
    def createFromTLSProgram(tl, tlsProgram, net, debug=False):
        # derive cycle time
        cycleTime = sum([int(phase.duration) for phase in tlsProgram.getPhases()])
        tlLogic = TlLogic(tl.getID(), tlsProgram._id, cycleTime, tlsProgram._offset,
                          [], tlsProgram.getParams(), net=net, debug=debug)
        tlLogic._phases = tlsProgram.getPhases()
        tlLogic.__patchActuationTimes()
        return tlLogic

    @staticmethod
    def createFromXML(tllFile, net, group, debug=False):
        tree = ET.parse(tllFile)
        root = tree.getroot()
        tlLogics = []
        tlLogicEls = root.findall("tlLogic")
        for tlLogicEl in tlLogicEls:
            phaseEls = tlLogicEl.findall("phase")
            if len(phaseEls) > 0:
                paramEls = tlLogicEl.findall("param")
                params = {paramEl.attrib["key"]: paramEl.attrib["value"] for paramEl in paramEls}
                cycleTime = sum(int(phaseEl.attrib["duration"]) for phaseEl in phaseEls)
                tlLogic = TlLogic(tlLogicEl.attrib["id"], tlLogicEl.attrib["programID"], cycleTime,
                                  tlLogicEl.attrib["offset"], [], params, net=net, debug=debug)
                tlLogic._phases = [sumolib.net.Phase(int(phaseEl.attrib["duration"]), phaseEl.attrib["state"],
                                                     int(phaseEl.attrib.get("minDur", -1)),
                                                     int(phaseEl.attrib.get("maxDur", -1))) for phaseEl in phaseEls]
                tlLogic.createSignalGroupsFromPhases(group=group)
                tlLogic.__patchActuationTimes()
                tlLogics.append(tlLogic)
        return tlLogics

    def addSignalGroups(self, signalGroups, signalGroupOrder=None):
        for signalGroup in signalGroups.values():
            self.__registerSignalGroup(signalGroup)
        if signalGroupOrder is not None:
            self.__signalGroupOrder = signalGroupOrder
        else:
            self.__signalGroupOrder = list(self.signalGroups.keys())
            self.__signalGroupOrder.sort()

    def __registerSignalGroup(self, signalGroup, tlIndices=[]):
        self._signalGroups[signalGroup._id] = signalGroup
        self._signalGroups[signalGroup._id].tlLogic = self
        for tlIndex in tlIndices:
            self._tlIndexToSignalGroup[tlIndex] = signalGroup._id

    def __patchActuationTimes(self):
        tx = 0
        for phase in self._phases:
            if phase.minDur > 0:
                self._actuated.extend([tx + phase.minDur, tx + phase.duration])
            tx += phase.duration
        if len(self._actuated) > 0:
            self._type = "actuated"

    def __assignConnections(self):
        if self.net is None:
            return
        connections = self.net.getTLS(self._id).getConnections()
        for conn in connections:
            if conn[2] in self._tlIndexToSignalGroup:
                self._signalGroups[self._tlIndexToSignalGroup[conn[2]]].addConnection(*conn)

    def createSignalGroupsFromPhases(self, group=False):
        # only for fresh instances
        if len(self._signalGroups) > 0:
            return
        # create signal state patterns by aggregating the phase states vertically
        states = [s for phase in self._phases for s in [phase.state] * int(phase.duration)]
        sgStates = [''.join(state) for state in zip(*states)]
        tlIndexMap = [[i] for i in range(len(sgStates))]
        j = 0
        if group:
            uniqueSgStates = list(dict.fromkeys(sgStates))
            uniqueSgStates.sort()
            if len(uniqueSgStates) < len(sgStates):
                tlIndexMap = [[i for i, value in enumerate(sgStates) if value == sgState] for sgState in uniqueSgStates]
            sgStates = uniqueSgStates
        for sgState in sgStates:
            sgState = sgState.lower()  # ignore minor/major semantics
            greenPhases = re.findall("(u*)(g+)(y*)", sgState)[:2]
            switchTimes = []
            if len(greenPhases) > 0:
                transOn = min([len(greenPhase[0]) for greenPhase in greenPhases])
                transOff = min([len(greenPhase[2]) for greenPhase in greenPhases])
                greenSearchIndex = 1 if sgState.startswith("g") and sgState.endswith("g") else 0
                for greenPhase in greenPhases:
                    tStart = greenSearchIndex + sgState.index("g", greenSearchIndex)
                    tEnd = (tStart + len(greenPhase[1])) % self._cycleTime
                    greenSearchIndex = tEnd
                    switchTimes.append((tStart, tEnd))
            else:
                transOn = 0
                transOff = 0
            sg = SignalGroup("SG%d" % j, transTimeOff=transOff, transTimeOn=transOn)
            for switchTime in switchTimes:
                sg.addFreeTime(*switchTime)
            self.__registerSignalGroup(sg, tlIndexMap[j])
            j += 1
        self.__assignConnections()

    def _timeToCycle(self, time):
        return time % self._cycleTime

    def setFreeTime(self):
        for sg in self._signalGroups.values():
            sg._times = {}  # clear previous entries
            for fromTime, toTime in sg._freeTimes:  # calculate times of signal state changes
                if sg._transTimes[0] > 0:
                    sg._times[self._timeToCycle(fromTime - sg._transTimes[0])] = sg._start
                sg._times[fromTime] = sg._free
                if sg._transTimes[1] > 0:
                    sg._times[self._timeToCycle(toTime)] = sg._stop
                    sg._times[self._timeToCycle(toTime + sg._transTimes[1])] = sg._red
                else:
                    sg._times[self._timeToCycle(toTime)] = sg._red
                for timeKey in sg._times:
                    if timeKey not in self._allTimes:
                        self._allTimes.append(timeKey)
        if self._debug:
            print("All switch times: %s" % str(sorted(self._allTimes)))
        for sg in sorted(self._signalGroups.keys()):
            self._signalGroups[sg].calculateCompleteSignals(self._allTimes)

    def setSignalGroupRelations(self, sgToLinks):
        if self.net is not None:
            tls = self.net._id2tls[self._id]
            connections = tls._connections
            for sgID in sorted(sgToLinks.keys()):
                for fromLink, toLink in sgToLinks[sgID]:
                    # check link validity (lane exists?)
                    for connIn, connOut, tlIndex in connections:
                        if isLaneID(fromLink):
                            valid = fromLink == connIn.getID()
                        else:
                            valid = fromLink == connIn.getEdge().getID()
                        if toLink != '' and valid:
                            if isLaneID(toLink):
                                valid = toLink == connOut.getID()
                            else:
                                valid = toLink == connOut.getEdge().getID()
                        if valid:
                            if self._debug:
                                print("Valid description from %s to %s (SG %s, tlIndex %d)" %
                                      (connIn.getID(), connOut.getID(), sgID, tlIndex))
                            if tlIndex not in self._tlIndexToSignalGroup:
                                self._signalGroups[sgID].addConnection(connIn, connOut, tlIndex)
                                self._tlIndexToSignalGroup[tlIndex] = sgID
                            elif self._tlIndexToSignalGroup[tlIndex] != sgID:
                                print(("Error: linkIndex %d already bound to signal group %s. " +
                                       "Cannot assign it to signal group %s.") % (
                                      tlIndex, self._tlIndexToSignalGroup[tlIndex], sgID))
                                sys.exit(-1)

            # set dummy signal groups for every uncovered linkIndex to output "o"/"O" signal
            for connection in connections:
                tlIndex = connection[2]
                if tlIndex not in self._tlIndexToSignalGroup:
                    sgID = ("tlIndex_%d" % tlIndex)
                    self._signalGroups[sgID] = SignalGroup(sgID, off=True)
                    self._signalGroups[sgID].tlLogic = self
                    self._signalGroups[sgID].addConnection(connection[0], connection[1], connection[2])
                    self._tlIndexToSignalGroup[tlIndex] = sgID
            # check for missing linkIndex values
            tlIndexMax = max(self._tlIndexToSignalGroup.keys())
            for tlIndex in range(0, tlIndexMax):
                if tlIndex not in self._tlIndexToSignalGroup:
                    sgID = ("tlIndex_%d" % tlIndex)
                    self._signalGroups[sgID] = SignalGroup(sgID, off=True)
                    self._signalGroups[sgID].addTlIndex(tlIndex)
                    self._signalGroups[sgID].tlLogic = self
                    self._tlIndexToSignalGroup[tlIndex] = sgID

    def xmlOutput(self, doc):
        '''
        Print out the TL Logic in the XML format used in SUMO.
        '''
        # transform signal group based information to "phase" elements of constant signal states
        # TODO: insert tlIndex in completeSignals query
        self._allTimes.sort()
        tlIndices = []
        if len(self._tlIndexToSignalGroup) > 0:
            tlIndices = list(self._tlIndexToSignalGroup.keys())
            tlIndices.sort()
        elif self._debug:
            print("No tlIndex to signal group relation available: Signal program will be empty.")
        tlEl = doc.addChild("tlLogic")
        tlEl.setAttribute("id", self._id)
        tlEl.setAttribute("type", self._type)
        tlEl.setAttribute("programID", self._programID)
        tlEl.setAttribute("offset", self._offset)

        tlEl.setText("<!-- Order of signal groups:\n%s-->\n" % "\n".join(
            [str(tlIndex) + " " + self._tlIndexToSignalGroup[tlIndex] for tlIndex in tlIndices]))

        # output custom parameters
        for key in self._parameters:
            parEl = tlEl.addChild("param")
            parEl.setAttribute("key", key)
            parEl.setAttribute("value", self._parameters[key])

        actuatedIdx = 0
        for i in range(len(self._allTimes)):
            states = {}
            for tlIndex in self._tlIndexToSignalGroup:
                sgID = self._tlIndexToSignalGroup[tlIndex]
                states[tlIndex] = self._signalGroups[sgID].completeSignals[tlIndex][self._allTimes[i]]
            # fill duration up to the cycle time
            if i == len(self._allTimes) - 1:
                duration = self._cycleTime - self._allTimes[i]
            else:
                duration = self._allTimes[i + 1] - self._allTimes[i]
            states = "".join([states[tlIndex] for tlIndex in tlIndices])
            phaseEl = tlEl.addChild("phase")
            phaseEl.setAttribute("duration", str(duration))
            phaseEl.setAttribute("state", states)
            if actuatedIdx + 1 < len(self._actuated):
                start, end = self._actuated[actuatedIdx:actuatedIdx + 2]
                if start >= self._allTimes[i] and end <= self._allTimes[i] + duration:
                    phaseEl.setAttribute("minDur", duration - (end - start))
                    phaseEl.setAttribute("maxDur", duration)
                    actuatedIdx += 2
        return tlEl

    def csvOutput(self, f):
        '''
        Print out the TL Logic in the CSV input format
        '''
        print("[general]\ncycle time;%d\nkey;%s\nsubkey;%s\noffset;%s" %
              (self._cycleTime, self._id, self._programID, int(float(self._offset))), file=f)
        if self._actuated:
            print("actuated;%s" % ";".join([str(value) for value in self._actuated]), file=f)
        for key, value in self._parameters.items():
            print("param;%s;%s" % (key, value), file=f)
        print("[links]", file=f)
        for id, sg in self._signalGroups.items():
            inOutRelations = []
            for conn in sg._connections:
                outGoingConns = conn.getFrom().getOutgoing()[conn.getTo()]
                # note lane IDs if there are links with same start and end edge not controlled this signal group
                # edge IDs otherwise
                if len(set([outConn.getTLLinkIndex() for outConn in outGoingConns])) > 1:
                    inOutRelations.append((conn.getFromLane().getID(), conn.getToLane().getID()))
                else:
                    inOutRelations.append((conn.getFrom().getID(), conn.getTo().getID()))
            inOutRelations.sort(key=lambda t: t[1])
            inOutRelations.sort(key=lambda t: t[0])
            for entry in inOutRelations:
                print("%s;%s;%s" % (id, entry[0], entry[1]), file=f)
        print("[signal groups]\nid;on1;off1;on2;off2;transOn;transOff", file=f)
        for id, sg in self._signalGroups.items():
            freeTimes = [str(t) for freeTime in sg._freeTimes for t in freeTime]
            freeTimes.extend(['']*(4-len(freeTimes)))
            print("%s;%s;%d;%d" % (id, ";".join(freeTimes), sg._transTimes[0], sg._transTimes[1]), file=f)


class SignalGroup(object):

    def __init__(self, id, free="g", transTimeOn=0, transTimeOff=0, off=False, debug=False):
        self._id = id
        self._free = free
        self._red = "r"
        self._start = "u"
        self._stop = "y"
        self._transTimes = (max(0, transTimeOn), max(0, transTimeOff))
        self._times = {}
        self._freeTimes = []
        self.completeSignals = {}
        self.tlLogic = None
        self._tlIndexToYield = {}
        self._debug = debug
        self._off = off
        self._connections = set()

    def addFreeTime(self, fromTime, toTime):
        if fromTime != toTime:
            self._freeTimes.append((fromTime, toTime))

    def addTlIndex(self, tlIndex):
        self._tlIndexToYield[tlIndex] = []

    def addConnection(self, connIn, connOut, tlIndex):
        junction = connIn.getEdge().getToNode()
        ownConn = None
        if junction is not None:
            # get junction index of the connection
            for conn in junction.getConnections():
                if conn.getFromLane() == connIn and conn.getToLane() == connOut:
                    ownConn = conn
                    break
        # store yielding info based on tlIndex values
        if tlIndex not in self._tlIndexToYield:
            self._tlIndexToYield[tlIndex] = []
        if ownConn is not None:
            self._connections.add(ownConn)
            for conn in junction.getConnections():
                if self.connectionYields(ownConn, conn):
                    self._tlIndexToYield[tlIndex].append(conn.getTLLinkIndex())

    def connectionYields(self, ownConn, conn):
        result = False
        if ownConn.getJunction() == conn.getJunction():
            otherTlIndex = conn.getTLLinkIndex()
            if otherTlIndex >= 0 and conn.getTLSID() == self.tlLogic._id:
                prohibits = ownConn.getJunction()._prohibits
                ownJunctionIndex = ownConn.getJunctionIndex()
                if ownJunctionIndex in prohibits:
                    result = prohibits[ownConn.getJunctionIndex()][len(prohibits) - conn.getJunctionIndex() - 1] == '1'
        return result

    def calculateCompleteSignals(self, times):
        for tlIndex in sorted(self._tlIndexToYield):
            if self._debug:
                print("SG %s: tlIndex %d: yield tlIndices %s" % (self._id, tlIndex, str(self._tlIndexToYield[tlIndex])))
            self.completeSignals[tlIndex] = {}
            for time in times:
                self.completeSignals[tlIndex][time] = self.getStateAt(time, tlIndex)

    def getStateAt(self, time, tlIndex, checkPriority=True):
        result = "o"
        wait = False

        if len(self._times) > 0 and tlIndex in self._tlIndexToYield:
            timeKeys = list(self._times.keys())
            timeKeys.sort()
            relevantKey = None
            if time < timeKeys[0] or time >= timeKeys[len(timeKeys) - 1]:
                relevantKey = timeKeys[len(timeKeys) - 1]
            else:
                for i in range(0, len(timeKeys) - 1):
                    if time >= timeKeys[i] and time < timeKeys[i + 1]:
                        relevantKey = timeKeys[i]
                        break
            result = self._times[relevantKey]

            if checkPriority and result in ["o", "g"]:
                for yieldTlIndex in self._tlIndexToYield[tlIndex]:
                    # ask signal state of signal to yield
                    if yieldTlIndex in self.tlLogic._tlIndexToSignalGroup:
                        sgID = self.tlLogic._tlIndexToSignalGroup[yieldTlIndex]
                        yieldSignal = self.tlLogic._signalGroups[sgID].getStateAt(
                            time, yieldTlIndex, checkPriority=False)
                        # Do not bother for "y" or "u": prioritary vehicles should not drive
                        wait = yieldSignal in ["g", "G", "o", "O"]
                        # (("SG %s (tlIndex %d) at time %d (state %s) has to wait for SG %s " +
                        # "(tlIndex %d, state %s)? %s") % (
                        #  self._id, tlIndex, time, result, sgID, yieldTlIndex, yieldSignal, str(wait)))
                        if wait:
                            break
        elif tlIndex in self._tlIndexToYield:
            wait = len(self._tlIndexToYield[tlIndex]) > 0
        if result in ["g", "o"] and not wait:  # prioritary signal
            result = result.upper()
        return result

    def __str__(self):
        return "SignalGroup %s (%ds %s %s %ds %s) has %d free times" % (
               self._id, self._transTimes[0], self._start, self._free, self._transTimes[1], self._stop,
               len(self._freeTimes))


def writeXmlOutput(tlList, outputFile):
    if len(tlList) > 0:
        root = sumolib.xml.create_document("additional")
        for tlLogic in tlList:
            tlLogic.xmlOutput(root)
        with open(outputFile, 'w') as out:
            sumolib.xml.writeHeader(out)
            out.write(root.toXML())


def writeInputTemplates(net, outputDir, delimiter):
    # identify tls-controlled junctions
    for tlsID in net._id2tls:
        # get all connections per junction from the incoming lanes
        connections = net._id2tls[tlsID].getConnections()

        # create the data
        data = [["[general]"], ["cycle time"], ["key", tlsID], ["subkey", "0"], ["offset", "0"], ["[links]"]]
        for connIn, connOut, tlIndex in connections:
            data.append(["SG_" + str(tlIndex), connIn.getID(), connOut.getID()])
        data.extend([["[signal groups]"], ["id", "on1", "off1", "on2", "off2", "transOn", "transOff"]])
        for connIn, connOut, tlIndex in connections:
            data.append(["SG_" + str(tlIndex)])

        # write the template file
        fopenArgs = {'mode': 'w', 'newline': ''}
        if sys.version_info.major < 3:
            fopenArgs = {'mode': 'wb'}
        with io.open(os.path.join(outputDir, "%s.csv" % tlsID), **fopenArgs) as inputTemplate:
            csvWriter = csv.writer(inputTemplate, quoting=csv.QUOTE_NONE, delimiter=delimiter)
            csvWriter.writerows(data)


def toTll(options):
    # read general and signal groub based information from input file(s)
    sections = ["general", "links", "signal groups"]
    signalColumns = ["id", "on1", "off1", "on2", "off2", "transOn", "transOff"]
    if(len(options.input) == 0):
        inputFiles = []
    else:
        inputFiles = options.input.split(',')
    tlList = []

    # read SUMO network
    net = None
    if len(options.net) > 0:
        net = sumolib.net.readNet(options.net,
                                  withInternal=True,
                                  withPedestrianConnections=True)

        if len(options.make_input_dir) > 0:  # check input template directory
            if(os.path.isdir(options.make_input_dir)):
                writeInputTemplates(net, options.make_input_dir, options.delimiter)
            else:
                sys.stderr.write("The input template directory %s does not exist.\n" % options.make_input_dir)
                sys.exit(-1)

    for inputFileName in inputFiles:  # one signal program per input file
        readSgHeader = False
        secondFreeTime = False
        colIndices = {}
        activeSection = None
        cycleTime = 90
        key = "SZP 1"
        subkey = "0"
        signalGroups = {}
        signalGroupOrder = []
        sgToLinks = {}
        parameters = {}
        actuated = []

        with open(inputFileName, 'r') as inputFile:
            inputReader = csv.reader(inputFile, delimiter=options.delimiter, quotechar='"')
            for i, line in enumerate(inputReader):
                try:
                    cell0 = line[0].strip() if line else ""

                    # skip lines with empty first cell
                    if len(cell0) == 0:
                        continue

                    # type of input announced by section title
                    if cell0.startswith("[") and cell0.endswith("]"):
                        if cell0[1:-1] in sections:
                            activeSection = cell0[1:-1]
                            continue
                        else:
                            print("Unknown input section %s." % cell0)
                            sys.exit()

                    # general TLS input
                    if activeSection == "general":
                        if cell0 == "cycle time":
                            cycleTime = int(line[1])
                        elif cell0 == "key":
                            key = line[1].strip()
                        elif cell0 == "subkey":
                            subkey = line[1].strip()
                        elif cell0 == "offset":
                            offset = int(line[1])
                        elif cell0 == "param":
                            parameters[line[1].strip()] = line[2].strip()
                        elif cell0 == "actuated":
                            actuated = [int(entry) for entry in line[1:]]

                    # relation between signal groups and network connection elements
                    elif activeSection == "links":
                        link = (line[1], line[2])
                        if cell0 not in sgToLinks:
                            sgToLinks[cell0] = []
                        sgToLinks[cell0].append(link)

                    # define green times (once or twice per cycle time) and special transitional signal states
                    # (yellow...)
                    elif activeSection == "signal groups":
                        if not readSgHeader:  # remember relation between columns and their meanings
                            readSgHeader = True
                            for colIndex in range(0, len(line)):
                                if line[colIndex].strip() in signalColumns:
                                    colIndices[line[colIndex].strip()] = colIndex
                            secondFreeTime = "on2" in colIndices.keys() and "off2" in colIndices.keys()
                        else:
                            sgID = line[colIndices["id"]]
                            sg = SignalGroup(sgID,
                                             transTimeOn=int(line[colIndices["transOn"]]),
                                             transTimeOff=int(line[colIndices["transOff"]]),
                                             debug=options.debug)
                            sg.addFreeTime(int(line[colIndices["on1"]]), int(line[colIndices["off1"]]))
                            if(secondFreeTime):
                                if(line[colIndices["on2"]] != "" and line[colIndices["off2"]] != ""):
                                    sg.addFreeTime(int(line[colIndices["on2"]]), int(line[colIndices["off2"]]))
                            signalGroups[sgID] = sg
                            signalGroupOrder.append(sgID)
                            sgFromLinkEdge = getEdgeID(sgToLinks[sgID][0][0])
                            if net.getEdge(sgFromLinkEdge).getFunction() == "walkingarea":
                                sg._stop = 'r'

                except Exception:
                    print("In file %s, line %s" % (inputFileName, i + 1), file=sys.stderr)
                    raise

        # build everything together
        tlLogic = TlLogic(key, subkey, cycleTime, offset, actuated, parameters, net=net, debug=options.debug)
        tlLogic.addSignalGroups(signalGroups, signalGroupOrder)
        tlLogic.setSignalGroupRelations(sgToLinks)
        tlLogic.setFreeTime()
        tlList.append(tlLogic)

    writeXmlOutput(tlList, options.output)


def toCsv(options):
    # read the network first
    net = sumolib.net.readNet(options.net, withInternal=True, withPedestrianConnections=True, withPrograms=True)
    # load tll files
    addLogics = {}
    filterIDs = options.tlsFilter.split(",") if len(options.tlsFilter) > 0 else []
    tllFiles = [f for f in options.input.split(",") if f]
    for tllFile in tllFiles:
        if len(filterIDs) == 0:
            tlLogics = [item for item in TlLogic.createFromXML(
                tllFile, net=net, group=options.group, debug=options.debug)]
        else:
            tlLogics = [item for item in TlLogic.createFromXML(
                tllFile, net=net, group=options.group, debug=options.debug) if item._id in filterIDs]
        for tlLogic in tlLogics:
            if tlLogic._id not in addLogics:
                addLogics[tlLogic._id] = []
            addLogics[tlLogic._id].append(tlLogic)

    tlLogicsTotal = []
    tlss = net.getTrafficLights()
    for tls in tlss:
        if len(filterIDs) > 0 and tls.getID() not in filterIDs:
            continue
        tlLogics = addLogics[tls.getID()] if tls.getID() in addLogics else []
        if options.tlsFromNet:
            tlPrograms = tls.getPrograms()
            for tlProgram in tlPrograms.values():
                tlLogic = TlLogic.createFromTLSProgram(tls, tlProgram, net=net, debug=options.debug)
                tlLogic.createSignalGroupsFromPhases(group=options.group)
                tlLogics.append(tlLogic)
        # check for same signal groups
        if options.group:
            if not len(set([len(tll._signalGroups) for tll in tlLogics])) == 1:
                print("Signal states of TL %s cannot be grouped unambiguously. "
                      "Please remove the group option or the contradictory tll file." % tls.getID())
                return
        tlLogicsTotal.extend(tlLogics)
    # write the csv format
    prefix = options.output if len(options.output) > 0 else ''
    for tlLogic in tlLogicsTotal:
        outputPath = "%s%s_%s.csv" % (prefix, tlLogic._id, tlLogic._programID)
        with open(outputPath, "w") as f:
            tlLogic.csvOutput(f)


def getOptions():
    ap = argparse.ArgumentParser()
    ap.add_argument("-o", "--output", action="store", default="tls.add.xml",
                    help="File path to tll output file (SUMO additional file) / prefix for generated csv files")
    ap.add_argument("-i", "--input", action="store", default="",
                    help="File path to input csv or tll file(s). Multiple file paths have to be separated by ','.")
    ap.add_argument("-r", "--reverse", action="store_true", default=False,
                    help="Interpret input files in tll format and convert them to csv files.")
    ap.add_argument("-g", "--group", action="store_true", default=False,
                    help="Join signals with identical states into one signal group when converting to csv format.")
    ap.add_argument("--tls-from-net", action="store_true", default=False, dest="tlsFromNet",
                    help="Convert TL programs stored within the net file to csv format.")
    ap.add_argument("--tls-filter", action="store", default="", dest="tlsFilter",
                    help="Comma-separated list of traffic lights " +
                    "which the reverse conversion from tll to csv should be limited to.")
    ap.add_argument("--delimiter", action="store", default=";",
                    help="CSV delimiter used for input and template files.")
    ap.add_argument("-n", "--net", action="store", default="",
                    help="File path to SUMO network file. Optional for creating TL xml, " +
                    "obligatory for converting TL xml to csv.")
    ap.add_argument("-m", "--make-input-dir", action="store", default="",
                    help="Create input file template(s) from the SUMO network file in the given directory.")
    ap.add_argument("-d", "--debug", action="store_true", default=False, help="Output debugging information")
    options = ap.parse_args()
    return options


# this is the main entry point of this script
if __name__ == "__main__":
    options = getOptions()
    if options.reverse:
        if len(options.output) > 0 and options.output.endswith(".add.xml"):
            options.output = ""
        if len(options.net) == 0:
            print("Cannot convert TL xml to csv due to missing network file.")
        elif not options.tlsFromNet and len(options.input) == 0:
            print("Missing tll file to convert to csv.")
        else:
            toCsv(options)
    else:
        toTll(options)
