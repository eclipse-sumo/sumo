# -*- coding: utf-8 -*-
"""
@file    route.py
@author  Michael Behrisch
@author  Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
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
