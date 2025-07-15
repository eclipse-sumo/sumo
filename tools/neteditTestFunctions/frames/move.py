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

# @file    move.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ..enums.attributesEnum import attrs
from ..general.functions import focusOnFrame
from ..input.keyboard import typeKey
from ..input.mouse import dragDrop, leftClick


def moveElementHorizontal(referencePosition, originalPosition, radius):
    """
    @brief move element in horizontal
    """
    leftClick(referencePosition, originalPosition)
    # move element
    dragDrop(referencePosition, originalPosition.x, originalPosition.y,
             originalPosition.x + radius.right, originalPosition.y)
    dragDrop(referencePosition, originalPosition.x + radius.right,
             originalPosition.y, originalPosition.x + radius.left, originalPosition.y)


def moveElementVertical(referencePosition, originalPosition, radius):
    """
    @brief move element in vertical
    """
    # move element
    if (radius.up != 0):
        dragDrop(referencePosition, originalPosition.x, originalPosition.y,
                 originalPosition.x, originalPosition.y + radius.up)
    if (radius.down != 0):
        dragDrop(referencePosition, originalPosition.x, originalPosition.y + radius.up,
                 originalPosition.x, originalPosition.y + radius.down)


def moveElement(referencePosition, originalPosition, radius):
    """
    @brief move element
    """
    # move element
    dragDrop(referencePosition,
             originalPosition.x,
             originalPosition.y,
             originalPosition.x + radius.right,
             originalPosition.y)
    dragDrop(referencePosition,
             originalPosition.x + radius.right,
             originalPosition.y,
             originalPosition.x + radius.right,
             originalPosition.y + radius.down)
    dragDrop(referencePosition,
             originalPosition.x + radius.right,
             originalPosition.y + radius.down,
             originalPosition.x + radius.left,
             originalPosition.y + radius.down)
    dragDrop(referencePosition,
             originalPosition.x + radius.left,
             originalPosition.y + radius.down,
             originalPosition.x + radius.left,
             originalPosition.y + radius.up)


def moveGeometryPoint(referencePosition, originalPosition, destinyPositionA, destinyPositionB):
    """
    @brief move geometry point
    """
    leftClick(referencePosition, originalPosition)
    # move element
    dragDrop(referencePosition, originalPosition.x, originalPosition.y, destinyPositionA.x, destinyPositionA.y)
    dragDrop(referencePosition, destinyPositionA.x, destinyPositionA.y, destinyPositionB.x, destinyPositionB.y)


def toggleMoveEntireShape():
    """
    @brief toggle move entire shape
    """
    # focus current frame
    focusOnFrame()
    for _ in range(attrs.move.moveWholePolygon):
        typeKey('tab')
    # type space to create crossing
    typeKey('space')
