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

# @file    stop.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...general.functions import *

def changeStopParent(stopParent):
    """
    @brief change stop parent
    """
    # focus current frame
    focusOnFrame()
    for _ in range(2):
        typeKey('tab')
    # paste the new stop parent
    updateText(stopParent)
    # type enter to save change
    typeKey('enter')


def changeStopType(stopType):
    """
    @brief change stop type
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(5):
        typeKey('tab')
    # paste the new personPlan
    updateText(stopType)
    # type enter to save change
    typeKey('enter')