# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _trafficlight.py
# @author  Michael Behrisch
# @date    2011-03-16

from __future__ import absolute_import
from .domain import Domain
from . import constants as tc
from .exceptions import TraCIException, deprecated


class Phase:

    def __init__(self, duration, state, minDur=-1, maxDur=-1, next=tuple(), name=""):
        self.duration = duration
        self.state = state
        self.minDur = minDur  # minimum duration (only for actuated tls)
        self.maxDur = maxDur  # maximum duration (only for actuated tls)
        self.next = next
        self.name = name

    def __repr__(self):
        name = "" if self.name == "" else ", name='%s'" % self.name
        return ("Phase(duration=%s, state='%s', minDur=%s, maxDur=%s, next=%s%s)" %
                (self.duration, self.state, self.minDur, self.maxDur, self.next, name))


class Logic:

    def __init__(self, programID, type, currentPhaseIndex, phases=None, subParameter=None):
        self.programID = programID
        self.type = type
        self.currentPhaseIndex = currentPhaseIndex
        self.phases = phases if phases is not None else []
        self.subParameter = subParameter if subParameter is not None else {}

    def getPhases(self):
        return self.phases

    def getSubID(self):
        return self.programID

    def getType(self):
        return self.type

    def getParameters(self):
        return self.subParameter

    def getParameter(self, key, default=None):
        return self.subParameter.get(key, default)

    def __repr__(self):
        return ("Logic(programID='%s', type=%s, currentPhaseIndex=%s, phases=%s, subParameter=%s)" %
                (self.programID, self.type, self.currentPhaseIndex, self.phases, self.subParameter))


def _readLogics(result):
    numLogics = result.readInt()
    logics = []
    for _ in range(numLogics):
        result.readCompound(5)
        programID = result.readTypedString()
        type = result.readTypedInt()
        currentPhaseIndex = result.readTypedInt()
        numPhases = result.readCompound()
        phases = []
        for __ in range(numPhases):
            result.readCompound(6)
            duration = result.readTypedDouble()
            state = result.readTypedString()
            minDur = result.readTypedDouble()
            maxDur = result.readTypedDouble()
            numNext = result.readCompound()
            next = tuple([result.readTypedInt() for ___ in range(numNext)])
            name = result.readTypedString()
            phases.append(Phase(duration, state, minDur, maxDur, next, name))
        logic = Logic(programID, type, currentPhaseIndex, tuple(phases))
        numParams = result.readCompound()
        for __ in range(numParams):
            key, value = result.readTypedStringList()
            logic.subParameter[key] = value
        logics.append(logic)
    return tuple(logics)


class Constraint:
    def __init__(self, signalId, tripId, foeId, foeSignal, limit, type, mustWait, active=True, param={}):
        self.signalId = signalId
        self.tripId = tripId
        self.foeId = foeId
        self.foeSignal = foeSignal
        self.limit = limit
        self.type = type
        self.mustWait = mustWait
        self.active = active
        self.param = param

    def getParameters(self):
        return self.param

    def getParameter(self, key, default=None):
        return self.param.get(key, default)

    def __repr__(self):
        param = ""
        if self.param:
            kvs = ["%s=%s" % (k, self.param[k]) for k in sorted(self.param.keys())]
            param = " param=%s" % '|'.join(kvs)
        return ("Constraint(signalId=%s tripId=%s, foeId=%s, foeSignal=%s, limit=%s, type=%s, mustWait=%s, active=%s%s)" %  # noqa
                (self.signalId, self.tripId, self.foeId, self.foeSignal,
                 self.limit, self.type, self.mustWait, self.active, param))


def _readLinks(result):
    result.readLength()
    numSignals = result.readInt()  # Length
    signals = []
    for _ in range(numSignals):
        # Type of Number of Controlled Links
        result.read("!B")
        # Number of Controlled Links
        nbControlledLinks = result.read("!i")[0]
        controlledLinks = []
        for __ in range(nbControlledLinks):
            result.read("!B")                       # Type of Link j
            link = result.readStringList()          # Link j
            controlledLinks.append(link)
        signals.append(controlledLinks)
    return signals


def _readConstraints(result):
    result.readLength()
    num = result.readInt()  # Length
    constraints = []
    for _ in range(num):
        signalId = result.readTypedString()
        tripId = result.readTypedString()
        foeId = result.readTypedString()
        foeSignal = result.readTypedString()
        limit = result.readTypedInt()
        type = result.readTypedInt()
        mustWait = bool(result.readTypedByte())
        active = bool(result.readTypedByte())
        paramItems = result.readTypedStringList()
        param = {}
        for i in range(0, len(paramItems), 2):
            param[paramItems[i]] = paramItems[i + 1]
        constraints.append(Constraint(signalId, tripId, foeId, foeSignal, limit, type, mustWait, active, param))
    return constraints


_RETURN_VALUE_FUNC = {tc.TL_COMPLETE_DEFINITION_RYG: _readLogics,
                      tc.TL_CONTROLLED_LINKS: _readLinks,
                      tc.TL_CONSTRAINT: _readConstraints,
                      tc.TL_CONSTRAINT_BYFOE: _readConstraints,
                      tc.TL_CONSTRAINT_SWAP: _readConstraints,
                      }


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
        rugGyYoO, for red, yed-yellow, green, yellow, off, where lower case letters mean that the stream
        has to decelerate.
        """
        return self._getUniversal(tc.TL_RED_YELLOW_GREEN_STATE, tlsID)

    def getAllProgramLogics(self, tlsID):
        """getAllProgramLogics(string) -> list(Logic)

        Returns a list of Logic objects.
        Each Logic encodes a traffic light program for the given tlsID.
        """
        return self._getUniversal(tc.TL_COMPLETE_DEFINITION_RYG, tlsID)

    getCompleteRedYellowGreenDefinition = deprecated("getCompleteRedYellowGreenDefinition")(getAllProgramLogics)

    def getControlledLanes(self, tlsID):
        """getControlledLanes(string) -> c

        Returns the list of lanes which are controlled by the named traffic light.
        """
        return self._getUniversal(tc.TL_CONTROLLED_LANES, tlsID)

    def getControlledLinks(self, tlsID):
        """getControlledLinks(string) -> list(list(list(string)))

        Returns the links controlled by the traffic light, sorted by the signal index and described by giving
        the incoming, outgoing, and via lane.
        """
        return self._getUniversal(tc.TL_CONTROLLED_LINKS, tlsID)

    def getProgram(self, tlsID):
        """getProgram(string) -> string

        Returns the id of the current program.
        """
        return self._getUniversal(tc.TL_CURRENT_PROGRAM, tlsID)

    def getPhase(self, tlsID):
        """getPhase(string) -> integer

        Returns the index of the current phase within the list of all phases of
        the current program.
        """
        return self._getUniversal(tc.TL_CURRENT_PHASE, tlsID)

    def getPhaseName(self, tlsID):
        """getPhaseName(string) -> string
        Returns the name of the current phase.
        """
        return self._getUniversal(tc.VAR_NAME, tlsID)

    def getNextSwitch(self, tlsID):
        """getNextSwitch(string) -> double

        Returns the absolute simulation time at which the traffic light is
        schedule to switch to the next phase (in seconds).
        """
        return self._getUniversal(tc.TL_NEXT_SWITCH, tlsID)

    def getPhaseDuration(self, tlsID):
        """getPhaseDuration(string) -> double

        Returns the total duration of the current phase (in seconds). This value
        is not affected by the elapsed or remaining duration of the current phase.
        """
        return self._getUniversal(tc.TL_PHASE_DURATION, tlsID)

    def getServedPersonCount(self, tlsID, index):
        """getServedPersonCount(string, int) -> int
        Returns the number of persons that would be served in the given phase
        """
        return self._getUniversal(tc.VAR_PERSON_NUMBER, tlsID, "i", index)

    def getBlockingVehicles(self, tlsID, linkIndex):
        """getBlockingVehicles(string, int) -> int
        Returns the list of vehicles that are blocking the subsequent block for
        the given tls-linkIndex
        """
        return self._getUniversal(tc.TL_BLOCKING_VEHICLES, tlsID, "i", linkIndex)

    def getRivalVehicles(self, tlsID, linkIndex):
        """getRivalVehicles(string, int) -> int
        Returns the list of vehicles that also wish to enter the subsequent block for
        the given tls-linkIndex (regardless of priority)
        """
        return self._getUniversal(tc.TL_RIVAL_VEHICLES, tlsID, "i", linkIndex)

    def getPriorityVehicles(self, tlsID, linkIndex):
        """getPriorityVehicles(string, int) -> int
        Returns the list of vehicles that also wish to enter the subsequent block for
        the given tls-linkIndex (only those with higher priority)
        """
        return self._getUniversal(tc.TL_PRIORITY_VEHICLES, tlsID, "i", linkIndex)

    def getConstraints(self, tlsID, tripId=""):
        """getConstraints(string, string) -> list(Constraint)
        Returns the list of rail signal constraints for the given rail signal.
        If tripId is not "", only constraints with the given tripId are
        returned. Otherwise, all constraints are returned
        """
        return self._getUniversal(tc.TL_CONSTRAINT, tlsID, "s", tripId)

    def getConstraintsByFoe(self, foeSignal, foeId=""):
        """getConstraintsByFoe(string, string) -> list(Constraint)
        Returns the list of rail signal constraints that have the given rail
        signal id as their foeSignal.
        If foeId is not "", only constraints with the given foeId are
        returned. Otherwise, all constraints are returned
        """
        return self._getUniversal(tc.TL_CONSTRAINT_BYFOE, foeSignal, "s", foeId)

    def setRedYellowGreenState(self, tlsID, state):
        """setRedYellowGreenState(string, string) -> None

        Sets the named tl's state as a tuple of light definitions from
        rugGyYuoO, for red, red-yellow, green, yellow, off, where lower case letters mean that the stream has
        to decelerate.
        """
        self._setCmd(tc.TL_RED_YELLOW_GREEN_STATE, tlsID, "s", state)

    def setLinkState(self, tlsID, tlsLinkIndex, state):
        """setLinkState(string, string, int, string) -> None
        Sets the state for the given tls and link index. The state must be one
        of rRgGyYoOu for red, red-yellow, green, yellow, off, where lower case letters mean that the stream has
        to decelerate.
        The link index is shown in the GUI when setting the appropriate junction
        visualization option.
        """
        fullState = list(self.getRedYellowGreenState(tlsID))
        if tlsLinkIndex >= len(fullState):
            raise TraCIException("Invalid tlsLinkIndex %s for tls '%s' with maximum index %s." % (
                tlsLinkIndex, tlsID, len(fullState) - 1))
        else:
            fullState[tlsLinkIndex] = state
            self.setRedYellowGreenState(tlsID, ''.join(fullState))

    def setPhase(self, tlsID, index):
        """setPhase(string, integer) -> None

        Switches to the phase with the given index in the list of all phases for
        the current program.
        """
        self._setCmd(tc.TL_PHASE_INDEX, tlsID, "i", index)

    def setPhaseName(self, tlsID, name):
        """setPhaseName(string, string) -> None

        Sets the name of the current phase within the current program
        """
        self._setCmd(tc.VAR_NAME, tlsID, "s", name)

    def setProgram(self, tlsID, programID):
        """setProgram(string, string) -> None

        Switches to the program with the given programID. The program must have
        been loaded earlier. The special value 'off' can always be used to
        switch off the traffic light.
        """
        self._setCmd(tc.TL_PROGRAM, tlsID, "s", programID)

    def getNemaPhaseCalls(self, tlsID):
        """getNemaPhaseCalls(string) -> list(string)
        Get the vehicle calls for the phases.
        The output is vehicle calls (coming from the detectors) for the phases.
        """
        vehCallStr = self.getParameter(tlsID, "NEMA.phaseCall")
        return vehCallStr.split(",")

    def setNemaSplits(self, tlsID, splits):
        """setNemaSplits(string, list(string)) -> None
        Set a new set of splits to the given NEMA-controller.
        This function is only effective for NEMA type of controllers.
        The new splits will be implemented in the next cycle of the control.
        The value must be a space-separated list of 8 numbers with each number
        being the time in seconds for NEMA-phases 1 to 8.
        Time 0 must be used of the phase does not exists.
        Example: “11.0 34.0 15.0 20.0 11.0 34.0 15.0 20.0" (gives total cycle length of 80s)
        """
        if type(splits) == list:
            splits = ' '.join(map(str, splits))
        self.setParameter(tlsID, "NEMA.splits", splits)

    def setNemaMaxGreens(self, tlsID, maxGreens):
        """setNemaMaxGreens(string, list(string)) -> None
        Set a new set of splits to the given NEMA-controller.
        This function is only effective for NEMA type of controllers.
        The new max green will be implemented in the next cycle of the control.
        The value must be a space-separated list of 8 numbers with each number
        being the time in seconds for NEMA-phases 1 to 8.
        Time 0 must be used of the phase does not exists.
        Example: “11.0 34.0 15.0 20.0 11.0 34.0 15.0 20.0"
        """
        if type(maxGreens) == list:
            maxGreens = ' '.join(map(str, maxGreens))
        self.setParameter(tlsID, "NEMA.maxGreens", maxGreens)

    def setNemaCycleLength(self, tlsID, cycleLength):
        """setNemaCycleLength(string, double) -> None
        Set a new cycle length to the given NEMA-controller.
        This function is only effective for NEMA type of controllers.
        The new cycle length will be implemented in the next cycle of the control.
        This function should be used with setNemaSplits or setNemaMaxGreen.
        """
        self.setParameter(tlsID, "NEMA.cycleLength", str(cycleLength))

    def setNemaOffset(self, tlsID, offset):
        """setNemaOffset(string, double) -> None
        Set a new offset to the given NEMA-controller.
        This function is only effective for NEMA type controllers when operating under coordinated mode.
        The new offset will be implemented in the next cycle of the control by adjusting
        the actual green time of the coordinated phase.
        There is no transition implemented in the NEMA-controller for changing the offset.
        It’s expected that the users will control the change of the offset in each cycle
        to implement their own transition algorithm.
        """
        self.setParameter(tlsID, "NEMA.offset", str(offset))

    def setPhaseDuration(self, tlsID, phaseDuration):
        """setPhaseDuration(string, double) -> None

        Set the remaining phase duration of the current phase in seconds.
        This value has no effect on subsquent repetitions of this phase.
        """
        self._setCmd(tc.TL_PHASE_DURATION, tlsID, "d", phaseDuration)

    def setProgramLogic(self, tlsID, tls):
        """setProgramLogic(string, Logic) -> None

        Sets a new program for the given tlsID from a Logic object.
        See getCompleteRedYellowGreenDefinition.
        """
        format = "tsiit"
        values = [5, tls.programID, tls.type, tls.currentPhaseIndex, len(tls.phases)]
        for p in tls.phases:
            format += "tdsddt"
            values += [6, p.duration, p.state, p.minDur, p.maxDur, len(p.next)]
            for n in p.next:
                format += "i"
                values += [n]
            format += "s"
            values += [p.name]
        # subparams
        format += "t"
        values += [len(tls.subParameter)]
        for par in tls.subParameter.items():
            format += "l"
            values += [par]
        self._setCmd(tc.TL_COMPLETE_PROGRAM_RYG, tlsID, format, *values)

    setCompleteRedYellowGreenDefinition = deprecated("setCompleteRedYellowGreenDefinition")(setProgramLogic)

    def swapConstraints(self, tlsID, tripId, foeSignal, foeId):
        """swapConstraints(string, string, string, string) -> list(Constraint)
        Reverse the given constraint and return list of new constraints that
        were created (by swapping) to avoid deadlock.
        """
        return self._getUniversal(tc.TL_CONSTRAINT_SWAP, tlsID, "tsss", 3, tripId, foeSignal, foeId)

    def removeConstraints(self, tlsID, tripId, foeSignal, foeId):
        """removeConstraints(string, string, string, string)
        remove constraints with the given values. Any combination of inputs may
        be set to "" to act as a wildcard filter """
        self._setCmd(tc.TL_CONSTRAINT_REMOVE, tlsID, "tsss", 3, tripId, foeSignal, foeId)

    def updateConstraints(self, vehID, tripId=""):
        """getConstraints(string, string)
        Removes all constraints that can no longer be met because the route of
        vehID does not pass traffic light of the constraint with the given tripId.
        This includes constraints on tripId as well as constraints where tripId
        is the foeId.
        If tripId is "", the current tripId of vehID is used.
        """
        self._setCmd(tc.TL_CONSTRAINT_UPDATE, vehID, "s", tripId)
