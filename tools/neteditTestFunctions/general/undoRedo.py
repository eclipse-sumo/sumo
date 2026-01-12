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

# @file    undoRedo.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..enums.viewPositions import positions
from ..constants import DELAY_UNDOREDO
from ..general.functions import focusOnFrame
from ..input.keyboard import typeKey, typeTwoKeys
from ..input.mouse import leftClickOffset


def undo(referencePosition, number, offsetX=0, offsetY=0):
    """
    @brief undo last operation
    """
    # first wait
    time.sleep(DELAY_UNDOREDO)
    # focus current frame
    focusOnFrame()
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey('i')
    # click over referencePosition
    leftClickOffset(referencePosition, positions.reference, offsetX, offsetY)
    for _ in range(number):
        typeTwoKeys('ctrl', 'z')
        time.sleep(DELAY_UNDOREDO)


def redo(referencePosition, number, offsetX=0, offsetY=0):
    """
    @brief undo last operation
    """
    # first wait
    time.sleep(DELAY_UNDOREDO)
    # focus current frame
    focusOnFrame()
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey('i')
    # click over referencePosition
    leftClickOffset(referencePosition, positions.reference, offsetX, offsetY)
    for _ in range(number):
        typeTwoKeys('ctrl', 'y')
        time.sleep(DELAY_UNDOREDO)


def checkUndoRedo(referencePosition, offsetX=0, offsetY=0):
    """
    @brief Check undo-redo
    """
    # Check undo
    undo(referencePosition, 9, offsetX)
    # Check redo
    redo(referencePosition, 9, offsetY)
