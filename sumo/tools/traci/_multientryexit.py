# -*- coding: utf-8 -*-
"""
@file    multientryexit.py
@author  Michael Behrisch
@date    2011-03-16
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from .domain import Domain
from .storage import Storage
from . import constants as tc

_RETURN_VALUE_FUNC = {tc.LAST_STEP_VEHICLE_NUMBER:         Storage.readInt,
                      tc.LAST_STEP_MEAN_SPEED:             Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_ID_LIST:        Storage.readStringList,
                      tc.LAST_STEP_VEHICLE_HALTING_NUMBER: Storage.readInt}


class MultiEntryExitDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "multientryexit", tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, None,
                        tc.CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, tc.RESPONSE_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE,
                        tc.CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_CONTEXT, tc.RESPONSE_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_CONTEXT,
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
