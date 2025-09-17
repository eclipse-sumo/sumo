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

# @file    createEdge.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time

from ...enums.attributesEnum import attrs
from ...constants import DELAY_SELECT
from ...general.functions import focusOnFrame
from ...input.keyboard import typeKey


def selectEdgeType():
    """
    @brief select edge type
    """
    # focus current frame
    focusOnFrame()
    # go to select button
    for _ in range(attrs.edge.edgeType.select):
        typeKey('tab')
    # type space to press button
    typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def createNewEdgeType(existent: bool):
    """
    @brief create new edge type
    """
    # focus current frame
    focusOnFrame()
    # go to cancel button
    if existent:
        for _ in range(attrs.edge.edgeType.createExistent):
            typeKey('tab')
    else:
        for _ in range(attrs.edge.edgeType.createNew):
            typeKey('tab')
    # type space to press button
    typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)
