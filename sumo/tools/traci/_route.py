# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _route.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @date    2008-10-09
# @version $Id$

from __future__ import absolute_import
from . import constants as tc
from .domain import Domain
from .storage import Storage

_RETURN_VALUE_FUNC = {tc.VAR_EDGES: Storage.readStringList}


class RouteDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "route", tc.CMD_GET_ROUTE_VARIABLE, tc.CMD_SET_ROUTE_VARIABLE,
                        tc.CMD_SUBSCRIBE_ROUTE_VARIABLE, tc.RESPONSE_SUBSCRIBE_ROUTE_VARIABLE,
                        tc.CMD_SUBSCRIBE_ROUTE_CONTEXT, tc.RESPONSE_SUBSCRIBE_ROUTE_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getEdges(self, routeID):
        """getEdges(string) -> list(string)

        Returns a list of all edges in the route.
        """
        return self._getUniversal(tc.VAR_EDGES, routeID)

    def add(self, routeID, edges):
        """add(string, list(string)) -> None

        Adds a new route with the given id consisting of the given list of edge IDs.
        """
        self._connection._beginMessage(tc.CMD_SET_ROUTE_VARIABLE, tc.ADD, routeID,
                                       1 + 4 + sum(map(len, edges)) + 4 * len(edges))
        self._connection._packStringList(edges)
        self._connection._sendExact()


RouteDomain()
