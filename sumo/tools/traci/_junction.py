# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _junction.py
# @author  Michael Behrisch
# @author  Mario Krumnow
# @author  Jakob Erdmann
# @date    2011-03-17
# @version $Id$

from __future__ import absolute_import
from .domain import Domain
from .storage import Storage
from . import constants as tc

_RETURN_VALUE_FUNC = {tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_SHAPE: Storage.readShape}


class JunctionDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "junction", tc.CMD_GET_JUNCTION_VARIABLE, tc.CMD_SET_JUNCTION_VARIABLE,
                        tc.CMD_SUBSCRIBE_JUNCTION_VARIABLE, tc.RESPONSE_SUBSCRIBE_JUNCTION_VARIABLE,
                        tc.CMD_SUBSCRIBE_JUNCTION_CONTEXT, tc.RESPONSE_SUBSCRIBE_JUNCTION_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getPosition(self, junctionID):
        """getPosition(string) -> (double, double)

        Returns the coordinates of the center of the junction.
        """
        return self._getUniversal(tc.VAR_POSITION, junctionID)

    def getShape(self, junctionID):
        """getShape(string) -> list((double, double))

        List of 2D positions (cartesian) describing the geometry.
        """
        return self._getUniversal(tc.VAR_SHAPE, junctionID)


JunctionDomain()
