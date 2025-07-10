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

# @file    elements.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ..enums.attributesEnum import attrs
from ..general.functions import focusOnFrame
from ..input.keyboard import typeKey, updateText


def changeElement(frame, element):
    """
    @brief change element in the given frame (Additional, shape, vehicle...)
    """
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    if (frame == "additionalFrame"):
        for _ in range(attrs.frames.changeElement.additional):
            typeKey('tab')
    elif (frame == "shapeFrame"):
        for _ in range(attrs.frames.changeElement.shape):
            typeKey('tab')
    elif (frame == "vehicleFrame"):
        for _ in range(attrs.frames.changeElement.vehicle):
            typeKey('tab')
    elif (frame == "routeFrame"):
        for _ in range(attrs.frames.changeElement.route):
            typeKey('tab')
    elif (frame == "personFrame"):
        for _ in range(attrs.frames.changeElement.person):
            typeKey('tab')
    elif (frame == "containerFrame"):
        for _ in range(attrs.frames.changeElement.container):
            typeKey('tab')
    elif (frame == "personPlanFrame"):
        for _ in range(attrs.frames.changeElement.personPlan):
            typeKey('tab')
    elif (frame == "containerPlanFrame"):
        for _ in range(attrs.frames.changeElement.containerPlan):
            typeKey('tab')
    elif (frame == "stopFrameFrame"):
        for _ in range(attrs.frames.changeElement.stop):
            typeKey('tab')
    elif (frame == "meanDataFrame"):
        for _ in range(attrs.frames.changeElement.meanData):
            typeKey('tab')
    # paste the new value
    updateText(element)
    # type enter to save change
    typeKey('enter')


def changeParentElement(frame, element):
    """
    @brief change parent element in the given frame (stop...)
    """
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    if (frame == "routeFrame"):
        for _ in range(attrs.frames.changeParentElement.route):
            typeKey('tab')
    elif (frame == "stopFrame"):
        for _ in range(attrs.frames.changeParentElement.stop):
            typeKey('tab')
    # paste the new value
    updateText(element)
    # type enter to save change
    typeKey('enter')


def changePlan(type, plan, flow):
    """
    @brief change plan (in person or container frame)
    """
    # focus current frame
    focusOnFrame()
    # continue depending of type
    if (type == "person"):
        # jump to person plan
        if (flow):
            for _ in range(attrs.frames.changePlan.personFlow):
                typeKey('tab')
        else:
            for _ in range(attrs.frames.changePlan.person):
                typeKey('tab')
    elif (type == "container"):
        # jump to container plan
        if (flow):
            for _ in range(attrs.frames.changePlan.containerFlow):
                typeKey('tab')
        else:
            for _ in range(attrs.frames.changePlan.container):
                typeKey('tab')
    # paste the new plan
    updateText(plan)
    # type enter to save change
    typeKey('enter')
