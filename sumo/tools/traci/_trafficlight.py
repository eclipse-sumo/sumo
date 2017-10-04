# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _trafficlight.py
# @author  Michael Behrisch
# @date    2011-03-16
# @version $Id$

from __future__ import absolute_import
import struct
from .domain import Domain
from .storage import Storage
from . import constants as tc
from .exceptions import TraCIException

class Phase:

    def __init__(self, duration, duration1, duration2, phaseDef):
        self._duration = duration
        self._duration1 = duration1 # minimum duration (only for actuated tls)
        self._duration2 = duration2 # maximum duration (only for actuated tls)
        self._phaseDef = phaseDef

    def __repr__(self):
        return ("Phase:\nduration: %s\nminDuration: %s\nmaxDuration: %s\nphaseDef: %s\n" %
                (self._duration, self._duration1, self._duration2, self._phaseDef))


class Logic:

    def __init__(self, subID, type, subParameter, currentPhaseIndex, phases):
        self._subID = subID
        self._type = type
        self._subParameter = subParameter
        self._currentPhaseIndex = currentPhaseIndex
        self._phases = phases

    def __repr__(self):
        result = ("Logic:\nsubID: %s\ntype: %s\nsubParameter: %s\ncurrentPhaseIndex: %s\n" %
                  (self._subID, self._type, self._subParameter, self._currentPhaseIndex))
        for p in self._phases:
            result += str(p)
        return result


def _readLogics(result):
    result.readLength()
    nbLogics = result.read("!i")[0]    # Number of logics
    logics = []
    for i in range(nbLogics):
        result.read("!B")                       # Type of SubID
        subID = result.readString()
        result.read("!B")                       # Type of Type
        type = result.read("!i")[0]             # Type
        result.read("!B")                       # Type of SubParameter
        subParameter = result.read("!i")[0]     # SubParameter
        result.read("!B")                       # Type of Current phase index
        currentPhaseIndex = result.read("!i")[0]    # Current phase index
        result.read("!B")                       # Type of Number of phases
        nbPhases = result.read("!i")[0]         # Number of phases
        phases = []
        for j in range(nbPhases):
            result.read("!B")                   # Type of Duration
            duration = result.read("!i")[0]     # Duration
            result.read("!B")                   # Type of Duration1
            duration1 = result.read("!i")[0]    # Duration1
            result.read("!B")                   # Type of Duration2
            duration2 = result.read("!i")[0]    # Duration2
            result.read("!B")                   # Type of Phase Definition
            phaseDef = result.readString()      # Phase Definition
            phase = Phase(duration, duration1, duration2, phaseDef)
            phases.append(phase)
        logic = Logic(subID, type, subParameter, currentPhaseIndex, phases)
        logics.append(logic)
    return logics


def _readLinks(result):
    result.readLength()
    nbSignals = result.read("!i")[0]  # Length
    signals = []
    for i in range(nbSignals):
        # Type of Number of Controlled Links
        result.read("!B")
        # Number of Controlled Links
        nbControlledLinks = result.read("!i")[0]
        controlledLinks = []
        for j in range(nbControlledLinks):
            result.read("!B")                       # Type of Link j
            link = result.readStringList()          # Link j
            controlledLinks.append(link)
        signals.append(controlledLinks)
    return signals


_RETURN_VALUE_FUNC = {tc.TL_RED_YELLOW_GREEN_STATE: Storage.readString,
                      tc.TL_COMPLETE_DEFINITION_RYG: _readLogics,
                      tc.TL_CONTROLLED_LANES: Storage.readStringList,
                      tc.TL_CONTROLLED_LINKS: _readLinks,
                      tc.TL_CURRENT_PROGRAM: Storage.readString,
                      tc.TL_CURRENT_PHASE: Storage.readInt,
                      tc.TL_NEXT_SWITCH: Storage.readInt,
                      tc.TL_PHASE_DURATION: Storage.readInt}


class TrafficLightDomain(Domain):

    Phase = Phase
    Logic = Logic

    def __init__(self, name="trafficlight", deprecatedFor=None):
        Domain.__init__(self, name, tc.CMD_GET_TL_VARIABLE, tc.CMD_SET_TL_VARIABLE,
                        tc.CMD_SUBSCRIBE_TL_VARIABLE, tc.RESPONSE_SUBSCRIBE_TL_VARIABLE,
                        tc.CMD_SUBSCRIBE_TL_CONTEXT, tc.RESPONSE_SUBSCRIBE_TL_CONTEXT,
                        _RETURN_VALUE_FUNC, deprecatedFor)

    def getRedYellowGreenState(self, tlsID):
        """getRedYellowGreenState(string) -> string

        Returns the named tl's state as a tuple of light definitions from
        rugGyYoO, for red, yed-yellow, green, yellow, off, where lower case letters mean that the stream has to decelerate.
        """
        return self._getUniversal(tc.TL_RED_YELLOW_GREEN_STATE, tlsID)

    def getCompleteRedYellowGreenDefinition(self, tlsID):
        """getCompleteRedYellowGreenDefinition(string) ->

        .
        """
        return self._getUniversal(tc.TL_COMPLETE_DEFINITION_RYG, tlsID)

    def getControlledLanes(self, tlsID):
        """getControlledLanes(string) -> c

        Returns the list of lanes which are controlled by the named traffic light.
        """
        return self._getUniversal(tc.TL_CONTROLLED_LANES, tlsID)

    def getControlledLinks(self, tlsID):
        """getControlledLinks(string) -> list(list(list(string)))

        Returns the links controlled by the traffic light, sorted by the signal index and described by giving the incoming, outgoing, and via lane.
        """
        return self._getUniversal(tc.TL_CONTROLLED_LINKS, tlsID)

    def getProgram(self, tlsID):
        """getProgram(string) -> string

        Returns the id of the current program.
        """
        return self._getUniversal(tc.TL_CURRENT_PROGRAM, tlsID)

    def getPhase(self, tlsID):
        """getPhase(string) -> integer

        .
        """
        return self._getUniversal(tc.TL_CURRENT_PHASE, tlsID)

    def getNextSwitch(self, tlsID):
        """getNextSwitch(string) -> integer

        .
        """
        return self._getUniversal(tc.TL_NEXT_SWITCH, tlsID)

    def getPhaseDuration(self, tlsID):
        """getPhaseDuration(string) -> integer

        .
        """
        return self._getUniversal(tc.TL_PHASE_DURATION, tlsID)

    def setRedYellowGreenState(self, tlsID, state):
        """setRedYellowGreenState(string, string) -> None

        Sets the named tl's state as a tuple of light definitions from
        rugGyYuoO, for red, red-yellow, green, yellow, off, where lower case letters mean that the stream has to decelerate.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_TL_VARIABLE, tc.TL_RED_YELLOW_GREEN_STATE, tlsID, state)

    def setLinkState(self, tlsID, tlsLinkIndex, state):
        """setLinkState(string, string, int, string) -> None
        Sets the state for the given tls and link index. The state must be one
        of rRgGyYoOu for red, red-yellow, green, yellow, off, where lower case letters mean that the stream has to decelerate.
        The link index is shown the gui when setting the appropriate junctino
        visualization optin.
        """
        fullState = list(self.getRedYellowGreenState(tlsID))
        if tlsLinkIndex >= len(fullState):
            raise TraCIException(None, None, "Invalid tlsLinkIndex %s for tls '%s' with maximum index %s." % (
                tlsLinkIndex, tlsID, len(fullState) - 1))
        else:
            fullState[tlsLinkIndex] = state
            self.setRedYellowGreenState(tlsID, ''.join(fullState))

    def setPhase(self, tlsID, index):
        """setPhase(string, integer) -> None

        .
        """
        self._connection._sendIntCmd(
            tc.CMD_SET_TL_VARIABLE, tc.TL_PHASE_INDEX, tlsID, index)

    def setProgram(self, tlsID, programID):
        """setProgram(string, string) -> None

        Sets the id of the current program.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_TL_VARIABLE, tc.TL_PROGRAM, tlsID, programID)

    def setPhaseDuration(self, tlsID, phaseDuration):
        """setPhaseDuration(string, integer or float) -> None

        Set the phase duration of the current phase in seconds.
        """
        self._connection._sendIntCmd(
            tc.CMD_SET_TL_VARIABLE, tc.TL_PHASE_DURATION, tlsID, int(1000 * phaseDuration))

    def setCompleteRedYellowGreenDefinition(self, tlsID, tls):
        """setCompleteRedYellowGreenDefinition(string, ) -> None

        .
        """
        length = 1 + 4 + 1 + 4 + \
            len(tls._subID) + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 4  # tls parameter
        itemNo = 1 + 1 + 1 + 1 + 1
        for p in tls._phases:
            length += 1 + 4 + 1 + 4 + 1 + 4 + 1 + 4 + len(p._phaseDef)
            itemNo += 4
        self._connection._beginMessage(
            tc.CMD_SET_TL_VARIABLE, tc.TL_COMPLETE_PROGRAM_RYG, tlsID, length)
        self._connection._string += struct.pack("!Bi",
                                                tc.TYPE_COMPOUND, itemNo)
        # programID
        self._connection._packString(tls._subID)
        # type
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, 0)
        # subitems
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
        # index
        self._connection._string += struct.pack("!Bi",
                                                tc.TYPE_INTEGER, tls._currentPhaseIndex)
        # phaseNo
        self._connection._string += struct.pack("!Bi",
                                                tc.TYPE_INTEGER, len(tls._phases))
        for p in tls._phases:
            self._connection._string += struct.pack("!BiBiBi", tc.TYPE_INTEGER,
                                                    p._duration, tc.TYPE_INTEGER, p._duration1, tc.TYPE_INTEGER, p._duration2)
            self._connection._packString(p._phaseDef)
        self._connection._sendExact()


TrafficLightDomain()
TrafficLightDomain("trafficlights", "trafficlight")
