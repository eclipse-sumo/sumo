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

# @file    additional.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...general.functions import focusOnFrame
from ...input.keyboard import typeKey


def selectAdditionalChild(attributeIndex, childNumber):
    """
    @brief select child of additional
    """
    # focus current frame
    focusOnFrame()
    # place cursor in the list of childs
    for _ in range(attributeIndex):
        typeKey('tab')
    # select child
    for _ in range(childNumber):
        typeKey('down')
    typeKey('space')
    # use TAB to select additional child
    typeKey('tab')
