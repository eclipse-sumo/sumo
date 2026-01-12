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

# @file    modes.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..constants import DELAY_RECOMPUTE
from ..input.keyboard import typeKey, typeTwoKeys


def changeSupermode(supermode):
    """
    @brief change supermode
    """
    match supermode:
        # common modes
        case "network":
            typeKey('F2')
        case "demand":
            typeKey('F3')
        case "data":
            typeKey('F4')
        case _:
            raise Exception("Invalid supermode")
    # wait for recompute
    time.sleep(DELAY_RECOMPUTE)


def changeMode(mode):
    """
    @brief change edit mode
    """
    match mode:
        # common modes
        case "inspect":
            typeKey('i')
        case "delete":
            typeKey('d')
        case "select":
            typeKey('s')
        # network modes
        case "createEdge":
            typeKey('e')
        case "move":
            typeKey('m')
        case "crossing":
            typeKey('r')
        case "connection":
            typeKey('c')
        case "additional":
            typeKey('a')
        case "shape":
            typeKey('p')
        case "TLS":
            typeKey('t')
        case "TAZ":
            typeKey('z')
        # demand modes
        case "route":
            typeKey('r')
        case "vehicle":
            typeKey('v')
        case "type":
            typeKey('t')
        case "person":
            typeKey('p')
        case "personPlan":
            typeKey('l')
        case "container":
            typeKey('c')
        case "containerPlan":
            typeKey('h')
        case "stop":
            typeKey('a')
        # data modes
        case "edgeData":
            typeKey('e')
        case "edgeRelData":
            typeKey('r')
        case "TAZRelData":
            typeKey('z')
        case "meanData":
            typeKey('m')
        case _:
            raise Exception("Invalid mode")
    # wait 1 second
    time.sleep(1)


def changeEditMode(key):
    """
    @brief Change edit mode (alt+1-9)
    """
    typeTwoKeys('alt', key)
