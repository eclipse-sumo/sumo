# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    connection.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09
# @version $Id$

from __future__ import print_function
from __future__ import absolute_import
import socket
import struct
import sys
import warnings
import abc

from . import constants as tc
from .exceptions import TraCIException, FatalTraCIError
from .domain import _defaultDomains
from .storage import Storage

_RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}


class Connection:

    """Contains the socket, the composed message string
    together with a list of TraCI commands which are inside.
    """

    def __init__(self, host, port, process):
        if sys.platform.startswith('java'):
            # working around jython 2.7.0 bug #2273
            self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        else:
            self._socket = socket.socket()
        self._socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self._socket.connect((host, port))
        self._process = process
        self._string = bytes()
        self._queue = []
        self._subscriptionMapping = {}
        self._stepListeners = {}
        self._nextStepListenerID = 0
        for domain in _defaultDomains:
            domain._register(self, self._subscriptionMapping)

    def _packString(self, s, pre=tc.TYPE_STRING):
        self._string += struct.pack("!Bi", pre, len(s)) + s.encode("latin1")

    def _packStringList(self, l):
        self._string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(l))
        for s in l:
            self._string += struct.pack("!i", len(s)) + s.encode("latin1")

    def _packDoubleList(self, l):
        self._string += struct.pack("!Bi", tc.TYPE_DOUBLELIST, len(l))
        for x in l:
            self._string += struct.pack("!d", x)

    def _recvExact(self):
        try:
            result = bytes()
            while len(result) < 4:
                t = self._socket.recv(4 - len(result))
                if not t:
                    return None
                result += t
            length = struct.unpack("!i", result)[0] - 4
            result = bytes()
            while len(result) < length:
                t = self._socket.recv(length - len(result))
                if not t:
                    return None
                result += t
            return Storage(result)
        except socket.error:
            return None

    def _sendExact(self):
        length = struct.pack("!i", len(self._string) + 4)
        # print("python_sendExact: '%s'" % ' '.join(map(lambda x : "%X" % ord(x), self._string)))
        self._socket.send(length + self._string)
        result = self._recvExact()
        if not result:
            self._socket.close()
            del self._socket
            raise FatalTraCIError("connection closed by SUMO")
        for command in self._queue:
            prefix = result.read("!BBB")
            err = result.readString()
            if prefix[2] or err:
                self._string = bytes()
                self._queue = []
                raise TraCIException(err, prefix[1], _RESULTS[prefix[2]])
            elif prefix[1] != command:
                raise FatalTraCIError("Received answer %s for command %s." % (prefix[1],
                                                                              command))
            elif prefix[1] == tc.CMD_STOP:
                length = result.read("!B")[0] - 1
                result.read("!%sx" % length)
        self._string = bytes()
        self._queue = []
        return result

    def _beginMessage(self, cmdID, varID, objID, length=0):
        self._queue.append(cmdID)
        length += 1 + 1 + 1 + 4 + len(objID)
        if length <= 255:
            self._string += struct.pack("!BB", length, cmdID)
        else:
            self._string += struct.pack("!BiB", 0, length + 4, cmdID)
        self._packString(objID, varID)

    def _sendReadOneStringCmd(self, cmdID, varID, objID):
        self._beginMessage(cmdID, varID, objID)
        return self._checkResult(cmdID, varID, objID)

    def _sendIntCmd(self, cmdID, varID, objID, value):
        self._beginMessage(cmdID, varID, objID, 1 + 4)
        self._string += struct.pack("!Bi", tc.TYPE_INTEGER, value)
        self._sendExact()

    def _sendDoubleCmd(self, cmdID, varID, objID, value):
        self._beginMessage(cmdID, varID, objID, 1 + 8)
        self._string += struct.pack("!Bd", tc.TYPE_DOUBLE, value)
        self._sendExact()

    def _sendByteCmd(self, cmdID, varID, objID, value):
        self._beginMessage(cmdID, varID, objID, 1 + 1)
        self._string += struct.pack("!BB", tc.TYPE_BYTE, value)
        self._sendExact()

    def _sendUByteCmd(self, cmdID, varID, objID, value):
        self._beginMessage(cmdID, varID, objID, 1 + 1)
        self._string += struct.pack("!BB", tc.TYPE_UBYTE, value)
        self._sendExact()

    def _sendStringCmd(self, cmdID, varID, objID, value):
        self._beginMessage(cmdID, varID, objID, 1 + 4 + len(value))
        self._packString(value)
        self._sendExact()

    def _checkResult(self, cmdID, varID, objID):
        result = self._sendExact()
        result.readLength()
        response, retVarID = result.read("!BB")
        objectID = result.readString()
        if response - cmdID != 16 or retVarID != varID or objectID != objID:
            raise FatalTraCIError("Received answer %s,%s,%s for command %s,%s,%s."
                                  % (response, retVarID, objectID, cmdID, varID, objID))
        result.read("!B")     # Return type of the variable
        return result

    def _readSubscription(self, result):
        # to enable this you also need to set _DEBUG to True in storage.py
        # result.printDebug()
        result.readLength()
        response = result.read("!B")[0]
        isVariableSubscription = (response >= tc.RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE and
                                  response <= tc.RESPONSE_SUBSCRIBE_PERSON_VARIABLE)
        objectID = result.readString()
        if not isVariableSubscription:
            domain = result.read("!B")[0]
        numVars = result.read("!B")[0]
        if isVariableSubscription:
            while numVars > 0:
                varID = result.read("!B")[0]
                status, _ = result.read("!BB")
                if status:
                    print("Error!", result.readString())
                elif response in self._subscriptionMapping:
                    self._subscriptionMapping[response].add(objectID, varID, result)
                else:
                    raise FatalTraCIError(
                        "Cannot handle subscription response %02x for %s." % (response, objectID))
                numVars -= 1
        else:
            objectNo = result.read("!i")[0]
            for _ in range(objectNo):
                oid = result.readString()
                if numVars == 0:
                    self._subscriptionMapping[response].addContext(
                        objectID, self._subscriptionMapping[domain], oid)
                for __ in range(numVars):
                    varID = result.read("!B")[0]
                    status, ___ = result.read("!BB")
                    if status:
                        print("Error!", result.readString())
                    elif response in self._subscriptionMapping:
                        self._subscriptionMapping[response].addContext(
                            objectID, self._subscriptionMapping[domain], oid, varID, result)
                    else:
                        raise FatalTraCIError(
                            "Cannot handle subscription response %02x for %s." % (response, objectID))
        return objectID, response

    def _subscribe(self, cmdID, begin, end, objID, varIDs, parameters=None):
        self._queue.append(cmdID)
        length = 1 + 1 + 8 + 8 + 4 + len(objID) + 1 + len(varIDs)
        if parameters:
            for v in varIDs:
                if v in parameters:
                    length += len(parameters[v])
        if length <= 255:
            self._string += struct.pack("!B", length)
        else:
            self._string += struct.pack("!Bi", 0, length + 4)
        self._string += struct.pack("!Bddi",
                                    cmdID, begin, end, len(objID)) + objID.encode("latin1")
        self._string += struct.pack("!B", len(varIDs))
        for v in varIDs:
            self._string += struct.pack("!B", v)
            if parameters and v in parameters:
                self._string += parameters[v]
        result = self._sendExact()
        if varIDs:
            objectID, response = self._readSubscription(result)
            if response - cmdID != 16 or objectID != objID:
                raise FatalTraCIError("Received answer %02x,%s for subscription command %02x,%s." % (
                    response, objectID, cmdID, objID))

    def _getSubscriptionResults(self, cmdID):
        return self._subscriptionMapping[cmdID]

    def _subscribeContext(self, cmdID, begin, end, objID, domain, dist, varIDs):
        self._queue.append(cmdID)
        length = 1 + 1 + 8 + 8 + 4 + len(objID) + 1 + 8 + 1 + len(varIDs)
        if length <= 255:
            self._string += struct.pack("!B", length)
        else:
            self._string += struct.pack("!Bi", 0, length + 4)
        self._string += struct.pack("!Bddi",
                                    cmdID, begin, end, len(objID)) + objID.encode("latin1")
        self._string += struct.pack("!BdB", domain, dist, len(varIDs))
        for v in varIDs:
            self._string += struct.pack("!B", v)
        result = self._sendExact()
        if varIDs:
            objectID, response = self._readSubscription(result)
            if response - cmdID != 16 or objectID != objID:
                raise FatalTraCIError("Received answer %02x,%s for context subscription command %02x,%s." % (
                    response, objectID, cmdID, objID))

    def _addSubscriptionFilter(self, filterType, params=None):
        command = tc.CMD_ADD_SUBSCRIPTION_FILTER
        self._queue.append(command)
        if filterType in (tc.FILTER_TYPE_NONE, tc.FILTER_TYPE_NOOPPOSITE,
                          tc.FILTER_TYPE_TURN, tc.FILTER_TYPE_LEAD_FOLLOW):
            # filter without parameter
            assert(params is None)
            length = 1 + 1 + 1  # length + CMD + FILTER_ID
            self._string += struct.pack("!BBB", length, command, filterType)
        elif filterType in (tc.FILTER_TYPE_DOWNSTREAM_DIST, tc.FILTER_TYPE_UPSTREAM_DIST):
            # filter with float parameter
            assert(type(params) is float)
            length = 1 + 1 + 1 + 1 + 8  # length + CMD + FILTER_ID + floattype + float
            self._string += struct.pack("!BBBBd", length, command, filterType, tc.TYPE_DOUBLE, params)
        elif filterType in (tc.FILTER_TYPE_VCLASS, tc.FILTER_TYPE_VTYPE):
            # filter with list(string) parameter
            length = 1 + 1 + 1 + 1 + 4  # length + CMD + FILTER_ID + TYPE_STRINGLIST + length(stringlist)
            try:
                for s in params:
                    length += 4 + len(s)  # length(s) + s
            except Exception:
                raise TraCIException("Filter type %s requires identifier list as parameter." % filterType)
            if length <= 255:
                self._string += struct.pack("!BBB", length, command, filterType)
            else:
                length += 4  # extended msg length
                self._string += struct.pack("!BiBB", 0, length, command, filterType)
            self._packStringList(params)
        elif filterType == tc.FILTER_TYPE_LANES:
            # filter with list(byte) parameter
            # check uniqueness of given lanes in list
            lanes = set(list(params))
            if len(lanes) < len(list(params)):
                warnings.warn("Ignoring duplicate lane specification for subscription filter.")
            length = 1 + 1 + 1 + 1 + len(lanes)  # length + CMD + FILTER_ID + length(list) as ubyte + lane-indices
            self._string += struct.pack("!BBBB", length, command, filterType, len(lanes))
            for i in lanes:
                if not type(i) is int:
                    raise TraCIException("Filter type lanes requires numeric index list as parameter.")
                if i <= -128 or i >= 128:
                    raise TraCIException("Filter type lanes: maximal lane index is 127.")
                if i < 0:
                    i += 256
                self._string += struct.pack("!B", i)

    def load(self, args):
        """
        Load a simulation from the given arguments.
        """
        self._queue.append(tc.CMD_LOAD)
        self._string += struct.pack("!BiB", 0, 1 + 4 + 1 + 1 + 4 + sum(map(len, args)) + 4 * len(args), tc.CMD_LOAD)
        self._packStringList(args)
        self._sendExact()

    def simulationStep(self, step=0.):
        """
        Make a simulation step and simulate up to the given second in sim time.
        If the given value is 0 or absent, exactly one step is performed.
        Values smaller than or equal to the current sim time result in no action.
        """
        if type(step) is int and step >= 1000:
            warnings.warn("API change now handles step as floating point seconds", stacklevel=2)
        self._queue.append(tc.CMD_SIMSTEP)
        self._string += struct.pack("!BBd", 1 + 1 + 8, tc.CMD_SIMSTEP, step)
        result = self._sendExact()
        for subscriptionResults in self._subscriptionMapping.values():
            subscriptionResults.reset()
        numSubs = result.readInt()
        responses = []
        while numSubs > 0:
            responses.append(self._readSubscription(result))
            numSubs -= 1

        # manage stepListeners
        listenersToRemove = []
        for (listenerID, listener) in self._stepListeners.items():
            keep = listener.step(step)
            if not keep:
                listenersToRemove.append(listenerID)
        for listenerID in listenersToRemove:
            self.removeStepListener(listenerID)

        return responses

    def addStepListener(self, listener):
        """addStepListener(traci.StepListener) -> int

        Append the step listener (its step function is called at the end of every call to traci.simulationStep())
        Returns the ID assigned to the listener if it was added successfully, None otherwise.
        """
        if issubclass(type(listener), StepListener):
            listener.setID(self._nextStepListenerID)
            self._stepListeners[self._nextStepListenerID] = listener
            self._nextStepListenerID += 1
            # print ("traci: Added stepListener %s\nlisteners: %s"%(_nextStepListenerID - 1, _stepListeners))
            return self._nextStepListenerID - 1
        warnings.warn(
            "Proposed listener's type must inherit from traci.StepListener. Not adding object of type '%s'" %
            type(listener))
        return None

    def removeStepListener(self, listenerID):
        """removeStepListener(traci.StepListener) -> bool

        Remove the step listener from traci's step listener container.
        Returns True if the listener was removed successfully, False if it wasn't registered.
        """
        # print ("traci: removeStepListener %s\nlisteners: %s"%(listenerID, _stepListeners))
        if listenerID in self._stepListeners:
            self._stepListeners[listenerID].cleanUp()
            del self._stepListeners[listenerID]
            # print ("traci: Removed stepListener %s"%(listenerID))
            return True
        warnings.warn("Cannot remove unknown listener %s.\nlisteners:%s" % (listenerID, self._stepListeners))
        return False

    def getVersion(self):
        command = tc.CMD_GETVERSION
        self._queue.append(command)
        self._string += struct.pack("!BB", 1 + 1, command)
        result = self._sendExact()
        result.readLength()
        response = result.read("!B")[0]
        if response != command:
            raise FatalTraCIError(
                "Received answer %s for command %s." % (response, command))
        return result.readInt(), result.readString()

    def setOrder(self, order):
        self._queue.append(tc.CMD_SETORDER)
        self._string += struct.pack("!BBi", 1 + 1 + 4, tc.CMD_SETORDER, order)
        self._sendExact()

    def close(self, wait=True):
        for listenerID in list(self._stepListeners.keys()):
            self.removeStepListener(listenerID)
        if hasattr(self, "_socket"):
            self._queue.append(tc.CMD_CLOSE)
            self._string += struct.pack("!BB", 1 + 1, tc.CMD_CLOSE)
            self._sendExact()
            self._socket.close()
            del self._socket
        if wait and self._process is not None:
            self._process.wait()


class StepListener(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def step(self, t=0):
        """step(int) -> bool

        After adding a StepListener 'listener' with traci.addStepListener(listener),
        TraCI will call listener.step(t) after each call to traci.simulationStep(t)
        The return value indicates whether the stepListener wants to stay active.
        """
        return True

    def cleanUp(self):
        """cleanUp() -> None

        This method is called at removal of the stepListener, allowing to schedule some final actions
        """
        pass

    def setID(self, ID):
        self._ID = ID

    def getID(self):
        return self._ID
