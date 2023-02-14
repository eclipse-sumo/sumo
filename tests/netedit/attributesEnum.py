# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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

# --------------------------------
# GENERAL
# --------------------------------


class editElements:
    overlapped = 5


class selection:
    class lockSelectionNetwork:
        junctions = 1
        edges = 2
        lanes = 3
        connections = 4
        crossings = 5
        additionals = 6
        TAZs = 7
        polygons = 8
        POIs = 9


class modes:
    class network:
        grid = '1'
        junctionShape = '2'
        spreadVehicle = '3'
        showDemandElements = '4'
        selectLane = '5'
        showConnections = '6'
        showSubAdditionals = '7'
    # create edge
        chainMode = '5'
        twoWayMode = '6'
    # TLS mode
        applyAllPhases = '5'
    # move mode
        mergingJunction = '5'
        showBubbles = '6'
        moveElevation = '7'

    class demand:
        grid = '1'
        junctionShape = '2'
        spreadVehicle = '3'
        showShapes = '4'
        showAllTrips = '5'
        showPersonPlans = '6'
        lockPerson = '7'
        showContainerPlans = '8'
        lockContainer = '9'
        showNonInspected = '10'
        showOverlappedRoutes = '11'

    class data:
        junctionShape = '1'
        showAdditionals = '2'
        showShapes = '3'
        showDemandElements = '4'
        TAZRelDrawingMode = '5'
        TAZFill = '6'
        TAZRelOnlyFrom = '7'
        TAZRelOnlyTo = '8'


class frames:
    class route:
        create = 15
        abort = 16
        removeLastInsertedEdge = 17


class fix:
    class demandElements:
        class routes:
            removeStops = 1
            selectInvalidRoutes = 2
            saveInvalidRoutes = 3

# --------------------------------
# NETWORK
# --------------------------------

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
        tlType = 13
        tlLayout = 15
        tl = 17
        parameters = 20

    class inspectSelection:
        type = 1
        radius = 3
        keepClear = 4
        rightOfWay = 5
        fringe = 7
        name = 9
        parameters = 12

    class inspectSelectionTLS:
        type = 1
        radius = 3
        keepClear = 4
        rightOfWay = 5
        fringe = 7
        name = 9
        tlType = 10
        tlLayout = 12
        tl = 14
        parameters = 17

    class contextualMenu:
        addTLS = 0
        addJoinedTLS = 0
        resetEdgeEndPoints = 0
        setCustomJunctionShape = 0
        resetJunctionShape = 0
        replaceJunctionByGeometryPoint = 0
        splitJunction = 0
        splitJunctionAndReconnect = 0
        converToRoundAbout = 0
        clearConnections = 0
        resetConnections = 0

    contextualMenuTLS = 10

# edge


class edge:
    class create:
        numLanes = 7
        speed = 8
        allowButton = 9
        allow = 10
        disallow = 11
        spreadType = 12
        priority = 14
        width = 15
        sidewalkWidth = 16
        bikelaneWidth = 17

    class createLane:
        add = 23
        remove = 24
        speed = 25
        allowButton = 26
        allow = 27
        disallow = 28
        width = 29

    class inspect:
        id = 1
        fromEdge = 2
        toEdge = 3
        speed = 4
        priority = 5
        numLanes = 6
        type = 7
        allowedButton = 8
        allowed = 9
        disallowed = 10
        shape = 11
        length = 12
        spreadType = 13
        name = 15
        width = 16
        endOffset = 17
        shapeStart = 18
        shapeEnd = 19
        distance = 20
        stopOffset = 21
        stopExceptionButton = 22
        stopException = 23
        parameters = 26

    class inspectSelection:
        speed = 1
        priority = 2
        numLanes = 3
        type = 4
        allowedButton = 5
        allowed = 6
        disallowed = 7
        length = 8
        spreadType = 9
        name = 11
        width = 12
        endOffset = 13
        shapeStart = 14
        shapeEnd = 15
        stopOffset = 16
        stopExceptionButton = 17
        stopException = 18
        parameters = 21

    class template:
        create = 32
        copy = 33
        clear = 34

    class contextualMenu:
        splitEdge = 1
        splitEdgeBothDirection = 2
        setGeometryPoint = 2
        restoreGeometryPoint = 3
        reverseEdge = 4
        addReverseDirection = 5
        addReverseDirectionDisconnected = 6
        resetLenght = 7
        straighteen = 8
        smooth = 9
        straighteenElevation = 10
        smoothElevation = 11

    contextualMenuEdgeA = 10
    contextualMenuEdgeB = 11

# lane


class lane:
    class inspect:
        speed = 1
        allowButton = 2
        allow = 3
        disallow = 4
        width = 5
        endOffset = 6
        acceleration = 7
        customShape = 8
        opposite = 9
        changeLeftButton = 10
        changeLeft = 11
        changeRightButton = 12
        changeRight = 13
        type = 14
        stopOffset = 15
        stopOffsetExceptionButton = 16
        stopOffsetException = 17
        parameters = 20

    class inspectSelection:
        speed = 1
        allowButton = 2
        allow = 3
        disallow = 4
        width = 5
        endOffset = 6
        acceleration = 7
        changeLeftButton = 8
        changeLeft = 9
        changeRightButton = 10
        changeRight = 11
        type = 12
        stopOffset = 13
        stopOffsetExceptionButton = 14
        stopOffsetException = 15
        parameters = 18

    class contextualMenu:
        duplicateLane = 1
        setCustomLaneShape = 2
        resetCustomLaneShape = 3
        resetOppositeLaneShape = 4
        addRestrictedLane = 3
        removeRestrictedLane = 4
        transformRestrictedLane = 4

    class restrictedLanes:
        sidewalk = 1
        bikelane = 2
        buslane = 3
        greenvergeFront = 4
        greenvergeBack = 4

    contextualMenuLane = 11

# connection


class connection:
    class inspect:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        allowButton = 6
        allow = 7
        disallow = 8
        speed = 9
        length = 10
        customShape = 11
        changeLeftButton = 12
        changeLeft = 13
        changeRightButton = 14
        changeRight = 15
        indirect = 16
        type = 17
        parameters = 20

    class inspectTLS:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        linkIndex = 6
        linkIndex2 = 7
        allowButton = 8
        allow = 9
        disallow = 10
        speed = 11
        length = 12
        customShape = 13
        changeLeftButton = 14
        changeLeft = 15
        changeRightButton = 16
        changeRight = 17
        indirect = 18
        type = 19
        parameters = 22

    class inspectSelection:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        allowButton = 6
        allow = 7
        disallow = 8
        speed = 9
        length = 10
        changeLeftButton = 11
        changeLeft = 12
        changeRightButton = 13
        changeRight = 14
        indirect = 15
        type = 16
        parameters = 19

    class inspectSelectionTLS:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        linkIndex = 6
        linkIndex2 = 7
        allowButton = 8
        allow = 9
        disallow = 10
        speed = 11
        length = 12
        changeLeftButton = 13
        changeLeft = 14
        changeRightButton = 15
        changeRight = 16
        indirect = 17
        type = 18
        parameters = 21

    saveConnections = 4

# crossing


class crossing:
    class create:
        edges = 1
        priority = 2
        width = 3
        button = 11

    class createTLS:
        edges = 1
        width = 2
        button = 10

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

    clearEdges = 3
    clearEdgesSelected = 4
    firstField = 5


# TLS


class TLS:
    class common:
        TLID = 1
        TLType = 2

    class single:
        class attributes:
            offset = 12
            parameters = 14

        class staticPhase:
            dur = 17 + (0 * 4) + 0
            state = 17 + (1 * 4) + 1
            nxt = 17 + (2 * 4) + 2
            name = 17 + (3 * 4) + 3
            addButton = 17 + (4 * 4) + 1
            deleteButton = 17 + (5 * 4) + 2
            moveUp = 17 + (6 * 4) + 3
            moveDown = 17 + (7 * 4) + 3
            cleanStates = 17 + (8 * 4) + 1
            groupSignal = 17 + (8 * 4) + 2
            addStates = 17 + (8 * 4) + 3
            ungroupSignal = 17 + (8 * 4) + 4

        class actuatedPhase:
            dur = 18 + (0 * 4) + 0
            minD = 18 + (1 * 4) + 1
            maxD = 18 + (2 * 4) + 2
            state = 18 + (3 * 4) + 3
            earlyEnd = 18 + (4 * 4) + 0
            latestEnd = 18 + (5 * 4) + 1
            nxt = 18 + (6 * 4) + 2
            name = 18 + (7 * 4) + 3
            addButton = 18 + (8 * 4) + 1
            deleteButton = 18 + (9 * 4) + 2
            moveUp = 18 + (10 * 4) + 3
            moveDown = 18 + (11 * 4) + 4
            cleanStates = 18 + (12 * 4) + 1
            groupSignal = 18 + (12 * 4) + 2
            addStates = 18 + (12 * 4) + 3
            ungroupSignal = 18 + (12 * 4) + 4

        class delayBasedPhase:
            dur = 18 + (0 * 4) + 0
            minD = 18 + (1 * 4) + 1
            maxD = 18 + (2 * 4) + 2
            state = 18 + (3 * 4) + 3
            nxt = 18 + (4 * 4) + 0
            name = 18 + (5 * 4) + 1
            addButton = 18 + (6 * 4) + 2
            deleteButton = 18 + (7 * 4) + 3
            moveUp = 18 + (8 * 4) + 0
            moveDown = 18 + (9 * 4) + 1
            cleanStates = 18 + (10 * 4) + 1
            groupSignal = 18 + (10 * 4) + 2
            addStates = 18 + (10 * 4) + 3
            ungroupSignal = 18 + (10 * 4) + 4

        class NEMAPhase:
            dur = 18 + (0 * 4) + 0
            minD = 18 + (1 * 4) + 1
            maxD = 18 + (2 * 4) + 2
            state = 18 + (3 * 4) + 3
            vehext = 18 + (4 * 4) + 0
            yellow = 18 + (5 * 4) + 1
            red = 18 + (6 * 4) + 2
            nxt = 18 + (7 * 4) + 3
            name = 18 + (8 * 4) + 0
            addButton = 18 + (9 * 4) + 1
            deleteButton = 18 + (10 * 4) + 2
            moveUp = 18 + (11 * 4) + 3
            moveDown = 18 + (12 * 4) + 0
            cleanStates = 18 + (13 * 4) + 1
            groupSignal = 18 + (13 * 4) + 2
            addStates = 18 + (13 * 4) + 3
            ungroupSignal = 18 + (13 * 4) + 4

    class join:
        class attributes:
            offset = 13
            parameters = 15

        class staticPhase:
            dur = 18 + (0 * 2)
            state = 18 + (1 * 2)
            nxt = 18 + (2 * 2)
            name = 18 + (3 * 2)
            addButton = 18 + (5 * 2)
            deleteButton = 18 + (6 * 2)
            moveUp = 18 + (7 * 2)
            moveDown = 18 + (8 * 2)
            cleanStates = 18 + (8 * 2) + 1
            groupSignal = 18 + (8 * 2) + 2
            addStates = 18 + (8 * 2) + 3
            ungroupSignal = 18 + (8 * 2) + 4

        class actuatedPhase:
            dur = 19 + (0 * 2)
            minD = 19 + (1 * 2)
            maxD = 19 + (2 * 2)
            state = 19 + (3 * 2)
            earlyEnd = 19 + (4 * 2)
            latestEnd = 19 + (5 * 2)
            nxt = 19 + (6 * 2)
            name = 19 + (7 * 2)
            addButton = 19 + (9 * 2)
            deleteButton = 19 + (10 * 2)
            moveUp = 19 + (11 * 2)
            moveDown = 19 + (12 * 2)
            cleanStates = 19 + (12 * 2) + 1
            groupSignal = 19 + (12 * 2) + 2
            addStates = 19 + (12 * 2) + 3
            ungroupSignal = 19 + (12 * 2) + 4

        class delayBasedPhase:
            dur = 19 + (0 * 2)
            minD = 19 + (1 * 2)
            maxD = 19 + (2 * 2)
            state = 19 + (3 * 2)
            nxt = 19 + (4 * 2)
            name = 19 + (5 * 2)
            addButton = 19 + (7 * 2)
            deleteButton = 19 + (8 * 2)
            moveUp = 19 + (9 * 2)
            moveDown = 19 + (10 * 2)
            cleanStates = 19 + (10 * 2) + 1
            groupSignal = 19 + (10 * 2) + 2
            addStates = 19 + (10 * 2) + 3
            ungroupSignal = 19 + (10 * 2) + 4

        class NEMAPhase:
            dur = 19 + (0 * 2)
            minD = 19 + (1 * 2)
            maxD = 19 + (2 * 2)
            state = 19 + (3 * 2)
            vehext = 19 + (4 * 2)
            yellow = 19 + (5 * 2)
            red = 19 + (6 * 2)
            nxt = 19 + (7 * 2)
            name = 19 + (8 * 2)
            addButton = 19 + (10 * 2)
            deleteButton = 19 + (11 * 2)
            moveUp = 19 + (12 * 2)
            moveDown = 19 + (13 * 2)
            cleanStates = 19 + (13 * 2) + 1
            groupSignal = 19 + (13 * 2) + 2
            addStates = 19 + (13 * 2) + 3
            ungroupSignal = 19 + (13 * 2) + 4

    create = 3
    createOverlapped = 4
    copySingle = 9
    copyJoined = 10
    joinTLS = 5
    disjoinTLS = 6
    deleteSingle = 10
    resetPhaseSingle = 11
    resetAllSingle = 12
    deleteJoined = 11
    resetPhaseJoined = 12
    resetAllJoined = 13


# --------------------------------
# ADDITIONALS
# --------------------------------

# additionals

class additionals:
    changeElement = 2

# busStop


class busStop:
    class create:
        id = 5
        name = 6
        friendlyPos = 7
        lines = 8
        personCapacity = 9
        parkingLength = 10
        colorButton = 11
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
        colorButton = 10
        color = 11
        parameters = 14

    class inspectSelection:
        name = 1
        friendlyPos = 2
        lines = 3
        personCapacity = 4
        parkingLength = 5
        colorButton = 6
        color = 7
        parameters = 10

# trainStop


class trainStop:
    class create:
        id = 5
        name = 6
        friendlyPos = 7
        lines = 8
        personCapacity = 9
        parkingLength = 10
        colorButton = 11
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
        colorButton = 10
        color = 11
        parameters = 14

    class inspectSelection:
        name = 1
        friendlyPos = 2
        lines = 3
        personCapacity = 4
        parkingLength = 5
        colorButton = 6
        color = 7
        parameters = 10

# containerStop


class containerStop:
    class create:
        id = 5
        name = 6
        friendlyPos = 7
        lines = 8
        containerCapacity = 9
        parkingLength = 10
        colorButton = 11
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
        colorButton = 10
        color = 11
        parameters = 14

    class inspectSelection:
        name = 1
        friendlyPos = 2
        lines = 3
        containerCapacity = 4
        parkingLength = 5
        colorButton = 6
        color = 7
        parameters = 10

# chargingStation


class chargingStation:
    class create:
        id = 5
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
        id = 5
        departPos = 6
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
        id = 1
        lane = 2
        startPos = 3
        endPos = 4
        departPos = 5
        name = 6
        roadSideCapacity = 7
        onRoad = 8
        friendlyPos = 9
        width = 10
        length = 11
        angle = 12
        parameters = 15

    class inspectSelection:
        departPos = 1
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
        length = 7
        angle = 8
        slope = 9
        parent = 12

    class inspect:
        pos = 1
        name = 2
        width = 3
        length = 4
        angle = 5
        slope = 6
        parameters = 9
        parent = 13

    class inspectSelection:
        name = 1
        width = 2
        length = 3
        angle = 4
        slope = 5
        parameters = 8
        parent = 11

# E1


class E1:
    class create:
        id = 5
        period = 6
        name = 7
        file = 8
        vTypes = 9
        friendlyPos = 10

    class inspect:
        id = 1
        lane = 2
        pos = 3
        period = 4
        name = 5
        file = 6
        vTypes = 7
        friendlyPos = 8
        parameters = 11

    class inspectSelection:
        period = 1
        name = 2
        file = 3
        vTypes = 4
        friendlyPos = 5
        parameters = 8

# E2


class E2:
    class create:
        id = 5
        length = 6
        period = 7
        tl = 8
        name = 9
        file = 10
        vTypes = 11
        timeThreshold = 12
        speedThreshold = 13
        jamThreshold = 14
        friendlyPos = 15

    class inspect:
        id = 1
        lane = 2
        pos = 3
        length = 4
        period = 5
        tl = 6
        name = 7
        file = 8
        vTypes = 9
        timeThreshold = 10
        speedThreshold = 11
        jamThreshold = 12
        friendlyPos = 13
        parameters = 16

    class inspectSelection:
        length = 1
        period = 2
        tl = 3
        name = 4
        file = 5
        vTypes = 6
        timeThreshold = 7
        speedThreshold = 8
        jamThreshold = 9
        friendlyPos = 10
        parameters = 13

# E2Multilane


class E2Multilane:
    class create:
        id = 5
        period = 6
        tl = 7
        name = 8
        file = 9
        vTypes = 10
        timeThreshold = 11
        speedThreshold = 12
        jamThreshold = 13
        friendlyPos = 14

    class inspect:
        id = 1
        lanes = 2
        pos = 3
        endPos = 4
        period = 5
        tl = 6
        name = 7
        file = 8
        vTypes = 9
        timeThreshold = 10
        speedThreshold = 11
        jamThreshold = 12
        friendlyPos = 13
        parameters = 16

    class inspectSelection:
        period = 1
        tl = 2
        name = 3
        file = 4
        vTypes = 5
        timeThreshold = 6
        speedThreshold = 7
        jamThreshold = 8
        friendlyPos = 9
        parameters = 12

# E3


class E3:
    class create:
        id = 5
        period = 6
        name = 7
        file = 8
        vTypes = 9
        timeThreshold = 10
        speedThreshold = 11

    class inspect:
        id = 1
        pos = 2
        period = 3
        name = 4
        file = 5
        vTypes = 6
        timeThreshold = 7
        speedThreshold = 8
        parameters = 11

    class inspectSelection:
        period = 1
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
        id = 5
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
        id = 5
        pos = 6
        frequency = 7
        name = 8
        routeProbe = 9
        output = 10
        jamTreshold = 11
        vTypes = 12
        center = 16

    class inspect:
        id = 1
        edgeLane = 2
        pos = 3
        frequency = 4
        name = 5
        routeProbe = 6
        output = 7
        jamTreshold = 8
        vTypes = 9
        parameters = 12

    class inspectSelection:
        pos = 1
        frequency = 2
        name = 3
        routeProbe = 4
        output = 5
        jamTreshold = 6
        vTypes = 7
        parameters = 10

# routeProbe


class routeProbe:
    class create:
        id = 5
        frequency = 6
        name = 7
        file = 8
        begin = 9
        center = 13

    class inspect:
        id = 1
        edge = 2
        frequency = 3
        name = 4
        file = 5
        begin = 6
        parameters = 9

    class inspectSelection:
        frequency = 1
        name = 2
        file = 3
        begin = 4
        parameters = 7

# vaporizer


class vaporizer:
    class create:
        begin = 5
        end = 6
        name = 7
        center = 11

    class inspect:
        edge = 1
        begin = 2
        end = 3
        name = 4
        parameters = 7

    class inspectSelection:
        begin = 1
        end = 2
        name = 3
        parameters = 6

# --------------------------------
# SHAPE ELEMENTS
# --------------------------------

# Poly


class Poly:
    class create:
        id = 5
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
        colorButton = 3
        color = 4
        fill = 5
        lineWidth = 6
        layer = 7
        type = 8
        name = 9
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
        fill = 3
        lineWidth = 4
        layer = 5
        type = 6
        name = 7
        imgFile = 8
        reativePath = 9
        angle = 10
        geo = 13
        parameters = 15
        closeShape = 18

# POI


class POI:
    class create:
        id = 5
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
        colorButton = 3
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
        colorButton = 1
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
        id = 5
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
        lonLat = 20

    class inspect:
        id = 1
        lon = 2
        lat = 3
        colorButton = 4
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
        colorButton = 1
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
        id = 5
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
        parameters = 18

    class inspectSelection:
        friendlyPos = 1
        posLat = 2
        colorButton = 3
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

# --------------------------------
# TAZ ELEMENTS
# --------------------------------

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
        colorButton = 5
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

# --------------------------------
# DEMAND ELEMENTS
# --------------------------------

# type


class type:

    color = 6

    class buttons:
        create = 2
        delete = 3
        copy = 3
        dialog = 25

    class edit:
        id = 8
        vTypeDist = 9
        vClass = 10
        colorButton = 12
        color = 13
        length = 14
        minGap = 15
        maxSpeed = 16
        desiredMaxSpeed = 17
        accel = 18
        decel = 19
        sigma = 20
        tau = 21

    class editDefault:
        vTypeDist = 7
        vClass = 8
        colorButton = 10
        color = 11
        length = 12
        minGap = 13
        maxSpeed = 14
        desiredMaxSpeed = 15
        accel = 16
        decel = 17
        sigma = 18
        tau = 19

    class editDialog:
        vClass = 1
        id = 3
        colorButton = 4
        color = 5
        length = 6
        minGap = 7
        maxSpeed = 8
        desiredMaxSpeed = 9
        speedFactor = 10
        emissionClass = 11
        width = 13
        height = 14
        imgFileButton = 15
        imgFile = 16
        osgFileButton = 17
        osgFile = 18
        laneChangeModel = 19
        guiShape = 21
        probability = 23
        personCapacity = 24
        containerCapacity = 25
        boardingDuration = 26
        loadingDuration = 27
        latAlignment = 28
        minGapLat = 30
        maxSpeedLat = 31
        actionStepLength = 32
        carriageLength = 33
        locomotiveLength = 34
        carriageGap = 35
        parametersButton = 36
        parameters = 37

# route


class route:
    class create:
        id = 7
        colorButton = 8
        color = 9
        repeat = 10
        cycletime = 11

    class inspect:
        id = 1
        edges = 2
        colorButton = 3
        color = 4
        repeat = 5
        cycletime = 6
        parameters = 9

    class inspectSelection:
        colorButton = 1
        color = 2
        repeat = 3
        cycletime = 4
        parameters = 7

# trip (from-to edges)


class trip:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        depart = 23

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        fromEdge = 5
        toEdge = 6
        via = 7
        colorButton = 8
        color = 9
        departLane = 10
        departPos = 11
        departSpeed = 12
        arrivalLane = 13
        arrivalPos = 14
        arrivalSpeed = 15
        line = 16
        personNumber = 17
        containerNumber = 18
        departPosLat = 19
        arrivalPosLat = 20
        insertionChecks = 21
        depart = 22
        parameters = 25

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        depart = 16
        parameters = 19

    class transform:
        vehicle = 1
        vehicleEmbedded = 2
        routeFlow = 3
        routeFlowEmbedded = 4
        flow = 5

    transformTo = 9

# trip (from-to junctions)


class tripFromToJunction:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        depart = 23

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        fromJunction = 5
        toJunction = 6
        colorButton = 7
        color = 8
        departLane = 9
        departPos = 10
        departSpeed = 11
        arrivalLane = 12
        arrivalPos = 13
        arrivalSpeed = 14
        line = 15
        personNumber = 16
        containerNumber = 17
        departPosLat = 18
        arrivalPosLat = 19
        insertionChecks = 20
        depart = 21
        parameters = 24

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        depart = 16
        parameters = 19

    class transform:
        vehicle = 1
        vehicleEmbedded = 2
        routeFlow = 3
        routeFlowEmbedded = 4
        trip = 5
        flow = 6

    transformTo = 9

# vehicle (over route)


class vehicle:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        depart = 23

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        route = 5
        departEdge = 6
        arrivalEdge = 7
        colorButton = 8
        color = 9
        departLane = 10
        departPos = 11
        departSpeed = 12
        arrivalLane = 13
        arrivalPos = 14
        arrivalSpeed = 15
        line = 16
        personNumber = 17
        containerNumber = 18
        departPosLat = 19
        arrivalPosLat = 20
        insertionChecks = 21
        depart = 22
        parameters = 25

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        depart = 16
        parameters = 19

    class transform:
        vehicleEmbedded = 1
        routeFlow = 2
        routeFlowEmbedded = 3
        trip = 4
        flow = 5

    transformTo = 9

# vehicle (embedded route)


class vehicleEmbedded:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        depart = 23

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        departEdge = 5
        arrivalEdge = 6
        colorButton = 7
        color = 8
        departLane = 9
        departPos = 10
        departSpeed = 11
        arrivalLane = 12
        arrivalPos = 13
        arrivalSpeed = 14
        line = 15
        personNumber = 16
        containerNumber = 17
        departPosLat = 18
        arrivalPosLat = 19
        insertionChecks = 20
        depart = 21
        parameters = 24

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        depart = 16
        parameters = 19

    class transform:
        vehicle = 1
        routeFlow = 2
        routeFlowEmbedded = 3
        trip = 4
        flow = 5

    transformTo = 9

# flow (from-to edges)


class flow:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        begin = 23
        terminate = 27
        end = 29
        number = 30
        spacing = 29
        terminateOption = 31
        spacingOption = 32

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        fromEdge = 5
        toEdge = 6
        via = 7
        colorButton = 8
        color = 9
        departLane = 10
        departPos = 11
        departSpeed = 12
        arrivalLane = 13
        arrivalPos = 14
        arrivalSpeed = 15
        line = 16
        personNumber = 17
        containerNumber = 18
        departPosLat = 19
        arrivalPosLat = 20
        insertionChecks = 21
        begin = 22
        terminate = 25
        spacing = 27
        end = 28
        terminateOption = 29
        spacingOption = 30
        parameters = 32

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        begin = 16
        terminate = 19
        spacing = 21
        terminateOption = 23
        spacingOption = 24
        parameters = 24

    class transform:
        vehicle = 1
        vehicleEmbedded = 2
        routeFlow = 3
        routeFlowEmbedded = 4
        trip = 5

    transformTo = 9

# flow (from-to junctions)


class flowJunction:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        begin = 23
        terminate = 27
        end = 29
        number = 30
        spacing = 29
        terminateOption = 31
        spacingOption = 32

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        fromJunction = 5
        toJunction = 6
        colorButton = 7
        color = 8
        departLane = 9
        departPos = 10
        departSpeed = 11
        arrivalLane = 12
        arrivalPos = 13
        arrivalSpeed = 14
        line = 15
        personNumber = 16
        containerNumber = 17
        departPosLat = 18
        arrivalPosLat = 19
        insertionChecks = 20
        begin = 21
        terminate = 24
        spacing = 26
        end = 27
        terminateOption = 28
        spacingOption = 29
        parameters = 31

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        begin = 16
        terminate = 19
        spacing = 21
        terminateOption = 23
        spacingOption = 24
        parameters = 24

    class transform:
        vehicle = 1
        vehicleEmbedded = 2
        routeFlow = 3
        routeFlowEmbedded = 4
        trip = 5
        flow = 6

    transformTo = 9

# flow (over route)


class routeFlow:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        begin = 23
        terminate = 27
        end = 29
        number = 30
        spacing = 29
        terminateOption = 31
        spacingOption = 32

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        route = 5
        departEdge = 6
        arrivalEdge = 7
        colorButton = 8
        color = 9
        departLane = 10
        departPos = 11
        departSpeed = 12
        arrivalLane = 13
        arrivalPos = 14
        arrivalSpeed = 15
        line = 16
        personNumber = 17
        containerNumber = 18
        departPosLat = 19
        arrivalPosLat = 20
        insertionChecks = 21
        begin = 22
        terminate = 25
        spacing = 27
        end = 28
        terminateOption = 29
        spacingOption = 30
        parameters = 32

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        begin = 16
        terminate = 19
        spacing = 21
        terminateOption = 23
        spacingOption = 24
        parameters = 24

    class transform:
        vehicle = 1
        vehicleEmbedded = 2
        routeFlowEmbedded = 3
        trip = 4
        flow = 5

    transformTo = 9

# flow (embedded route)


class flowEmbedded:
    class create:
        type = 5
        id = 8
        colorButton = 9
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
        insertionChecks = 22
        begin = 23
        terminate = 27
        end = 29
        number = 30
        spacing = 29
        terminateOption = 31
        spacingOption = 32

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        departEdge = 5
        arrivalEdge = 6
        colorButton = 7
        color = 8
        departLane = 9
        departPos = 10
        departSpeed = 11
        arrivalLane = 12
        arrivalPos = 13
        arrivalSpeed = 14
        line = 15
        personNumber = 16
        containerNumber = 17
        departPosLat = 18
        arrivalPosLat = 19
        insertionChecks = 20
        begin = 21
        terminate = 24
        spacing = 26
        end = 27
        terminateOption = 28
        spacingOption = 29
        parameters = 31

    class inspectSelection:
        type = 1
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
        insertionChecks = 15
        begin = 16
        terminate = 19
        spacing = 21
        terminateOption = 23
        spacingOption = 24
        parameters = 24

    class transform:
        vehicle = 1
        vehicleEmbedded = 2
        routeFlow = 3
        trip = 4
        flow = 5

    transformTo = 9

# stop over bus stop


class stopBusStop:
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
        actType = 17
        tripID = 18
        line = 19
        onDemand = 20

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
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        parking = 11
        actType = 13
        tripID = 14
        line = 15
        onDemand = 16

# stop over container stop


class stopContainerStop:
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
        actType = 17
        tripID = 18
        line = 19
        onDemand = 20

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
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        parking = 11
        actType = 13
        tripID = 14
        line = 15
        onDemand = 16

# stop over charging station


class stopChargingStation:
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
        actType = 17
        tripID = 18
        line = 19
        onDemand = 20

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
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        parking = 11
        actType = 13
        tripID = 14
        line = 15
        onDemand = 16

# stop over parkings


class stopParking:
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
        actType = 15
        tripID = 16
        line = 17
        onDemand = 18

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
        actType = 8
        tripID = 9
        line = 10
        onDemand = 11

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        actType = 11
        tripID = 12
        line = 13
        onDemand = 14

# stop over lane


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
        expected = 16
        permitted = 16
        parking = 17
        actType = 19
        tripID = 20
        line = 21
        onDemand = 22

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
        actType = 15
        tripID = 16
        line = 17
        onDemand = 18

    class inspectSelection:
        friendlyPos = 1
        posLat = 2
        durationEnable = 3
        duration = 4
        untilEnable = 5
        until = 6
        extensionEnable = 7
        extension = 8
        triggered = 9
        expected = 11
        permitted = 12
        parking = 13
        actType = 15
        tripID = 16
        line = 17
        onDemand = 18

# waypoint over bus stops


class waypointBusStop:
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
        actType = 17
        tripID = 18
        line = 19
        speed = 20

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
        actType = 10
        tripID = 11
        line = 12
        speed = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        parking = 11
        actType = 13
        tripID = 14
        line = 15
        speed = 16

# waypoint over container stops


class waypointContainerStop:
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
        actType = 17
        tripID = 18
        line = 19
        speed = 20

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
        actType = 10
        tripID = 11
        line = 12
        speed = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        parking = 11
        actType = 13
        tripID = 14
        line = 15
        speed = 16

# waypoint over charging stations


class waypointChargingStation:
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
        actType = 17
        tripID = 18
        line = 19
        speed = 20

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
        actType = 10
        tripID = 11
        line = 12
        speed = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        parking = 11
        actType = 13
        tripID = 14
        line = 15
        speed = 16

# waypoint over parkings


class waypointParking:
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
        actType = 15
        tripID = 16
        line = 17
        speed = 18

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
        actType = 8
        tripID = 9
        line = 10
        speed = 11

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        permitted = 10
        actType = 11
        tripID = 12
        line = 13
        speed = 14

# waypoint (over lane)


class waypointLane:
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
        expected = 16
        permitted = 16
        parking = 17
        actType = 19
        tripID = 20
        line = 21
        speed = 22

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
        actType = 15
        tripID = 16
        line = 17
        speed = 18

    class inspectSelection:
        friendlyPos = 1
        posLat = 2
        durationEnable = 3
        duration = 4
        untilEnable = 5
        until = 6
        extensionEnable = 7
        extension = 8
        triggered = 9
        expected = 11
        permitted = 12
        parking = 13
        actType = 15
        tripID = 16
        line = 17
        speed = 18

# Person


class person:
    class create:
        id = 8
        colorButton = 9
        color = 10
        departPos = 11
        depart = 12

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        colorButton = 5
        color = 6
        departPos = 7
        depart = 8
        parameters = 11

    class inspectSelection:
        type = 1
        colorButton = 2
        color = 3
        departPos = 4
        depart = 5
        parameters = 8

    transformTo = 8


class personFlow:
    class create:
        id = 8
        colorButton = 9
        color = 10
        departPos = 11
        begin = 12
        terminate = 16
        end = 18
        number = 19
        spacing = 18
        terminateOption = 20
        spacingOption = 21

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        colorButton = 5
        color = 6
        departPos = 7
        begin = 8
        terminate = 11
        spacing = 13
        end = 14
        terminateOption = 15
        spacingOption = 16
        parameters = 18

    class inspectSelection:
        type = 1
        colorButton = 2
        color = 3
        departPos = 4
        begin = 5
        terminate = 8
        spacing = 10
        terminateOption = 12
        spacingOption = 13
        parameters = 15

    transformTo = 8

# PersonPlans


class personTripEdgeEdge:
    class create:
        arrivalPos = 8
        vTypes = 9
        modes = 10
        lines = 11

    class inspect:
        to = 1
        arrivalPos = 2
        vTypes = 3
        modes = 4
        lines = 5

    class inspectSelection:
        arrivalPos = 1
        vTypes = 2
        modes = 3
        lines = 4


class personTripEdgeBusStop:
    class create:
        vTypes = 8
        modes = 9
        lines = 10

    class inspect:
        to = 1
        vTypes = 2
        modes = 3
        lines = 4

    class inspectSelection:
        vTypes = 1
        modes = 2
        lines = 3


class personTripJunctions:
    class create:
        vTypes = 8
        modes = 9
        lines = 10

    class inspect:
        to = 1
        vTypes = 2
        modes = 3
        lines = 4

    class inspectSelection:
        vTypes = 1
        modes = 2
        lines = 3


class rideEdgeEdge:
    class create:
        arrivalPos = 8
        lines = 9

    class inspect:
        to = 1
        arrivalPos = 2
        lines = 3

    class inspectSelection:
        arrivalPos = 1
        lines = 2


class rideEdgeBusStop:
    class create:
        lines = 8

    class inspect:
        to = 1
        lines = 2

    class inspectSelection:
        lines = 1


class walkEdgeEdge:
    class create:
        arrivalPos = 8

    class inspect:
        to = 1
        arrivalPos = 2

    class inspectSelection:
        arrivalPos = 1


class walkEdges:
    class create:
        arrivalPos = 8

    class inspect:
        edges = 1
        arrivalPos = 2

    class inspectSelection:
        arrivalPos = 1


class walkRoute:
    class create:
        arrivalPos = 8

    class inspect:
        route = 1
        arrivalPos = 2

    class inspectSelection:
        arrivalPos = 1


class walkEdgeBusStop:
    class inspect:
        to = 1


class walkJunctions:
    class inspect:
        to = 1


class personStopEdge:
    class create:
        friendlyPos = 8
        durationEnable = 9
        duration = 10
        untilEnable = 11
        until = 12
        actType = 12

    class inspect:
        edge = 1
        endPos = 2
        friendlyPos = 3
        durationEnable = 4
        duration = 5
        untilEnable = 6
        until = 7
        actType = 7

    class inspectSelection:
        friendlyPos = 1
        durationEnable = 2
        duration = 3
        untilEnable = 4
        until = 5
        actType = 6


class personStopBusStop:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 10
        until = 11
        actType = 11

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        actType = 4

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        actType = 4

# Container


class container:
    class create:
        id = 8
        colorButton = 9
        color = 10
        depart = 11

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        colorButton = 5
        color = 6
        depart = 7
        parameters = 10

    class inspectSelection:
        type = 1
        colorButton = 2
        color = 3
        depart = 4
        parameters = 7

    transformTo = 8


class containerFlow:
    class create:
        id = 8
        colorButton = 9
        color = 10
        begin = 11
        terminate = 15
        end = 17
        number = 18
        spacing = 17
        terminateOption = 19
        spacingOption = 20

    class inspect:
        id = 1
        typeButton = 2
        type = 3
        colorButton = 5
        color = 6
        begin = 7
        terminate = 10
        spacing = 12
        end = 13
        terminateOption = 14
        spacingOption = 15
        parameters = 17

    class inspectSelection:
        type = 1
        colorButton = 2
        color = 3
        begin = 4
        terminate = 7
        spacing = 9
        terminateOption = 11
        spacingOption = 12
        parameters = 14

    transformTo = 8

# ContainerPlans


class transportEdgeEdge:
    class create:
        arrivalPos = 8
        lines = 9

    class inspect:
        to = 1
        arrivalPos = 2
        lines = 3

    class inspectSelection:
        arrivalPos = 1
        lines = 2


class transportEdgeContainerStop:
    class create:
        lines = 8

    class inspect:
        to = 1
        lines = 2

    class inspectSelection:
        lines = 1


class transhipEdgeEdge:
    class create:
        departPos = 8
        arrivalPos = 9
        speed = 9

    class inspect:
        to = 1
        departPos = 2
        arrivalPos = 3
        speed = 4

    class inspectSelection:
        departPos = 1
        arrivalPos = 2
        speed = 3


class transhipEdgeContainerStop:
    class create:
        departPos = 8
        speed = 9

    class inspect:
        to = 1
        departPos = 2
        speed = 3

    class inspectSelection:
        departPos = 1
        speed = 2


class transhipEdges:
    class create:
        departPos = 8
        arrivalPos = 9
        speed = 9

    class inspect:
        edges = 1
        departPos = 2
        arrivalPos = 3
        speed = 4

    class inspectSelection:
        departPos = 1
        arrivalPos = 2
        speed = 3


class containerStopEdge:
    class create:
        friendlyPos = 8
        durationEnable = 9
        duration = 10
        untilEnable = 11
        until = 12
        actType = 12

    class inspect:
        edge = 1
        endPos = 2
        friendlyPos = 3
        durationEnable = 4
        duration = 5
        untilEnable = 6
        until = 7
        actType = 7

    class inspectSelection:
        friendlyPos = 1
        durationEnable = 2
        duration = 3
        untilEnable = 4
        until = 5
        actType = 6


class containerStopContainerStop:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 10
        until = 11
        actType = 11

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        actType = 4

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        actType = 4

# --------------------------------
# DATA ELEMENTS
# --------------------------------

# edgeData


class edgeData:
    class create:
        parameters = 12

    class inspect:
        parameters = 3

    class inspectSelection:
        parameters = 3

# edgeRelData


class edgeRelData:
    class create:
        parameters = 12

    class inspect:
        fromEdge = 1
        toEdge = 2
        parameters = 5

    class inspectSelection:
        parameters = 3

# TAZRelData


class TAZRelData:
    class create:
        parameters = 12

    class inspect:
        fromTAZ = 1
        toTAZ = 2
        parameters = 5

    class inspectSelection:
        parameters = 3
