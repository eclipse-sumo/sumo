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
        type = 1
        radius = 2
        keepClear = 3
        rightOfWay = 4
        fringe = 5
        name = 6
        parameters = 9
    class inspectSelectionTLS:
        type = 1
        radius = 2
        keepClear = 3
        rightOfWay = 4
        fringe = 5
        name = 6
        parameters = 9

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

# connection
class connection:
    class inspect:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        allowButton = 6         # missing
        allow = 7               # missing
        disallowButton = 8      # missing
        disallow = 9            # missing
        speed = 10
        length = 11             # missing
        customShape = 12    
        changeLeftButton = 13   # missing
        changeLeft = 14         # missing
        changeRightButton = 15  # missing
        changeRignt = 16        # missing
        indirect = 17           # missing
        type = 18               # missing
        parameters = 21
    class inspectTLS:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        linkIndex = 6
        linkIndex2 = 7          # missing
        allowButton = 8         # missing
        allow = 9               # missing
        disallowButton = 10     # missing
        disallow = 11           # missing
        speed = 12
        length = 13             # missing
        customShape = 14
        changeLeftButton = 15   # missing
        changeLeft = 16         # missing
        changeRightBtton = 17   # missing
        changeRignt = 18        # missing
        indirect = 19           # missing
        type = 20               # missing
        parameters = 23
    class inspectSelection:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        allowButton = 6         # missing
        allow = 7               # missing
        disallowButton = 8      # missing
        disallow = 9            # missing
        speed = 10
        length = 11             # missing
        customShape = 12    
        changeLeftButton = 13   # missing
        changeLeft = 14         # missing
        changeRightButton = 15  # missing
        changeRignt = 16        # missing
        indirect = 17           # missing
        type = 18               # missing
        parameters = 20
    class inspectSelectionTLS:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        linkIndex = 6
        linkIndex2 = 7          # missing
        allowButton = 8         # missing
        allow = 9               # missing
        disallowButton = 10     # missing
        disallow = 11           # missing
        speed = 12
        length = 13             # missing
        customShape = 14
        changeLeftButton = 15   # missing
        changeLeft = 16         # missing
        changeRightBtton = 17   # missing
        changeRignt = 18        # missing
        indirect = 19           # missing
        type = 20               # missing
        parameters = 22

# crossing
class crossing:
    class create:
        edges = 1
        priority = 2
        width = 3
    class createTLS:
        edges = 1
        width = 2
    class inspect:
        edges = 1
        priority = 2
        width = 3
        customShape = 4
        parameters = 7
    class inspectTLS:
        edges = 1
        priority = 2
        width = 3
        linkIndex = 4
        linkIndex2 = 5
        customShape = 6
        parameters = 9
    class inspectSelection:
        priority = 1
        width = 2
        parameters = 5
    class inspectSelectionTLS:
        priority = 1
        width = 2
        linkIndex = 3
        linkIndex2 = 4
        parameters = 7

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

# parkingSpace
class parkingSpace:
    class create:
        name = 5
        width = 6
        lenght = 7
        angle = 8
        slope = 9       # missing       
        parent = 12
    class inspect: 
        pos = 1
        name = 2
        width = 3
        lenght = 4
        angle = 5
        slope = 6       # missing
        parameters = 9
        parent = 13
    class inspectSelection:
        name = 1
        width = 2
        lenght = 3
        angle = 4
        slope = 5       # missing
        parameters = 8
        parent = 11

# E1
class E1:
    class create:
        idEnable = 5    # missing
        id = 6          # missing
        freq = 6
        name = 7
        file = 8
        vTypes = 9 
        friendlyPos = 10
    class inspect:
        id = 1
        lane = 2
        pos = 3
        freq = 4
        name = 5
        file = 6
        vTypes = 7
        friendlyPos = 8
        parameters = 11
    class inspectSelection:
        freq = 1
        name = 2
        file = 3
        vTypes = 4
        friendlyPos = 5
        parameters = 8

# E2
class E2:
    class create:
        idEnable = 5
        id = 6
        length = 6
        freq = 7
        tl = 8
        name = 9
        file = 10
        vTypes = 11 
        timeThreshold = 12
        speedThreshold = 13
        jamThreshold= 14
        friendlyPos = 15
    class inspect:
        id = 1
        lane = 2
        pos = 3
        length = 4
        freq = 5
        tl = 6
        name = 7
        file = 8
        vTypes = 9 
        timeThreshold = 10
        speedThreshold = 11
        jamThreshold= 12
        friendlyPos = 13
        parameters = 16
    class inspectSelection:
        length = 1
        freq = 2
        tl = 3
        name = 4
        file = 5
        vTypes = 6 
        timeThreshold = 7
        speedThreshold = 8
        jamThreshold= 9
        friendlyPos = 10
        parameters = 13

# E2Multilane
class E2Multilane:
    class create:
        idEnable = 5
        id = 6
        freq = 6
        tl = 7
        name = 8
        file = 9
        vTypes = 10 
        timeThreshold = 11
        speedThreshold = 12
        jamThreshold= 13
        friendlyPos = 14
    class inspect:
        id = 1
        lanes = 2
        pos = 3
        endPos = 4
        freq = 5
        tl = 6
        name = 7
        file = 8
        vTypes = 9 
        timeThreshold = 10
        speedThreshold = 11
        jamThreshold= 12
        friendlyPos = 13
        parameters = 16
    class inspectSelection:
        freq = 1
        tl = 2
        name = 3
        file = 4
        vTypes = 5 
        timeThreshold = 6
        speedThreshold = 7
        jamThreshold= 8
        friendlyPos = 9
        parameters = 12

# E3
class E3:
    class create:
        idEnable = 5
        id = 6
        freq = 6
        name = 7
        file = 8
        vTypes = 9 
        timeThreshold = 10
        speedThreshold = 11
    class inspect:
        id = 1
        pos = 2
        freq = 3
        name = 4
        file = 5
        vTypes = 6 
        timeThreshold = 7
        speedThreshold = 8
        parameters = 11
    class inspectSelection:
        freq = 1
        name = 2
        file = 3
        vTypes = 4 
        timeThreshold = 5
        speedThreshold = 6
        parameters = 9

# entryExit
class entryExit:
    class create:
        friendlyPos = 5
        parent = 8
    class inspect:
        lane = 1
        pos = 2
        friendlyPos = 3
        parameters = 6
        parent = 10
    class inspectSelection:
        friendlyPos = 1
        parameters = 4
        parent = 7

# E1Instant
class E1Instant:
    class create:
        idEnable = 5
        id = 6
        name = 6
        file = 7
        vTypes = 8
        friendlyPos = 9
    class inspect:
        id = 1
        lane = 2
        pos = 3
        name = 4
        file = 5
        vTypes = 6
        friendlyPos = 7
        parameters = 10
    class inspectSelection:
        name = 1
        file = 2
        vTypes = 3
        friendlyPos = 4
        parameters = 7

# calibrator
class calibrator:
    class create:
        idEnable = 5
        id = 6              # missing
        pos = 6
        frequency = 7
        name = 8
        routeProbe = 9
        output = 10
        jamTreshold = 11    # missing
        vTypes = 12         # missing
        center = 16
    class inspect:
        id = 1
        edgeLane = 2
        pos = 3
        frequency = 4
        name = 5
        routeProbe = 6
        output = 7
        jamTreshold = 8     # missing
        vTypes = 9          # missing
        parameters = 12
    class inspectSelection:
        pos = 1
        frequency = 2
        name = 3
        routeProbe = 4
        output = 5
        jamTreshold = 6     # missing
        vTypes = 7          # missing
        parameters = 10

#--------------------------------
# SHAPE ELEMENTS
#--------------------------------

# Poly
class Poly:
    class create:
        idEnable = 5
        id = 6
        colorButton = 6
        color = 7
        fill = 8
        lineWidth = 9
        layer = 10
        type = 11
        name = 12
        imgFile = 13
        reativePath = 14
        angle = 15
        closeShape = 19
    class inspect:
        id = 1
        shape = 2
        colorButton = 3     # missing
        color = 4
        fill = 5
        lineWidth = 6
        layer = 7
        type = 8
        name = 9            # missing
        imgFile = 10
        reativePath = 11
        angle = 12
        geoshape = 15
        geo = 16
        parameters = 17
        closeShape = 21
    class inspectSelection:
        colorButton = 1
        color = 2
        fill = 3            # missing
        lineWidth = 4
        layer = 5
        type = 6
        name = 7            # missing
        imgFile = 8
        reativePath = 9
        angle = 10
        geo = 13
        parameters = 15
        closeShape = 18

# POI
class POI:
    class create:
        idEnable = 5
        id = 6
        colorButton = 6
        color = 7
        type = 8
        name = 9
        layer = 10
        width = 11
        height = 12
        imgFile = 13
        relativePath = 14
        angle = 15
    class inspect:
        id = 1
        pos = 2
        colorButton = 3     # missing
        color = 4
        type = 5
        name = 6
        layer = 7
        width = 8
        height = 9
        imgFile = 10
        relativePath = 11
        angle = 12
        parameters = 15
    class inspectSelection:
        colorButton = 1     # missing
        color = 2
        type = 3
        name = 4
        layer = 5
        width = 6
        height = 7
        imgFile = 8
        relativePath = 9
        angle = 10
        parameters = 13

# POIGEO
class POIGEO:
    class create:
        idEnable = 5
        id = 6
        colorButton = 6
        color = 7
        type = 8
        name = 9
        layer = 10
        width = 11
        height = 12
        imgFile = 13
        relativePath = 14
        angle = 15
        lonLat = 20         # missing
    class inspect:
        id = 1
        lon = 2
        lat = 3
        colorButton = 4     # missing
        color = 5
        type = 6
        name = 7
        layer = 8
        width = 9
        height = 10
        imgFile = 11
        relativePath = 12
        angle = 13
        parameters = 16
    class inspectSelection:
        colorButton = 1     # missing
        color = 2
        type = 3
        name = 4
        layer = 5
        width = 6
        height = 7
        imgFile = 8
        relativePath = 9
        angle = 10
        parameters = 13

# POILane
class POILane:
    class create:
        idEnable = 5
        id = 6
        friendlyPos = 6
        posLat = 7
        colorButton = 8
        color = 9
        type = 10
        name = 11
        layer = 12
        width = 13
        height = 14
        imgFile = 15
        relativePath = 16
        angle = 14
    class inspect:
        id = 1
        lane = 2
        pos = 3
        friendlyPos = 4
        posLat = 5
        colorButton = 6     # missing
        color = 7
        type = 8
        name = 9
        layer = 10
        width = 11
        height = 12
        imgFile = 13
        relativePath = 14
        angle = 15
        parameters = 18
    class inspectSelection:
        friendlyPos = 1
        posLat = 2
        colorButton = 3     # missing
        color = 4
        type = 5
        name = 6
        layer = 7
        width = 8
        height = 9
        imgFile = 10
        relativePath = 11
        angle = 12
        parameters = 15

#--------------------------------
# TAZ ELEMENTS
#--------------------------------

# TAZ
class TAZ:
    class create:
        center = 3
        fill = 4
        colorButton = 5
        color = 6
        name = 7
        edgesWithin = 8
    class inspect:
        id = 1
        shape = 2
        center = 3
        fill = 4
        colorButton = 5         # missing
        color = 6
        name = 7
        parameters = 10
    class inspectSelection:
        center = 1
        fill = 2
        colorButton = 3
        color = 4
        name = 5
        parameters = 8

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
        colorButton = 1 # missing
        color = 2
        repeat = 3
        cycletime = 4 
        parameters = 7

# trip (from-to edges)
class tripFromToEdge:
    class create:
        type = 5
        idEnable = 8        # missing
        id = 9              # missing
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
        type = 5
        idEnable = 8        # missing
        id = 9              # missing
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

# vehicle (over route)
class vehicle:
    class create:
        type = 5
        idEnable = 8        # missing
        id = 9              # missing
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
        route = 3
        departEdge = 4      # missing
        arrivalEdge = 5     # missing
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

# flow (over route)
class routeflow:
    class create:
        type = 5
        idEnable = 8        # missing
        id = 9              # missing
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
        route = 3
        departEdge = 4      # missing
        arrivalEdge = 5     # missing
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

# vehicle (embedded route)
class vehicleEmbedded:
    class create:
        type = 5
        idEnable = 8        # missing
        id = 9              # missing
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
        id = 1              # missing
        type = 2
        departEdge = 3      # missing
        arrivalEdge = 4     # missing
        colorButton = 5     # missing
        color = 6
        departLane = 7
        departPos = 8
        departSpeed = 9
        arrivalLane = 10
        arrivalPos = 11
        arrivalSpeed = 12
        line = 13
        personNumber = 14
        containerNumber = 15
        departPosLat = 16
        arrivalPosLat = 17
        depart = 18
        parameters = 21
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

# flow (embedded route)
class flowEmbedded:
    class create:
        type = 5
        idEnable = 8        # missing
        id = 9              # missing
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
        id = 1              # missing
        type = 2
        departEdge = 3      # missing
        arrivalEdge = 4     # missing
        colorButton = 5     # missing
        color = 6
        departLane = 7
        departPos = 8
        departSpeed = 9
        arrivalLane = 10
        arrivalPos = 11
        arrivalSpeed = 12
        line = 13
        personNumber = 14
        containerNumber = 15
        departPosLat = 16
        arrivalPosLat = 17
        begin = 18          # missing
        # flow attributes missing
        parameters = 29
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
        begin = 15          # missing
        # flow attributes missing
        parameters = 26

# stop (over additionals)
class stop:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        triggered = 12
        expected = 14
        permitted = 14
        parking = 15
        actType = 16
        actTypeParking = 15
    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        triggered = 5
        expected = 7
        permitted = 7
        parking = 8
        actType = 9
        actTypeParking = 8
    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 8
        permitted = 9
        parking = 10
        actType = 11
        actTypeParking = 10

# stop (over lane)
class stopLane:
    class create:
        friendlyPos = 8
        posLat = 9
        durationEnable = 10
        duration = 11
        untilEnable = 11
        until = 12
        extensionEnable = 12
        extension = 13
        triggered = 14
        expected = 15
        permitted = 16
        parking = 17
        actType = 18
    class inspect:
        lane = 1
        startPos = 2
        endPos = 3
        friendlyPos = 4
        posLat = 5
        durationEnable = 6
        duration = 7
        untilEnable = 7
        until = 8
        extensionEnable = 8
        extension = 9
        triggered = 10
        expected = 12
        permitted = 12
        parking = 13
        actType = 14
    class inspectSelection:
        friendlyPos = 1
        posLat = 2
        durationEnable = 3
        duration = 4
        untilEnable = 4
        until = 5
        extensionEnable = 5
        extension = 6
        triggered = 6
        expected = 8
        permitted = 9
        parking = 10
        actType = 11

#--------------------------------
# DATA ELEMENTS
#--------------------------------

# edgeData
class edgeData:
    class create:
        parameters = 12
    class inspect:
        parameters = 7
    class inspectSelection:
        parameters = 7

# edgeRelData
class edgeRelData:
    class create:
        parameters = 12
    class inspect:
        parameters = 7
    class inspectSelection:
        parameters = 7