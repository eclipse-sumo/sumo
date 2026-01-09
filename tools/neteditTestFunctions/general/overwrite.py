# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    overwrite.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..constants import DELAY_QUESTION
from ..input.keyboard import typeKey, typeTwoKeys


def overwritingAccept():
    """
    @brief overwrite Element
    """
    # simple type space, because is the default focused button
    typeKey('space')
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)


def overwritingCancel():
    """
    @brief overwrite Element
    """
    # go to no button
    typeKey('tab')
    # press cancel
    typeKey('space')
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)


def overwritingAbort():
    """
    @brief abort overwriting
    """
    # go to cancel button
    for _ in range(2):
        typeKey('tab')
    # press cancel
    typeKey('space')
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)


def overwritingApplyToAll():
    """
    @brief overwritte Element
    """
    # go to check
    typeTwoKeys('shift', 'tab')
    # toggle check
    typeKey('space')
    # go back to accept buton
    typeKey('tab')
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
