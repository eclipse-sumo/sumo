# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    attributesEnum.py
# @author  Pablo Alvarez Lopez
# @date    2022-01-31

#--------------------------------
# NETWORK
#--------------------------------

# junction
class junction:
    class inspect:
        id = 1
        pos = 2
        type = 3
        shape = 5
        radius = 6
        keepClear = 7
        rightOfWay = 8
        fringe = 10
        name = 12
        parameters = 15
    class inspectTLS:
        id = 1
        pos = 2
        type = 3
        shape = 5
        radius = 6
        keepClear = 7
        rightOfWay = 8
        fringe = 10
        name = 12
        tlType = 13     # missing
        tlLayout = 15   # missing
        tl = 17         # missing
        parameters = 20
    class inspectSelection:
        type = 3
        radius = 4
        keepClear = 5
        rightOfWay = 6
        fringe = 7
        name = 8
        parameters = 11

# edge
class edge:
    class inspect:
        id = 1
        fromEdge = 2
        toEdge = 3
        speed = 4
        priority = 5
        numLanes = 6
        type = 7
        allowedButton = 8       # missing
        allowed = 9
        disallowedButton = 10   # missing
        disallowed = 11
        shape = 12
        length = 13
        spreadType = 14
        name = 16
        width = 17
        endOffset = 18
        shapeStart = 19
        shapeEnd = 20
        distance = 21               # missing
        stopOffset = 22             # missing
        stopExceptionButton = 23    # missing
        stopException = 24          # missing
        parameters = 27
    class inspectSelection:
        speed = 1
        priority = 2
        numLanes = 3
        type = 4
        allowedButton = 5       # missing
        allowed = 6
        disallowedButton = 7    # missing
        disallowed = 8
        length = 9
        spreadType = 10
        name = 12
        width = 13
        endOffset = 14
        shapeStart = 15
        shapeEnd = 16
        stopOffset = 17             # missing
        stopExceptionButton = 18    # missing
        stopException = 19          # missing
        parameters = 22

# lane
class lane:
    class inspect:
        speed = 1
        allowButton = 2             # missing
        allow = 3
        disallowButton = 4          # missing
        disallow = 5
        width = 6
        endOffset = 7
        acceleration = 8
        customShape = 9
        opposite = 10               # missing
        changeLeftButton = 11       # missing
        changeLeft = 12             # missing
        changeRightButton = 13      # missing
        changeRight = 14            # missing
        type = 15                   # missing
        stopOffset = 16             # missing
        stopOffsetException = 17    # missing
        parameters = 21
    class inspectSelection:
        speed = 1
        allowButton = 2             # missing
        allow = 3
        disallowButton = 4          # missing
        disallow = 5
        width = 6
        endOffset = 7
        acceleration = 8
        changeLeftButton = 9        # missing
        changeLeft = 10             # missing
        changeRightButton = 11      # missing
        changeRight = 12            # missing
        type = 13                   # missing
        stopOffset = 14             # missing
        stopOffsetException = 15    # missing
        parameters = 19

#--------------------------------
# ADDITIONALS
#--------------------------------

# busStop
class busStop:
    class create:
        idEnable = 5
        id = 6
        name = 6
        friendlyPos = 7
        lines = 8
        personCapacity = 9
        parkingLength = 10
        colorDialog = 11
        color = 12
        references = 16
        length = 18
    class inspect:
        id = 1
        lane = 2
        startPos = 3
        endPos = 4
        name = 5
        friendlyPos = 6
        lines = 7
        personCapacity = 8
        parkingLength = 9
        colorDialog = 10        # missing
        color = 11
        parameters = 14
    class inspectSelection:
        name = 1
        friendlyPos = 2
        lines = 3
        personCapacity = 4
        parkingLength = 5
        colorDialog = 6        # missing
        color = 7
        parameters = 10

# trainStop
class trainStop:
    class create:
        idEnable = 5
        id = 6
        name = 6
        friendlyPos = 7
        lines = 8
        personCapacity = 9
        parkingLength = 10
        colorDialog = 11
        color = 12
        references = 16
        length = 18
    class inspect:
        id = 1
        lane = 2
        startPos = 3
        endPos = 4
        name = 5
        friendlyPos = 6
        lines = 7
        personCapacity = 8
        parkingLength = 9
        colorDialog = 10        # missing
        color = 11
        parameters = 14
    class inspectSelection:
        name = 1
        friendlyPos = 2
        lines = 3
        personCapacity = 4
        parkingLength = 5
        colorDialog = 6        # missing
        color = 7
        parameters = 10
        
# containerStop
class containerStop:
    class create:
        idEnable = 5
        id = 6
        name = 6
        friendlyPos = 7
        lines = 8
        containerCapacity = 9
        parkingLength = 10
        colorDialog = 11
        color = 12
        references = 16
        length = 18
    class inspect:
        id = 1
        lane = 2
        startPos = 3
        endPos = 4
        name = 5
        friendlyPos = 6
        lines = 7
        containerCapacity = 8
        parkingLength = 9
        colorDialog = 10        # missing
        color = 11
        parameters = 14
    class inspectSelection:
        name = 1
        friendlyPos = 2
        lines = 3
        containerCapacity = 4
        parkingLength = 5
        colorDialog = 6        # missing
        color = 7
        parameters = 10

# chargingStation
class chargingStation:
    class create:
        idEnable = 5
        id = 6
        name = 6
        friendlyPos = 7
        power = 8
        efficiency = 9
        chargeInTransit = 10
        chargeDelay = 11
        references = 15
        length = 17
    class inspect:
        id = 1
        lane = 2
        startPos = 3
        endPos = 4
        name = 5
        friendlyPos = 6
        power = 7
        efficiency = 8
        chargeInTransit = 9
        chargeDelay = 10
        parameters = 13
    class inspectSelection:
        name = 1
        friendlyPos = 2
        power = 3
        efficiency = 4
        chargeInTrainsit = 5
        chargeDelay = 6
        parameters = 9

# access
class access:
    class create:
        length = 5
        friendlyPos = 6
        parent = 9
    class inspect:
        lane = 1
        pos = 2
        length = 3
        friendlyPos = 4
    class inspectSelection:
        length = 1
        friendlyPos = 2


# parkingArea
class parkingArea:
    class create:
        idEnable = 5
        id = 6
        departPos = 6           # missing
        name = 7
        roadSideCapacity = 8
        onRoad = 9
        friendlyPos = 10
        width = 11
        length = 12
        angle = 13
        references = 17
        referencesLength = 19
    class inspect:
        id = 1                  # missing
        lane = 2
        startPos = 3
        endPos = 4
        departPos = 5           # missing
        name = 6
        roadSideCapacity = 7
        onRoad = 8
        friendlyPos = 9
        width = 10
        length = 11
        angle = 12
        parameters = 15
    class inspectSelection:
        departPos = 1           # missing
        name = 2
        roadSideCapacity = 3
        onRoad = 4
        friendlyPos = 5
        width = 6
        length = 7
        angle = 8
        parameters = 11

#--------------------------------
# DEMAND ELEMENTS
#--------------------------------

# route
class route:
    class create:
        idEnable = 7
        id = 8
        colorButton = 8     # missing
        color = 9
        repeat = 10
        cycletime = 11
    class inspect:
        id = 1
        edges = 2
        colorButton = 3     # missing
        color = 4
        repeat = 5
        cycletime = 6
        parameters = 9
    class inspectSelection:
        colorButton = 2
        color = 3
        repeat = 4
        cycletime = 5
        parameters = 8

# trip (from-to edges)
class tripFromToEdge:
    class create:
        idEnable = 8        # missing
        id = 9              # missing
        type = 5
        colorButton = 9     # missing
        color = 10
        departLane = 11
        departPos = 12
        departSpeed = 13
        arrivalLane = 14
        arrivalPos = 15
        arrivalSpeed = 16
        line = 17
        personNumber = 18
        containerNumber = 19
        departPosLat = 20
        arrivalPosLat = 21
        depart = 22
    class inspect:
        id = 1
        type = 2
        fromEdge = 3
        toEdge = 4
        via = 5
        colorButton = 6     # missing
        color = 7
        departLane = 8
        departPos = 9
        departSpeed = 10
        arrivalLane = 11
        arrivalPos = 12
        arrivalSpeed = 13
        line = 14
        personNumber = 15
        containerNumber = 16
        departPosLat = 17
        arrivalPosLat = 18
        depart = 19
        parameters = 22
    class inspectSelection:
        type = 1            # missing
        colorButton = 2
        color = 3
        departLane = 4
        departPos = 5
        departSpeed = 6
        arrivalLane = 7
        arrivalPos = 8
        arrivalSpeed = 9
        line = 10
        personNumber = 11
        containerNumber = 12
        departPosLat = 13
        arrivalPosLat = 14
        depart = 15
        parameters = 18

# flow (from-to edges)
class flowFromToEdge:
    class create:
        idEnable = 8        # missing
        id = 9              # missing
        type = 5
        colorButton = 9     # missing
        color = 10
        departLane = 11
        departPos = 12
        departSpeed = 13
        arrivalLane = 14
        arrivalPos = 15
        arrivalSpeed = 16
        line = 17
        personNumber = 18
        containerNumber = 19
        departPosLat = 20
        arrivalPosLat = 21
        # flow attributes missing
    class inspect:
        id = 1
        type = 2
        fromEdge = 3
        toEdge = 4
        via = 5
        colorButton = 6     # missing
        color = 7
        departLane = 8
        departPos = 9
        departSpeed = 10
        arrivalLane = 11
        arrivalPos = 12
        arrivalSpeed = 13
        line = 14
        personNumber = 15
        containerNumber = 16
        departPosLat = 17
        arrivalPosLat = 18
        # flow attributes missing
        parameters = 30
    class inspectSelection:
        type = 1            # missing
        colorButton = 2
        color = 3
        departLane = 4
        departPos = 5
        departSpeed = 6
        arrivalLane = 7
        arrivalPos = 8
        arrivalSpeed = 9
        line = 10
        personNumber = 11
        containerNumber = 12
        departPosLat = 13
        arrivalPosLat = 14
        # flow attributes missing
        parameters = 26