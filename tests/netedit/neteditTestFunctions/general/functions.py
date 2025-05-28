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

# @file    functions.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ..imports import *
from ..input.keyboard import *
from ..input.mouse import *
from ..constants import *
from .modes import *

#################################################
    # processing functions
#################################################


def computeJunctions():
    """
    @brief compute junctions (rebuild network)
    """
    typeKey('F5')
    # wait for output
    time.sleep(DELAY_RECOMPUTE)


def computeJunctionsVolatileOptions(question=True):
    """
    @brief rebuild network with volatile options
    """
    typeTwoKeys('shift', 'F5')
    # confirm recompute
    if question is True:
        waitQuestion('y')
    # wait for output
        time.sleep(DELAY_RECOMPUTE_VOLATILE)
    else:
        waitQuestion('n')


def joinSelectedJunctions():
    """
    @brief join selected junctions
    """
    typeKey('F7')


def focusOnFrame():
    """
    @brief select focus on upper element of current frame
    """
    typeTwoKeys('shift', 'F12')
    time.sleep(1)


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
    leftClick(referencePosition, positions.reference, offsetX, offsetY)
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
    leftClick(referencePosition, positions.reference, offsetX, offsetY)
    for _ in range(number):
        typeTwoKeys('ctrl', 'y')
        time.sleep(DELAY_UNDOREDO)


def waitQuestion(answer):
    """
    @brief wait question of Netedit and select a yes/no answer (by default yes)
    """
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
    if (answer == 'n'):
        typeKey('tab')
    typeKey('space')


def overwritte(value):
    """
    @brief check if overwritte loaded elements
    """
    if value == "yes":
        typeKey('space')
    elif value == "no":
        typeKey('tab')
        typeKey('space')
    else:
        typeKey('tab')
        typeKey('tab')
        typeKey('space')


def fixDemandElements(solution):
    """
    @brief fix stoppingPlaces
    """
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


def openAboutDialog(waitingTime=DELAY_QUESTION):
    """
    @brief open and close about dialog
    """
    # type F12 to open about dialog
    typeKey('F12')
    # wait before closing
    time.sleep(waitingTime)
    # press enter to close dialog (Ok must be focused)
    typeKey('space')

#################################################
    # demand elements
#################################################

def fixDemandElement(value):
    """
    @brief fix demand element
    """
    # focus current frame
    focusOnFrame()
    # jump to option
    for _ in range(value):
        typeTwoKeys('shift', 'tab')
    # type space to select
    typeKey('space')
    # accept
    typeTwoKeys('alt', 'a')