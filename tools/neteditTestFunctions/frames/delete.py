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

# @file    delete.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..enums.attributesEnum import attrs
from ..constants import DELAY_REMOVESELECTION, DELAY_QUESTION
from ..general.modes import changeMode
from ..input.keyboard import typeKey


def delete():
    """
    @brief delete using SUPR key
    """
    typeKey('del')
    # wait for GL Debug
    time.sleep(DELAY_REMOVESELECTION)


def protectElements():
    """
    @brief Protect or unprotect delete elements
    """
    # select delete mode again to set mode
    changeMode("delete")
    # jump to checkbox
    for _ in range(attrs.frames.delete.protectElements):
        typeKey('tab')
    # type SPACE to change value
    typeKey('space')


def waitDeleteWarning():
    """
    @brief close warning about automatically delete additionals
    """
    # wait 0.5 second to question dialog
    time.sleep(DELAY_QUESTION)
    # press enter to close dialog
    typeKey('enter')
