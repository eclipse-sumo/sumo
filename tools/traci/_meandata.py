# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _meandata.py
# @author  Melanie Weber
# @date    2020-08-18

from __future__ import absolute_import
from . import constants as tc
from .domain import Domain


class MeanDataDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "meandata", tc.CMD_GET_MEANDATA_VARIABLE, None,
                        tc.CMD_SUBSCRIBE_MEANDATA_VARIABLE, tc.RESPONSE_SUBSCRIBE_MEANDATA_VARIABLE,
                        tc.CMD_SUBSCRIBE_MEANDATA_CONTEXT, tc.RESPONSE_SUBSCRIBE_MEANDATA_CONTEXT)

    def getAttributeValue(self, meanDataID, laneID, attr):
        """getAttributeValue(string, string, string) -> double
        Return the requested attribute for the give laneID as collected by the named laneData or edgeData definition
        If the id belongs to edgeData, then any lane of the edge may be used.
        """
        return self._getUniversal(tc.VAR_MEANDATA_LANE, meanDataID, "tss", 2, laneID, attr)

    def getIDs(self, meanDataID):
        """getAttributeValue(string) -> tuple(string)
        Return the list of edge ids (for edgeData) or lane ids (for laneData) for which data is being collected
        This is the order corresponding to the data returned by getAttributeValues
        """
        return self._getUniversal(tc.VAR_MEANDATA_IDS, meanDataID)

    def getAttributeValues(self, meanDataID, attr):
        """getAttributeValue(string, string) -> tuple(double)
        Return the requested attribute for all edges (edgeData) or all lanes
        (laneData). The order corresponds to the list of ids returned by getIDs
        """
        return self._getUniversal(tc.VAR_MEANDATA_VALUES, meanDataID, "s", attr)
