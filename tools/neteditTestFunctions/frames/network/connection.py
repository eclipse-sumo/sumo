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

# @file    connection.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...general.functions import *


def createConnection(referencePosition, fromLanePosition, toLanePosition, mode=""):
    """
    @brief create connection
    """
    # check if connection has to be created in certain mode
    if mode == "conflict":
        keyPress('ctrl')
    elif mode == "yield":
        keyPress('shift')
    # select first lane
    leftClick(referencePosition, fromLanePosition)
    # select another lane for create a connection
    leftClick(referencePosition, toLanePosition)
    # check if connection has to be created in certain mode
    if mode == "conflict":
        keyRelease('ctrl')
    elif mode == "yield":
        keyRelease('shift')


def saveConnectionEdit():
    """
    @brief Change to crossing mode
    """
    # focus current frame
    focusOnFrame()
    # go to cancel button
    for _ in range(attrs.connection.saveConnections):
        typeKey('tab')
    # type space to press button
    typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)
