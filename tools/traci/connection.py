# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    connection.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09

from __future__ import print_function
from __future__ import absolute_import
import socket
import struct
import sys
import warnings

from . import constants as tc
from .exceptions import TraCIException, FatalTraCIError
from .domain import DOMAINS
from .storage import Storage
from .step import StepManager

_DEBUG = False
_RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}

_connections = {}
_connectHook = None


def check():
    if "" not in _connections:
        raise FatalTraCIError("Not connected.")
    return _connections[""]


def has(label):
    return label in _connections


def get(label="default"):
    if label not in _connections:
        raise TraCIException("Connection '%s' is not known." % label)
    return _connections[label]


def switch(label):
    con = get(label)
    _connections[""] = con
    for domain in DOMAINS:
        domain._setConnection(con)


class Connection(StepManager):

    """Contains the socket, the composed message string
    together with a list of TraCI commands which are inside.
    """

    def __init__(self, host, port, process, traceFile, traceGetters, label=None):
        StepManager.__init__(self)
        if label in _connections:
            raise TraCIException("Connection '%s' is already active." % label)
        if sys.platform.startswith('java'):
            # working around jython 2.7.0 bug #2273
            self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        else:
            self._socket = socket.socket()
        self._socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        try:
            self._socket.connect((host, port))
        except socket.error:
            self._socket.close()
            raise
        self._process = process
        self._string = bytes()
        self._queue = []
        self._subscriptionMapping = {}
        if traceFile is not None:
            self.startTracing(traceFile, traceGetters, DOMAINS)
        for domain in DOMAINS:
            domain._register(self, self._subscriptionMapping)
        self._label = label
        if _connectHook is not None:
            _connectHook(self)
        if label is not None:
            _connections[label] = self

    def getLabel(self):
        return self._label

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
        if self._socket is None:
            raise FatalTraCIError("Connection already closed.")
        length = struct.pack("!i", len(self._string) + 4)
        if _DEBUG:
            print("sending", Storage(length + self._string).getDebugString())
        self._socket.send(length + self._string)
        result = self._recvExact()
        if _DEBUG:
            print("receiving", result.getDebugString())
        if not result:
            self._socket.close()
            self._socket = None
            raise FatalTraCIError("connection closed by SUMO")
        for command in self._queue:
            prefix = result.read("!BBB")
            err = result.readString()
            if prefix[2] or err:
                self._string = bytes()
                self._queue = []
                raise TraCIException(err, prefix[1], _RESULTS[prefix[2]])
            elif prefix[1] != command:
                raise FatalTraCIError("Received answer %s for command %s." % (prefix[1], command))
            elif prefix[1] == tc.CMD_STOP:
                length = result.read("!B")[0] - 1
                result.read("!%sx" % length)
        self._string = bytes()
        self._queue = []
        return result

    def _pack(self, format, *values):
        packed = bytes()
        for f, v in zip(format, values):
            if f == "i":
                packed += struct.pack("!Bi", tc.TYPE_INTEGER, int(v))
            elif f == "I":  # raw int for setOrder
                packed += struct.pack("!i", int(v))
            elif f == "d":
                packed += struct.pack("!Bd", tc.TYPE_DOUBLE, float(v))
            elif f == "D":  # raw double for some base commands like simstep
                packed += struct.pack("!d", float(v))
            elif f == "b":
                packed += struct.pack("!Bb", tc.TYPE_BYTE, int(v))
            elif f == "B":
                packed += struct.pack("!BB", tc.TYPE_UBYTE, int(v))
            elif f == "u":  # raw unsigned byte needed for distance command and subscribe
                packed += struct.pack("!B", int(v))
            elif f == "s":
                v = str(v)
                packed += struct.pack("!Bi", tc.TYPE_STRING, len(v)) + v.encode("latin1")
            elif f == "p":  # polygon
                if len(v) <= 255:
                    packed += struct.pack("!BB", tc.TYPE_POLYGON, len(v))
                else:
                    packed += struct.pack("!BBi", tc.TYPE_POLYGON, 0, len(v))
                for p in v:
                    packed += struct.pack("!dd", *p)
            elif f == "t":  # tuple aka compound
                packed += struct.pack("!Bi", tc.TYPE_COMPOUND, v)
            elif f == "c":  # color
                packed += struct.pack("!BBBBB", tc.TYPE_COLOR, int(v[0]), int(v[1]), int(v[2]),
                                      int(v[3]) if len(v) > 3 else 255)
            elif f == "l":  # string list
                packed += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(v))
                for s in v:
                    packed += struct.pack("!i", len(s)) + s.encode("latin1")
            elif f == "f":  # float list
                packed += struct.pack("!Bi", tc.TYPE_DOUBLELIST, len(v))
                for x in v:
                    packed += struct.pack("!d", x)
            elif f == "o":
                packed += struct.pack("!Bdd", tc.POSITION_2D, *v)
            elif f == "O":
                packed += struct.pack("!Bddd", tc.POSITION_3D, *v)
            elif f == "g":
                packed += struct.pack("!Bdd", tc.POSITION_LON_LAT, *v)
            elif f == "G":
                packed += struct.pack("!Bddd", tc.POSITION_LON_LAT_ALT, *v)
            elif f == "r":
                packed += struct.pack("!Bi", tc.POSITION_ROADMAP, len(v[0])) + v[0].encode("latin1")
                packed += struct.pack("!dB", v[1], v[2])
        return packed

    def _sendCmd(self, cmdID, varID, objID, format="", *values):
        self._queue.append(cmdID)
        packed = self._pack(format, *values)
        length = len(packed) + 1 + 1  # length and command
        if varID is not None:
            if isinstance(varID, tuple):  # begin and end of a subscription
                length += 8 + 8 + 4 + len(objID)
            else:
                length += 1 + 4 + len(objID)
        if length <= 255:
            self._string += struct.pack("!BB", length, cmdID)
        else:
            self._string += struct.pack("!BiB", 0, length + 4, cmdID)
        if varID is not None:
            if isinstance(varID, tuple):
                self._string += struct.pack("!dd", *varID)
            else:
                self._string += struct.pack("!B", varID)
            self._string += struct.pack("!i", len(objID)) + objID.encode("latin1")
        self._string += packed
        return self._sendExact()

    def _readSubscription(self, result):
        if _DEBUG:
            print("reading subscription", result.getDebugString())
        result.readLength()
        response = result.read("!B")[0]
        isVariableSubscription = ((response >= tc.RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE and
                                   response <= tc.RESPONSE_SUBSCRIBE_BUSSTOP_VARIABLE) or
                                  (response >= tc.RESPONSE_SUBSCRIBE_PARKINGAREA_VARIABLE and
                                   response <= tc.RESPONSE_SUBSCRIBE_OVERHEADWIRE_VARIABLE))
        objectID = result.readString()
        if not isVariableSubscription:
            domain = result.read("!B")[0]
        numVars = result.read("!B")[0]
        if isVariableSubscription:
            while numVars > 0:
                varID, status = result.read("!BB")
                if status:
                    print("Error!", result.readTypedString())
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
                    varID, status = result.read("!BB")
                    if status:
                        print("Error!", result.readTypedString())
                    elif response in self._subscriptionMapping:
                        self._subscriptionMapping[response].addContext(
                            objectID, self._subscriptionMapping[domain], oid, varID, result)
                    else:
                        raise FatalTraCIError(
                            "Cannot handle subscription response %02x for %s." % (response, objectID))
        return objectID, response

    def _subscribe(self, cmdID, begin, end, objID, varIDs, parameters):
        format = "u"
        args = [len(varIDs)]
        for v in varIDs:
            format += "u"
            args.append(v)
            if parameters is not None and v in parameters:
                if isinstance(parameters[v], tuple):
                    format += parameters[v][0]
                    for a in parameters[v][1:]:
                        args.append(a)
                elif isinstance(parameters[v], int):
                    format += "i"
                    args.append(parameters[v])
                elif isinstance(parameters[v], float):
                    format += "d"
                    args.append(parameters[v])
                else:
                    format += "s"
                    args.append(parameters[v])
        result = self._sendCmd(cmdID, (begin, end), objID, format, *args)
        if varIDs:
            objectID, response = self._readSubscription(result)
            if response - cmdID != 16 or objectID != objID:
                raise FatalTraCIError("Received answer %02x,%s for subscription command %02x,%s." % (
                    response, objectID, cmdID, objID))

    def _getSubscriptionResults(self, cmdID):
        return self._subscriptionMapping[cmdID]

    def _subscribeContext(self, cmdID, begin, end, objID, domain, dist, varIDs, parameters=None):
        result = self._sendCmd(cmdID, (begin, end), objID, "uDu" + (len(varIDs) * "u"),
                               domain, dist, len(varIDs), *varIDs)
        if varIDs:
            objectID, response = self._readSubscription(result)
            if response - cmdID != 16 or objectID != objID:
                raise FatalTraCIError("Received answer %02x,%s for context subscription command %02x,%s." % (
                    response, objectID, cmdID, objID))

    def _addSubscriptionFilter(self, filterType, params=None):
        if filterType in (tc.FILTER_TYPE_NONE, tc.FILTER_TYPE_NOOPPOSITE,
                          tc.FILTER_TYPE_LEAD_FOLLOW):
            # filter without parameter
            assert params is None
            self._sendCmd(tc.CMD_ADD_SUBSCRIPTION_FILTER, None, None, "u", filterType)
        elif filterType in (tc.FILTER_TYPE_DOWNSTREAM_DIST, tc.FILTER_TYPE_UPSTREAM_DIST,
                            tc.FILTER_TYPE_TURN, tc.FILTER_TYPE_FIELD_OF_VISION,
                            tc.FILTER_TYPE_LATERAL_DIST):
            # filter with float parameter
            self._sendCmd(tc.CMD_ADD_SUBSCRIPTION_FILTER, None, None, "ud", filterType, params)
        elif filterType in (tc.FILTER_TYPE_VCLASS, tc.FILTER_TYPE_VTYPE):
            # filter with list(string) parameter
            self._sendCmd(tc.CMD_ADD_SUBSCRIPTION_FILTER, None, None, "ul", filterType, params)
        elif filterType == tc.FILTER_TYPE_LANES:
            # filter with list(byte) parameter
            # check uniqueness of given lanes in list
            lanes = set()
            for i in params:
                lane = int(i)
                if lane < 0:
                    lane += 256
                lanes.add(lane)
            if len(lanes) < len(list(params)):
                warnings.warn("Ignoring duplicate lane specification for subscription filter.")
            self._sendCmd(tc.CMD_ADD_SUBSCRIPTION_FILTER, None, None,
                          (len(lanes) + 2) * "u", filterType, len(lanes), *lanes)

    def hasGUI(self):
        try:
            self.gui.getIDList()
            return True
        except TraCIException:
            return False

    def load(self, args):
        """
        Load a simulation from the given arguments.
        """
        if self._traceFile:
            self._traceFile.write("traci.load(%s)\n" % repr(args))
        self._sendCmd(tc.CMD_LOAD, None, None, "l", args)

    def simulationStep(self, step=0.):
        """
        Make a simulation step and simulate up to the given second in sim time.
        If the given value is 0 or absent, exactly one step is performed.
        Values smaller than or equal to the current sim time result in no action.
        """
        if self._traceFile:
            args = "" if step == 0 else str(step)
            self._traceFile.write("traci.simulationStep(%s)\n" % args)
        if type(step) is int and step >= 1000:
            warnings.warn("API change now handles step as floating point seconds", stacklevel=2)
        result = self._sendCmd(tc.CMD_SIMSTEP, None, None, "D", step)
        for subscriptionResults in self._subscriptionMapping.values():
            subscriptionResults.reset()
        numSubs = result.readInt()
        responses = []
        while numSubs > 0:
            responses.append(self._readSubscription(result))
            numSubs -= 1
        self.manageStepListeners(step)
        return responses

    def getVersion(self):
        command = tc.CMD_GETVERSION
        result = self._sendCmd(command, None, None)
        result.readLength()
        response = result.read("!B")[0]
        if response != command:
            raise FatalTraCIError("Received answer %s for command %s." % (response, command))
        return result.readInt(), result.readString()

    def setOrder(self, order):
        self._sendCmd(tc.CMD_SETORDER, None, None, "I", order)

    def close(self, wait=True):
        StepManager.close(self, True)
        for listenerID in list(self._stepListeners.keys()):
            self.removeStepListener(listenerID)
        if self._socket is not None:
            self._sendCmd(tc.CMD_CLOSE, None, None)
            self._socket.close()
            self._socket = None
        if wait and self._process is not None:
            self._process.wait()
        self.simulation._setConnection(None)
        if self._label is not None:
            if _connections[""] == self:
                del _connections[""]
            del _connections[self._label]
