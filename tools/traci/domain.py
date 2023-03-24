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

# @file    domain.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09

from __future__ import print_function
from __future__ import absolute_import
import copy
import warnings

from . import constants as tc
from .exceptions import FatalTraCIError

DOMAINS = []


def _readParameterWithKey(result):
    assert result.read("!i")[0] == 2  # compound size
    key = result.readTypedString()
    val = result.readTypedString()
    return key, val


def _parse(valueFunc, varID, data):
    varType = data.read("!B")[0]
    if varID in valueFunc:
        return valueFunc[varID](data)
    if varType in (tc.POSITION_2D, tc.POSITION_LON_LAT):
        return data.read("!dd")
    if varType in (tc.POSITION_3D, tc.POSITION_LON_LAT_ALT):
        return data.read("!ddd")
    if varType == tc.TYPE_POLYGON:
        return data.readShape()
    if varType == tc.TYPE_UBYTE:
        return data.read("!B")[0]
    if varType == tc.TYPE_BYTE:
        return data.read("!b")[0]
    if varType == tc.TYPE_INTEGER:
        return data.readInt()
    if varType == tc.TYPE_DOUBLE:
        return data.readDouble()
    if varType == tc.TYPE_STRING:
        return data.readString()
    if varType == tc.TYPE_STRINGLIST:
        return data.readStringList()
    if varType == tc.TYPE_DOUBLELIST:
        n = data.read("!i")[0]
        return tuple([data.readDouble() for i in range(n)])
    if varType == tc.TYPE_COLOR:
        return data.read("!BBBB")
    raise FatalTraCIError("Unknown variable %02x or invalid type %02x." % (varID, varType))


class SubscriptionResults:

    def __init__(self, valueFunc):
        self._results = {}
        self._contextResults = {}
        self._valueFunc = valueFunc

    def reset(self):
        self._results.clear()
        self._contextResults.clear()

    def add(self, refID, varID, data):
        if refID not in self._results:
            self._results[refID] = {}
        self._results[refID][varID] = _parse(self._valueFunc, varID, data)

    def get(self, refID=None):
        if refID is None:
            return self._results
        return self._results.get(refID, {})

    def addContext(self, refID, domain, objID, varID=None, data=None):
        if refID not in self._contextResults:
            self._contextResults[refID] = {}
        if objID not in self._contextResults[refID]:
            self._contextResults[refID][objID] = {}
        if varID is not None and data is not None:
            self._contextResults[refID][objID][varID] = _parse(self._valueFunc, varID, data)

    def getContext(self, refID=None):
        if refID is None:
            return self._contextResults
        return self._contextResults.get(refID, {})

    def __repr__(self):
        return "<%s, %s>" % (self._results, self._contextResults)


class Domain:

    def __init__(self, name, cmdGetID, cmdSetID,
                 subscribeID, subscribeResponseID,
                 contextID, contextResponseID,
                 retValFunc=None, deprecatedFor=None,
                 subscriptionDefault=(tc.TRACI_ID_LIST,)):
        self._name = name
        self._cmdGetID = cmdGetID
        self._cmdSetID = cmdSetID
        self._subscribeID = subscribeID
        self._subscribeResponseID = subscribeResponseID
        self._contextID = contextID
        self._contextResponseID = contextResponseID
        self._retValFunc = {tc.VAR_PARAMETER_WITH_KEY: _readParameterWithKey}
        if retValFunc is not None:
            self._retValFunc.update(retValFunc)
        self._deprecatedFor = deprecatedFor
        self._subscriptionDefault = subscriptionDefault
        self._connection = None
        DOMAINS.append(self)
        # alias
        self.DOMAIN_ID = cmdGetID

    def _register(self, connection, mapping):
        dom = copy.copy(self)
        dom._connection = connection
        subscriptionResults = SubscriptionResults(self._retValFunc)
        mapping[self._subscribeResponseID] = subscriptionResults
        mapping[self._contextResponseID] = subscriptionResults
        mapping[self._cmdGetID] = subscriptionResults
        setattr(connection, self._name, dom)

    def _setConnection(self, connection):
        self._connection = connection

    def _getUniversal(self, varID, objectID="", format="", *values):
        if self._deprecatedFor:
            warnings.warn("The domain %s is deprecated, use %s instead." % (self._name, self._deprecatedFor))
        return _parse(self._retValFunc, varID, self._getCmd(varID, objectID, format, *values))

    def _getCmd(self, varID, objID, format="", *values):
        if self._connection is None:
            raise FatalTraCIError("Not connected.")
        r = self._connection._sendCmd(self._cmdGetID, varID, objID, format, *values)
        r.readLength()
        response, retVarID = r.read("!BB")
        objectID = r.readString()
        if response - self._cmdGetID != 16 or retVarID != varID or objectID != objID:
            raise FatalTraCIError("Received answer %s,%s,%s for command %s,%s,%s."
                                  % (response, retVarID, objectID, self._cmdGetID, varID, objID))
        return r

    def _setCmd(self, varID, objectID, format="", *values):
        if self._connection is None:
            raise FatalTraCIError("Not connected.")
        self._connection._sendCmd(self._cmdSetID, varID, objectID, format, *values)

    def getIDList(self):
        """getIDList() -> list(string)

        Returns a list of all objects in the network.
        """
        return self._getUniversal(tc.TRACI_ID_LIST, "")

    def getIDCount(self):
        """getIDCount() -> integer

        Returns the number of currently loaded objects.
        """
        return self._getUniversal(tc.ID_COUNT, "")

    def subscribe(self, objectID, varIDs=None, begin=tc.INVALID_DOUBLE_VALUE, end=tc.INVALID_DOUBLE_VALUE,
                  parameters=None):
        """subscribe(string, list(integer), double, double, map(string->tuple)) -> None

        Subscribe to one or more object values for the given interval.
        """
        if varIDs is None:
            varIDs = self._subscriptionDefault
        self._connection._subscribe(self._subscribeID, begin, end, objectID, varIDs, parameters)

    def unsubscribe(self, objectID):
        """unsubscribe(string) -> None

        Unsubscribe from receiving object values.
        """
        self._connection._subscribe(self._subscribeID, tc.INVALID_DOUBLE_VALUE, tc.INVALID_DOUBLE_VALUE,
                                    objectID, [], None)

    def getSubscriptionResults(self, objectID):
        """getSubscriptionResults(string) -> dict(integer: <value_type>)

        Returns the subscription results for the last time step and the given object.
        If the object id is unknown or the subscription did for any reason return no data,
        'None' is returned.
        It is not possible to retrieve older subscription results than the ones
        from the last time step.
        """
        return self._connection._getSubscriptionResults(self._subscribeResponseID).get(objectID)

    def getAllSubscriptionResults(self):
        """getAllSubscriptionResults() -> dict(string: dict(integer: <value_type>))

        Returns the subscription results for the last time step and all objects of the domain.
        It is not possible to retrieve older subscription results than the ones
        from the last time step.
        """
        return self._connection._getSubscriptionResults(self._subscribeResponseID).get(None)

    def subscribeContext(self, objectID, domain, dist, varIDs=None,
                         begin=tc.INVALID_DOUBLE_VALUE, end=tc.INVALID_DOUBLE_VALUE, parameters=None):
        """subscribeContext(string, int, double, list(integer), double, double) -> None

        Subscribe to objects of the given domain (specified as domain=traci.constants.CMD_GET_<DOMAIN>_VARIABLE),
        which are closer than dist to the object specified by objectID.
        """
        if varIDs is None:
            varIDs = self._subscriptionDefault
        self._connection._subscribeContext(self._contextID, begin, end, objectID, domain, dist, varIDs, parameters)

    def unsubscribeContext(self, objectID, domain, dist):
        self._connection._subscribeContext(self._contextID, tc.INVALID_DOUBLE_VALUE, tc.INVALID_DOUBLE_VALUE,
                                           objectID, domain, dist, [], None)

    def getContextSubscriptionResults(self, objectID):
        return self._connection._getSubscriptionResults(self._contextResponseID).getContext(objectID)

    def getAllContextSubscriptionResults(self):
        return self._connection._getSubscriptionResults(self._contextResponseID).getContext(None)

    def getParameter(self, objID, param):
        """getParameter(string, string) -> string

        Returns the value of the given parameter for the given objID
        """
        return self._getUniversal(tc.VAR_PARAMETER, objID, "s", param)

    def getParameterWithKey(self, objID, param):
        """getParameterWithKey(string, string) -> (string, string)

        Returns the (key, value) tuple of the given parameter for the given objID
        """
        return self._getUniversal(tc.VAR_PARAMETER_WITH_KEY, objID, "s", param)

    def subscribeParameterWithKey(self, objID, key, begin=tc.INVALID_DOUBLE_VALUE, end=tc.INVALID_DOUBLE_VALUE):
        """subscribeParameterWithKey(string, string) -> None

        Subscribe for a generic parameter with the given key.
        """
        self.subscribe(objID, (tc.VAR_PARAMETER_WITH_KEY,), begin, end, {tc.VAR_PARAMETER_WITH_KEY: ("s", key)})

    def setParameter(self, objID, param, value):
        """setParameter(string, string, string) -> None

        Sets the value of the given parameter to value for the given objID
        """
        self._setCmd(tc.VAR_PARAMETER, objID, "tss", 2, param, value)
