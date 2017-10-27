# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    domain.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09
# @version $Id$

from __future__ import print_function
from __future__ import absolute_import
import copy
import struct
import warnings

import traci
from . import constants as tc
from .storage import Storage
from .exceptions import FatalTraCIError

_defaultDomains = []


class SubscriptionResults:

    def __init__(self, valueFunc):
        self._results = {}
        self._contextResults = {}
        self._valueFunc = valueFunc

    def _parse(self, varID, data):
        if varID not in self._valueFunc:
            raise FatalTraCIError("Unknown variable %02x." % varID)
        return self._valueFunc[varID](data)

    def reset(self):
        self._results.clear()
        self._contextResults.clear()

    def add(self, refID, varID, data):
        if refID not in self._results:
            self._results[refID] = {}
        self._results[refID][varID] = self._parse(varID, data)

    def get(self, refID=None):
        if refID is None:
            return self._results
        return self._results.get(refID, None)

    def addContext(self, refID, domain, objID, varID=None, data=None):
        if refID not in self._contextResults:
            self._contextResults[refID] = {}
        if objID not in self._contextResults[refID]:
            self._contextResults[refID][objID] = {}
        if varID is not None and data is not None:
            self._contextResults[refID][objID][
                varID] = domain._parse(varID, data)

    def getContext(self, refID=None):
        if refID is None:
            return self._contextResults
        return self._contextResults.get(refID, None)

    def __repr__(self):
        return "<%s, %s>" % (self._results, self._contextResults)


class Domain:

    def __init__(self, name, cmdGetID, cmdSetID,
                 subscribeID, subscribeResponseID,
                 contextID, contextResponseID,
                 retValFunc, deprecatedFor=None):
        self._name = name
        self._cmdGetID = cmdGetID
        self._cmdSetID = cmdSetID
        self._subscribeID = subscribeID
        self._subscribeResponseID = subscribeResponseID
        self._contextID = contextID
        self._contextResponseID = contextResponseID
        self._retValFunc = {tc.ID_LIST: Storage.readStringList,
                            tc.ID_COUNT: Storage.readInt}
        self._retValFunc.update(retValFunc)
        self._deprecatedFor = deprecatedFor
        self._connection = None
        _defaultDomains.append(self)
        setattr(traci, name, self)

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

    def _getUniversal(self, varID, objectID=""):
        if self._deprecatedFor:
            warnings.warn("The domain %s is deprecated, use %s instead." % (
                self._name, self._deprecatedFor))  # , DeprecationWarning)
        result = self._connection._sendReadOneStringCmd(
            self._cmdGetID, varID, objectID)
        return self._retValFunc[varID](result)

    def getIDList(self):
        """getIDList() -> list(string)

        Returns a list of all objects in the network.
        """
        return self._getUniversal(tc.ID_LIST, "")

    def getIDCount(self):
        """getIDCount() -> integer

        Returns the number of currently loaded objects.
        """
        return self._getUniversal(tc.ID_COUNT, "")

    def subscribe(self, objectID, varIDs=None, begin=0, end=2**31 - 1):
        """subscribe(string, list(integer), double, double) -> None

        Subscribe to one or more object values for the given interval.
        """
        if varIDs is None:
            if tc.LAST_STEP_VEHICLE_NUMBER in self._retValFunc:
                varIDs = (tc.LAST_STEP_VEHICLE_NUMBER,)
            else:
                varIDs = (tc.ID_LIST,)
        self._connection._subscribe(
            self._subscribeID, begin, end, objectID, varIDs)

    def unsubscribe(self, objectID):
        """unsubscribe(string) -> None

        Unsubscribe from receiving object values.
        """
        self._connection._subscribe(
            self._subscribeID, 0, 2**31 - 1, objectID, [])

    def getSubscriptionResults(self, objectID=None):
        """getSubscriptionResults(string) -> dict(integer: <value_type>)

        Returns the subscription results for the last time step and the given object.
        If no object id is given, all subscription results are returned in a dict.
        If the object id is unknown or the subscription did for any reason return no data,
        'None' is returned.
        It is not possible to retrieve older subscription results than the ones
        from the last time step.
        """
        return self._connection._getSubscriptionResults(self._subscribeResponseID).get(objectID)

    def subscribeContext(self, objectID, domain, dist, varIDs=None, begin=0, end=2**31 - 1):
        """subscribeContext(string, int, double, list(integer), double, double) -> None

        Subscribe to objects of the given domain (specified as domain=traci.constants.CMD_GET_<DOMAIN>_VARIABLE),
        which are closer than dist to the object specified by objectID.
        """
        if varIDs is None:
            if tc.LAST_STEP_VEHICLE_NUMBER in self._retValFunc:
                varIDs = (tc.LAST_STEP_VEHICLE_NUMBER,)
            else:
                varIDs = (tc.ID_LIST,)
        self._connection._subscribeContext(
            self._contextID, begin, end, objectID, domain, dist, varIDs)

    def unsubscribeContext(self, objectID, domain, dist):
        self._connection._subscribeContext(
            self._contextID, 0, 2**31 - 1, objectID, domain, dist, [])

    def getContextSubscriptionResults(self, objectID=None):
        return self._connection._getSubscriptionResults(self._contextResponseID).getContext(objectID)

    def getParameter(self, objID, param):
        """getParameter(string, string) -> string

        Returns the value of the given parameter for the given objID
        """
        self._connection._beginMessage(
            self._cmdGetID, tc.VAR_PARAMETER, objID, 1 + 4 + len(param))
        self._connection._packString(param)
        result = self._connection._checkResult(
            self._cmdGetID, tc.VAR_PARAMETER, objID)
        return result.readString()

    def setParameter(self, objID, param, value):
        """setParameter(string, string, string) -> string

        Sets the value of the given parameter to value for the given objID
        """
        self._connection._beginMessage(self._cmdSetID, tc.VAR_PARAMETER, objID,
                                       1 + 4 + 1 + 4 + len(param) + 1 + 4 + len(value))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._packString(param)
        self._connection._packString(value)
        self._connection._sendExact()
