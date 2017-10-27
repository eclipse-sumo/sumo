# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _multientryexit.py
# @author  Michael Behrisch
# @date    2011-03-16
# @version $Id$

from __future__ import absolute_import
from .domain import Domain
from .storage import Storage
from . import constants as tc

_RETURN_VALUE_FUNC = {tc.LAST_STEP_VEHICLE_NUMBER: Storage.readInt,
                      tc.LAST_STEP_MEAN_SPEED: Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_ID_LIST: Storage.readStringList,
                      tc.LAST_STEP_VEHICLE_HALTING_NUMBER: Storage.readInt}


class MultiEntryExitDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "multientryexit", tc.CMD_GET_MULTIENTRYEXIT_VARIABLE, None,
                        tc.CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE, tc.RESPONSE_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE,
                        tc.CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT, tc.RESPONSE_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getLastStepVehicleNumber(self, detID):
        """getLastStepVehicleNumber(string) -> integer

        Returns the number of vehicles that have been within the named multi-entry/multi-exit detector within the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, detID)

    def getLastStepMeanSpeed(self, detID):
        """getLastStepMeanSpeed(string) -> double

        Returns the mean speed in m/s of vehicles that have been within the named multi-entry/multi-exit detector within the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_MEAN_SPEED, detID)

    def getLastStepVehicleIDs(self, detID):
        """getLastStepVehicleIDs(string) -> list(string)

        Returns the list of ids of vehicles that have been within the named multi-entry/multi-exit detector in the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, detID)

    def getLastStepHaltingNumber(self, detID):
        """getLastStepHaltingNumber(string) -> integer

        Returns the number of vehicles which were halting during the last time step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, detID)


MultiEntryExitDomain()
