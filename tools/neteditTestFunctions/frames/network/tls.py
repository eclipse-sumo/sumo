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

# @file    tls.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ...enums.attributesEnum import attrs
from ...constants import DELAY_SELECT
from ...general.functions import focusOnFrame, typeKey


def createTLS():
    """
    @brief Create TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.create):
        typeKey('tab')
    # create TLS
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)


def createTLSOverlapped():
    """
    @brief Create TLS in overlapped junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.createOverlapped):
        typeKey('tab')
    # press space
    typeKey('space')
    for _ in range(attrs.TLS.createOverlapped):
        typeKey('tab')
    # create TLS
    typeKey('space')


def copyTLS(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.copyJoined):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.copySingle):
            typeKey('tab')
    # create TLS
    typeKey('space')


def joinTSL():
    """
    @brief join TLS
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.joinTLS):
        typeKey('tab')
    # create TLS
    typeKey('space')


def disJoinTLS():
    """
    @brief disjoin the current TLS
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.disjoinTLS):
        typeKey('tab')
    # create TLS
    typeKey('space')


def deleteTLS(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to delete TLS button
    if (joined):
        for _ in range(attrs.TLS.deleteJoined):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.deleteSingle):
            typeKey('tab')
    # create TLS
    typeKey('space')


def resetSingleTLSPhases(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.resetPhaseSingle):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.resetPhaseJoined):
            typeKey('tab')
    # create TLS
    typeKey('space')


def resetAllTLSPhases(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.resetAllJoined):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.resetAllSingle):
            typeKey('tab')
    # create TLS
    typeKey('space')
