# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    fixElements.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..constants import DELAY_QUESTION
from ..files.save import saveExistentShortcut
from ..input.keyboard import typeKey, typeTwoKeys


def fixRoute(solution):
    """
    @brief fix route
    """
    # save config
    saveExistentShortcut("neteditConfig")
    # select bullet depending of solution
    if (solution == "selectRouteInvalids"):
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        for _ in range(2):
            typeKey('tab')
        typeKey('space')
    elif (solution == "saveRouteInvalids"):
        for _ in range(3):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        for _ in range(3):
            typeKey('tab')
        typeKey('space')
    elif (solution == "removeRouteInvalids"):
        for _ in range(4):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        for _ in range(4):
            typeKey('tab')
        typeKey('space')
    else:
        # press cancel
        typeKey('tab')
        typeKey('space')


def fixDemandElements(solution):
    """
    @brief fix stoppingPlaces
    """
    # save config
    saveExistentShortcut("neteditConfig")
    # select bullet depending of solution
    if (solution == "saveInvalids"):
        for _ in range(3):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        for _ in range(3):
            typeKey('tab')
        typeKey('space')
    elif (solution == "fixPositions"):
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        for _ in range(2):
            typeKey('tab')
        typeKey('space')
    elif (solution == "selectInvalids"):
        typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        typeKey('tab')
        typeKey('space')
    elif (solution == "activateFriendlyPos"):
        # default option, then press accept
        typeKey('space')
    else:
        # press cancel
        typeKey('tab')
        typeKey('space')


def fixStoppingPlace(solution):
    """
    @brief fix stoppingPlaces
    """
    # save config
    saveExistentShortcut("neteditConfig")
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
    # select bullet depending of solution
    if (solution == "savePositionInvalids"):
        for _ in range(3):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
    # go back and press accept
        for _ in range(3):
            typeKey('tab')
        typeKey('space')
    elif (solution == "fixPositions"):
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
    # go back and press accept
        for _ in range(2):
            typeKey('tab')
        typeKey('space')
    elif (solution == "selectPositionInvalids"):
        typeTwoKeys('shift', 'tab')
        typeKey('space')
    # go back and press accept
        typeKey('tab')
        typeKey('space')
    elif (solution == "activatePositionFriendlyPos"):
        # default option, then press accept
        typeKey('space')
    else:
        # press cancel
        typeKey('tab')
        typeKey('space')


def fixCrossings(solution):
    """
    @brief fix stoppingPlaces
    """
    # save config
    saveExistentShortcut("neteditConfig")
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
    # select bullet depending of solution
    if (solution == "saveInvalidCrossings"):
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
    # go back and press accept
        for _ in range(2):
            typeKey('tab')
        typeKey('space')
    elif (solution == "selectInvalidCrossings"):
        typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        typeKey('tab')
        typeKey('space')
    elif (solution == "removeInvalidCrossings"):
        # default option, then press accept
        typeKey('space')
    else:
        # press cancel
        typeKey('tab')
        typeKey('space')
