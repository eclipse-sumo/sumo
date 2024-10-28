# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _multientryexit.py
# @author  Michael Behrisch
# @date    2011-03-16

from __future__ import absolute_import
from .domain import Domain
from . import constants as tc


class MultiEntryExitDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "multientryexit",
                        tc.CMD_GET_MULTIENTRYEXIT_VARIABLE, tc.CMD_SET_MULTIENTRYEXIT_VARIABLE,
                        tc.CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE, tc.RESPONSE_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE,
                        tc.CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT, tc.RESPONSE_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT,
                        subscriptionDefault=(tc.LAST_STEP_VEHICLE_NUMBER,))

    def getEntryLanes(self, detID):
        """getEntryLanes(string) -> list(string)

        Returns the list of ids of the detector's entry lanes.
        """
        return self._getUniversal(tc.VAR_LANES, detID)

    def getExitLanes(self, detID):
        """getExitLanes(string) -> list(string)

        Returns the list of ids of the detector's exit lanes.
        """
        return self._getUniversal(tc.VAR_EXIT_LANES, detID)

    def getEntryPositions(self, detID):
        """getEntryPositions(string) -> list(double)

        Returns the list of positions of the detector's entry lanes.
        """
        return self._getUniversal(tc.VAR_POSITION, detID)

    def getExitPositions(self, detID):
        """getExitPositions(string) -> list(double)

        Returns the list of positions of the detector's exit lanes.
        """
        return self._getUniversal(tc.VAR_EXIT_POSITIONS, detID)

    def getLastStepVehicleNumber(self, detID):
        """getLastStepVehicleNumber(string) -> integer

        Returns the number of vehicles that have been within the named multi-entry/multi-exit detector within the
        last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, detID)

    def getLastStepMeanSpeed(self, detID):
        """getLastStepMeanSpeed(string) -> double

        Returns the mean speed in m/s of vehicles that have been within the named multi-entry/multi-exit detector
        within the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_MEAN_SPEED, detID)

    def getLastStepVehicleIDs(self, detID):
        """getLastStepVehicleIDs(string) -> list(string)

        Returns the list of ids of vehicles that have been within the named multi-entry/multi-exit detector in the
        last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, detID)

    def getLastStepHaltingNumber(self, detID):
        """getLastStepHaltingNumber(string) -> integer

        Returns the number of vehicles which were halting during the last time step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, detID)

    def getLastIntervalMeanTravelTime(self, detID):
        """getLastIntervalMeanTravelTime(string) -> double

        Returns the average travel time of vehicles that passed the detector in
        the previous completed measurement interval
        """
        return self._getUniversal(tc.VAR_LAST_INTERVAL_TRAVELTIME, detID)

    def getLastIntervalMeanHaltsPerVehicle(self, detID):
        """getLastIntervalMeanHaltsPerVehicle(string) -> double

        Returns the average number of halts of vehicles that passed the detector in
        the previous completed measurement interval
        """
        return self._getUniversal(tc.VAR_LAST_INTERVAL_MEAN_HALTING_NUMBER, detID)

    def getLastIntervalMeanTimeLoss(self, detID):
        """getLastIntervalMeanTimeLoss(string) -> double

        Returns the average time loss of vehicles that passed the detector in
        the previous completed measurement interval
        """
        return self._getUniversal(tc.VAR_TIMELOSS, detID)

    def getLastIntervalVehicleSum(self, detID):
        """getLastIntervalVehicleSum(string) -> integer

        Returns the number of vehicles that passed the detector in
        the previous completed measurement interval
        """
        return self._getUniversal(tc.VAR_LAST_INTERVAL_VEHICLE_NUMBER, detID)
