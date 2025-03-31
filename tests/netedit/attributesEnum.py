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

# @file    attributesEnum.py
# @author  Pablo Alvarez Lopez
# @date    2022-01-31

# --------------------------------
# GENERAL
# --------------------------------

class toolbar:
    class file:
        newNetwork = 1
        newWindow = 2
        generateNetwork = 3
        loadNeteditConfig = 4
        loadSumoConfig = 5
        loadNetwork = 6
        openNetconvertConfiguration = 7
        importForeignNetwork = 8
        saveNetwork = 9
        saveNetworkAs = 9
        savePlainXML = 10
        saveJoinedJunctions = 11
        reloadConfig = 12
        reloadNetwork = 13

        class neteditConfig:
            menu = 14
            save = 1
            saveAs = 2

        class sumoConfig:
            menu = 15
            save = 1
            saveAs = 2

        class trafficLights:
            menu = 16
            load = 1
            save = 2
            saveAs = 3
            reload = 4

        class edgeTypes:
            menu = 17
            load = 1
            save = 2
            saveAs = 3
            reload = 4

        class aditionalElements:
            menu = 18
            load = 1
            save = 2
            saveAs = 2
            saveJupedsim = 3
            reload = 4

        class demandElements:
            menu = 19
            load = 1
            save = 2
            saveAs = 2
            reload = 3

        class dataElements:
            menu = 20
            load = 1
            save = 2
            saveAs = 2
            reload = 3

        class meanDataElements:
            menu = 21
            load = 1
            save = 2
            saveAs = 2
            reload = 3


class editElements:
    overlapped = 5


class move:
    moveWholePolygon = 5


class selection:
    class lockSelectionNetwork:
        junctions = 1
        edges = 2
        lanes = 3
        connections = 4
        crossings = 5
        walkingAreas = 6
        additionals = 7
        tazs = 8
        wires = 9
        polygons = 10
        pois = 11
        walkableAreas = 12
        obstacles = 13
        selected = 14
        lockAll = 15
        unlockAll = 16


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
    # select mode
        automaticSelectJunctions = '9'

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
        parameters = 14
        parametersButton = 15
        frontButton = 17
        helpDialog = 18
        reset = 19

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
        parameters = 19
        parametersButton = 20
        frontButton = 22
        helpDialog = 23
        reset = 24

    class inspectSelection:
        type = 1
        radius = 2
        keepClear = 3
        rightOfWay = 4
        fringe = 5
        name = 6
        parameters = 8
        parametersButton = 9
        frontButton = 11
        helpDialog = 12
        reset = 13

    class inspectSelectionTLS:
        type = 1
        radius = 2
        keepClear = 3
        rightOfWay = 4
        fringe = 5
        name = 6
        tlType = 7
        tlLayout = 8
        tl = 10
        parameters = 13
        parametersButton = 14
        frontButton = 16
        helpDialog = 17
        reset = 18

# edge


class edge:
    class create:
        defaultShort = 3
        disallowPedestrians = 4
        addSidewalk = 5
        addBikelane = 6
        useTemplate = 7
        numLanes = 9
        speed = 10
        allowButton = 10
        allow = 12
        disallow = 13
        spreadType = 14
        priority = 16
        width = 17
        sidewalkWidth = 18
        bikelaneWidth = 19
        parameters = 21
        parametersButton = 22

    class inspect:
        id = 1
        fromEdge = 2
        toEdge = 3
        speed = 4
        priority = 5
        numLanes = 6
        type = 7
        allowButton = 8
        allow = 9
        disallow = 10
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
        stopOffsetExceptionButton = 22
        stopOffsetException = 23
        parameters = 25
        parametersButton = 26
        frontButton = 28
        helpDialog = 29
        reset = 30

    class inspectSelection:
        speed = 1
        priority = 2
        numLanes = 3
        type = 4
        allowButton = 5
        allow = 6
        disallow = 7
        length = 8
        spreadType = 9
        name = 10
        width = 11
        endOffset = 12
        shapeStart = 13
        shapeEnd = 14
        stopOffset = 15
        stopOffsetExceptionButton = 16
        stopOffsetException = 17
        parameters = 19
        parametersButton = 20
        frontButton = 22
        helpDialog = 23
        reset = 24

    class template:
        create = 32
        copy = 33
        clear = 34

# lane


class lane:
    class create:
        add = 26
        remove = 27
        speed = 28
        allowButton = 29
        allow = 30
        disallow = 31
        width = 32
        parameters = 33
        parametersButton = 34

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
        parameters = 18
        parametersButton = 19
        frontButton = 21
        helpDialog = 22
        reset = 23

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
        parameters = 17
        parametersButton = 18
        frontButton = 20
        helpDialog = 21
        reset = 22

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
        parameters = 19
        parametersButton = 20
        frontButton = 22
        helpDialog = 23
        reset = 24

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
        parameters = 21
        parametersButton = 22
        frontButton = 24
        helpDialog = 25
        reset = 26

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
        parameters = 18
        parametersButton = 19
        frontButton = 21
        helpDialog = 22
        reset = 23

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
        parameters = 20
        parametersButton = 21
        frontButton = 23
        helpDialog = 24
        reset = 25

    saveConnections = 4

# crossing


class crossing:
    class create:
        edges = 2
        priority = 3
        width = 4
        button = 10

    class createTLS:
        edges = 2
        width = 3
        button = 9

    class inspect:
        edges = 1
        priority = 2
        width = 3
        customShape = 4
        parameters = 6
        parametersButton = 7
        frontButton = 9
        helpDialog = 10
        reset = 11

    class inspectTLS:
        edges = 1
        priority = 2
        width = 3
        linkIndex = 4
        linkIndex2 = 5
        customShape = 6
        parameters = 8
        parametersButton = 9
        frontButton = 11
        helpDialog = 12
        reset = 13

    class inspectSelection:
        priority = 1
        width = 2
        parameters = 4
        parametersButton = 5
        frontButton = 7
        helpDialog = 8
        reset = 9

    class inspectSelectionTLS:
        priority = 1
        width = 2
        linkIndex = 3
        linkIndex2 = 4
        parameters = 6
        parametersButton = 7
        frontButton = 9
        helpDialog = 10
        reset = 11

    clearEdges = 3
    useSelectedEdges = 3
    firstField = 3


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
            numPhases = 4
            dur = 17 + (0 * numPhases) + 0
            state = 17 + (1 * numPhases) + 1
            nxt = 17 + (2 * numPhases) + 2
            name = 17 + (3 * numPhases) + 3
            addButton = 17 + (4 * numPhases) + 1
            deleteButton = 17 + (5 * numPhases) + 2
            moveUp = 17 + (6 * numPhases) + 3
            moveDown = 17 + (7 * numPhases) + 3
            cleanStates = 17 + (8 * numPhases) + 1
            groupSignal = 17 + (8 * numPhases) + 2
            addStates = 17 + (8 * numPhases) + 3
            ungroupSignal = 17 + (8 * numPhases) + 4

        class actuatedPhase:
            numPhases = 4
            dur = 18 + (0 * numPhases) + 0
            minD = 18 + (1 * numPhases) + 1
            maxD = 18 + (2 * numPhases) + 2
            state = 18 + (3 * numPhases) + 3
            earlyEnd = 18 + (4 * numPhases) + 0
            latestEnd = 18 + (5 * numPhases) + 1
            nxt = 18 + (6 * numPhases) + 2
            name = 18 + (7 * numPhases) + 3
            addButton = 18 + (8 * numPhases) + 1
            deleteButton = 18 + (9 * numPhases) + 2
            moveUp = 18 + (10 * numPhases) + 3
            moveDown = 18 + (11 * numPhases) + 4
            cleanStates = 18 + (12 * numPhases) + 1
            groupSignal = 18 + (12 * numPhases) + 2
            addStates = 18 + (12 * numPhases) + 3
            ungroupSignal = 18 + (12 * numPhases) + 4

        class delayBasedPhase:
            numPhases = 4
            dur = 18 + (0 * numPhases) + 0
            minD = 18 + (1 * numPhases) + 1
            maxD = 18 + (2 * numPhases) + 2
            state = 18 + (3 * numPhases) + 3
            nxt = 18 + (4 * numPhases) + 0
            name = 18 + (5 * numPhases) + 1
            addButton = 18 + (6 * numPhases) + 2
            deleteButton = 18 + (7 * numPhases) + 3
            moveUp = 18 + (8 * numPhases) + 0
            moveDown = 18 + (9 * numPhases) + 1
            cleanStates = 18 + (10 * numPhases) + 1
            groupSignal = 18 + (10 * numPhases) + 2
            addStates = 18 + (10 * numPhases) + 3
            ungroupSignal = 18 + (10 * numPhases) + 4

        class NEMAPhase:
            numPhases = 4
            dur = 18 + (0 * numPhases) + 0
            minD = 18 + (1 * numPhases) + 1
            maxD = 18 + (2 * numPhases) + 2
            state = 18 + (3 * numPhases) + 3
            vehext = 18 + (4 * numPhases) + 0
            yellow = 18 + (5 * numPhases) + 1
            red = 18 + (6 * numPhases) + 2
            nxt = 18 + (7 * numPhases) + 3
            name = 18 + (8 * numPhases) + 0
            addButton = 18 + (9 * numPhases) + 1
            deleteButton = 18 + (10 * numPhases) + 2
            moveUp = 18 + (11 * numPhases) + 3
            moveDown = 18 + (12 * numPhases) + 0
            cleanStates = 18 + (13 * numPhases) + 1
            groupSignal = 18 + (13 * numPhases) + 2
            addStates = 18 + (13 * numPhases) + 3
            ungroupSignal = 18 + (13 * numPhases) + 4

    class join:
        class attributes:
            offset = 13
            parameters = 15

        class staticPhase:
            numPhases = 2
            dur = 18 + (0 * numPhases)
            state = 18 + (1 * numPhases)
            nxt = 18 + (2 * numPhases)
            name = 18 + (3 * numPhases)
            addButton = 18 + (5 * numPhases)
            deleteButton = 18 + (6 * numPhases)
            moveUp = 18 + (7 * numPhases)
            moveDown = 18 + (8 * numPhases)
            cleanStates = 18 + (8 * numPhases) + 1
            groupSignal = 18 + (8 * numPhases) + 2
            addStates = 18 + (8 * numPhases) + 3
            ungroupSignal = 18 + (8 * numPhases) + 4

        class actuatedPhase:
            numPhases = 2
            dur = 19 + (0 * numPhases)
            minD = 19 + (1 * numPhases)
            maxD = 19 + (2 * numPhases)
            state = 19 + (3 * numPhases)
            earlyEnd = 19 + (4 * numPhases)
            latestEnd = 19 + (5 * numPhases)
            nxt = 19 + (6 * numPhases)
            name = 19 + (7 * numPhases)
            addButton = 19 + (9 * numPhases)
            deleteButton = 19 + (10 * numPhases)
            moveUp = 19 + (11 * numPhases)
            moveDown = 19 + (12 * numPhases)
            cleanStates = 19 + (12 * numPhases) + 1
            groupSignal = 19 + (12 * numPhases) + 2
            addStates = 19 + (12 * numPhases) + 3
            ungroupSignal = 19 + (12 * numPhases) + 4

        class delayBasedPhase:
            numPhases = 2
            dur = 19 + (0 * numPhases)
            minD = 19 + (1 * numPhases)
            maxD = 19 + (2 * numPhases)
            state = 19 + (3 * numPhases)
            nxt = 19 + (4 * numPhases)
            name = 19 + (5 * numPhases)
            addButton = 19 + (7 * numPhases)
            deleteButton = 19 + (8 * numPhases)
            moveUp = 19 + (9 * numPhases)
            moveDown = 19 + (10 * numPhases)
            cleanStates = 19 + (10 * numPhases) + 1
            groupSignal = 19 + (10 * numPhases) + 2
            addStates = 19 + (10 * numPhases) + 3
            ungroupSignal = 19 + (10 * numPhases) + 4

        class NEMAPhase:
            numPhases = 2
            dur = 19 + (0 * numPhases)
            minD = 19 + (1 * numPhases)
            maxD = 19 + (2 * numPhases)
            state = 19 + (3 * numPhases)
            vehext = 19 + (4 * numPhases)
            yellow = 19 + (5 * numPhases)
            red = 19 + (6 * numPhases)
            nxt = 19 + (7 * numPhases)
            name = 19 + (8 * numPhases)
            addButton = 20 + (9 * numPhases)
            deleteButton = 20 + (10 * numPhases)
            moveUp = 20 + (11 * numPhases)
            moveDown = 20 + (12 * numPhases)
            cleanStates = 20 + (13 * numPhases) + 1
            groupSignal = 20 + (13 * numPhases) + 2
            addStates = 20 + (13 * numPhases) + 3
            ungroupSignal = 20 + (13 * numPhases) + 4

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
        friendlyPos = 6
        name = 7
        colorButton = 8
        color = 9
        lines = 10
        personCapacity = 11
        parkingLength = 12
        parameters = 14
        parametersButton = 15
        size = 17
        forceSize = 18
        reference = 19
        additionalFileButton = 21
        additionalFile = 22
        helpDialog = 23
        reset = 24

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        startPos = 4
        endPos = 5
        friendlyPos = 6
        name = 7
        colorButton = 8
        color = 9
        lines = 10
        personCapacity = 11
        parkingLength = 12
        parameters = 14
        parametersButton = 15
        frontButton = 17
        size = 18
        forceSize = 19
        reference = 20
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

    class inspectSelection:
        friendlyPos = 1
        name = 2
        colorButton = 3
        color = 4
        lines = 5
        personCapacity = 6
        parkingLength = 7
        parameters = 9
        parametersButton = 10
        frontButton = 12
        size = 13
        forceSize = 14
        reference = 15
        additionalFileButton = 17
        additionalFile = 18
        helpDialog = 19
        reset = 20


# trainStop


class trainStop:
    class create:
        id = 5
        friendlyPos = 6
        name = 7
        colorButton = 8
        color = 9
        lines = 10
        personCapacity = 11
        parkingLength = 12
        parameters = 14
        parametersButton = 15
        size = 17
        forceSize = 18
        reference = 19
        additionalFileButton = 21
        additionalFile = 22
        helpDialog = 23
        reset = 24

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        startPos = 4
        endPos = 5
        friendlyPos = 6
        name = 7
        colorButton = 8
        color = 9
        lines = 10
        personCapacity = 11
        parkingLength = 12
        parameters = 14
        parametersButton = 15
        frontButton = 17
        size = 18
        forceSize = 19
        reference = 20
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

    class inspectSelection:
        friendlyPos = 1
        name = 2
        colorButton = 3
        color = 4
        lines = 5
        personCapacity = 6
        parkingLength = 7
        parameters = 9
        parametersButton = 10
        frontButton = 12
        size = 13
        forceSize = 14
        reference = 15
        additionalFileButton = 17
        additionalFile = 18
        helpDialog = 19
        reset = 20

# containerStop


class containerStop:
    class create:
        id = 5
        friendlyPos = 6
        name = 7
        colorButton = 8
        color = 9
        lines = 10
        containerCapacity = 11
        parkingLength = 12
        parameters = 14
        parametersButton = 15
        size = 17
        forceSize = 18
        reference = 19
        additionalFileButton = 21
        additionalFile = 22
        helpDialog = 23
        reset = 24

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        startPos = 4
        endPos = 5
        friendlyPos = 6
        name = 7
        colorButton = 8
        color = 9
        lines = 10
        containerCapacity = 11
        parkingLength = 12
        parameters = 14
        parametersButton = 15
        frontButton = 17
        size = 18
        forceSize = 19
        reference = 20
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

    class inspectSelection:
        friendlyPos = 1
        name = 2
        colorButton = 3
        color = 4
        lines = 5
        containerCapacity = 6
        parkingLength = 7
        parameters = 9
        parametersButton = 10
        frontButton = 12
        size = 13
        forceSize = 14
        reference = 15
        additionalFileButton = 17
        additionalFile = 18
        helpDialog = 19
        reset = 20

# until here

# chargingStation


class chargingStation:
    class create:
        id = 5
        friendlyPos = 6
        name = 7
        power = 8
        efficiency = 9
        chargeInTransit = 10
        chargeDelay = 11
        chargeType = 12
        waitingTime = 14
        parkingArea = 15
        parameters = 17
        parametersButton = 18
        size = 20
        forceSize = 21
        reference = 22
        additionalFileButton = 24
        additionalFile = 25
        helpDialog = 26
        reset = 27

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        startPos = 4
        endPos = 5
        name = 6
        friendlyPos = 7
        power = 8
        efficiency = 9
        chargeInTransit = 10
        chargeDelay = 11
        chargeType = 12
        waitingTime = 14
        parkingArea = 15
        parameters = 17
        parametersButton = 18
        frontButton = 20
        forceSize = 21
        reference = 22
        additionalFileButton = 24
        additionalFile = 25
        helpDialog = 26
        reset = 27

    class inspectSelection:
        name = 1
        friendlyPos = 2
        power = 3
        efficiency = 4
        chargeInTrainsit = 5
        chargeDelay = 6
        chargeType = 7
        waitingTime = 8
        parkingArea = 9
        parameters = 11
        parametersButton = 12
        frontButton = 14
        forceSize = 15
        reference = 16
        additionalFileButton = 18
        additionalFile = 19
        helpDialog = 20
        reset = 21

# access


class access:
    class create:
        friendlyPos = 5
        length = 6
        parameters = 8
        parent = 10
        helpDialog = 12
        reset = 13

    class inspect:
        lane = 1
        moveLane = 2
        pos = 3
        friendlyPos = 4
        length = 5
        parameters = 7
        parametersButton = 8
        frontButton = 10
        reparentButton = 11
        reparent = 12
        helpDialog = 13
        reset = 14

    class inspectSelection:
        friendlyPos = 1
        length = 2
        parameters = 4
        parametersButton = 5
        frontButton = 7
        reparentButton = 8
        reparent = 9
        helpDialog = 10
        reset = 11

# parkingArea


class parkingArea:
    class create:
        id = 5
        friendlyPos = 6
        name = 7
        departPos = 8
        acceptedBadges = 9
        roadSideCapacity = 10
        onRoad = 11
        width = 12
        length = 13
        angle = 14
        lefthand = 15
        parameters = 17
        parametersButton = 18
        size = 20
        forceSize = 21
        reference = 22
        additionalFileButton = 24
        additionalFile = 25
        helpDialog = 26
        reset = 27

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        startPos = 4
        endPos = 5
        friendlyPos = 6
        name = 7
        departPos = 8
        acceptedBadges = 9
        roadSideCapacity = 10
        onRoad = 11
        width = 12
        length = 13
        angle = 14
        lefthand = 15
        parameters = 17
        parametersButton = 18
        frontButton = 20
        size = 21
        forceSize = 22
        reference = 23
        additionalFileButton = 25
        additionalFile = 26
        helpDialog = 27
        reset = 28

    class inspectSelection:
        friendlyPos = 1
        name = 2
        departPos = 3
        acceptedBadges = 4
        roadSideCapacity = 5
        onRoad = 6
        width = 7
        length = 8
        angle = 9
        lefthand = 10
        parameters = 12
        parametersButton = 13
        frontButton = 15
        size = 16
        forceSize = 17
        reference = 18
        additionalFileButton = 20
        additionalFile = 21
        helpDialog = 22
        reset = 23


# parkingSpace


class parkingSpace:
    class create:
        name = 5
        width = 6
        length = 7
        angle = 8
        slope = 9
        parameters = 11
        parametersButton = 12
        parent = 14
        helpDialog = 16
        reset = 17

    class inspect:
        pos = 1
        name = 2
        width = 3
        length = 4
        angle = 5
        slope = 6
        parameters = 8
        parametersButton = 9
        frontButton = 11
        reparentButton = 12
        reparentButton = 13
        helpDialog = 14
        reset = 15

    class inspectSelection:
        name = 1
        width = 2
        length = 3
        angle = 4
        slope = 5
        parameters = 7
        parametersButton = 8
        frontButton = 10
        reparentButton = 11
        reparentButton = 12
        helpDialog = 13
        reset = 14

# E1


class E1:
    class create:
        id = 5
        friendlyPos = 6
        name = 7
        period = 8
        fileButton = 9
        file = 10
        vTypes = 11
        nextEdges = 12
        detectPersons = 13
        parameters = 16
        parametersButton = 17
        additionalFileButton = 19
        additionalFile = 20
        helpDialog = 21
        reset = 22

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        pos = 4
        friendlyPos = 5
        name = 6
        period = 7
        fileButton = 8
        file = 9
        vTypes = 10
        nextEdges = 11
        detectPersons = 12
        parameters = 15
        parametersButton = 16
        frontButton = 18
        additionalFileButton = 19
        additionalFile = 20
        helpDialog = 21
        reset = 22

    class inspectSelection:
        friendlyPos = 1
        name = 2
        period = 3
        fileButton = 4
        file = 5
        vTypes = 6
        nextEdges = 7
        detectPersons = 8
        parameters = 10
        parametersButton = 11
        frontButton = 13
        additionalFileButton = 14
        additionalFile = 15
        helpDialog = 16
        reset = 17

# E2


class E2:
    class create:
        id = 5
        friendlyPos = 6
        length = 7
        name = 8
        period = 9
        tl = 10
        fileButton = 11
        file = 12
        vTypes = 13
        nextEdges = 14
        detectPersons = 15
        timeThreshold = 17
        speedThreshold = 18
        jamThreshold = 19
        show = 20
        parameters = 22
        parametersButton = 23
        additionalFileButton = 25
        additionalFile = 26
        helpDialog = 27
        reset = 28

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        pos = 4
        friendlyPos = 5
        length = 6
        name = 7
        period = 8
        tl = 9
        fileButton = 10
        file = 11
        vTypes = 12
        nextEdges = 13
        detectPersons = 14
        timeThreshold = 16
        speedThreshold = 17
        jamThreshold = 18
        show = 19
        parameters = 21
        parametersButton = 22
        frontButton = 24
        additionalFileButton = 25
        additionalFile = 26
        helpDialog = 27
        reset = 28

    class inspectSelection:
        friendlyPos = 1
        length = 2
        name = 3
        period = 4
        tl = 5
        fileButton = 6
        file = 7
        vTypes = 8
        nextEdges = 9
        detectPersons = 11
        timeThreshold = 12
        speedThreshold = 13
        jamThreshold = 14
        show = 15
        parameters = 17
        parametersButton = 18
        frontButton = 20
        additionalFileButton = 21
        additionalFile = 22
        helpDialog = 23
        reset = 24

# E2Multilane


class E2Multilane:
    class create:
        id = 5
        friendlyPos = 6
        period = 7
        tl = 8
        name = 9
        fileButton = 10
        file = 11
        vTypes = 12
        nextEdges = 13
        detectPersons = 14
        timeThreshold = 16
        speedThreshold = 17
        jamThreshold = 18
        show = 19
        parameters = 21
        parametersButton = 22
        additionalFileButton = 24
        additionalFile = 25
        helpDialog = 26
        reset = 27

    class inspect:
        id = 1
        lanes = 2
        pos = 3
        endPos = 4
        friendlyPos = 5
        period = 6
        tl = 7
        name = 8
        fileButton = 9
        file = 10
        vTypes = 11
        nextEdges = 12
        detectPersons = 13
        timeThreshold = 15
        speedThreshold = 16
        jamThreshold = 17
        show = 18
        parameters = 20
        parametersButton = 21
        frontButton = 23
        additionalFileButton = 24
        additionalFile = 25
        helpDialog = 26
        reset = 27

    class inspectSelection:
        friendlyPos = 1
        period = 2
        tl = 3
        name = 4
        fileButton = 5
        file = 6
        vTypes = 7
        nextEdges = 8
        detectPersons = 9
        timeThreshold = 11
        speedThreshold = 12
        jamThreshold = 13
        show = 14
        parameters = 16
        parametersButton = 17
        frontButton = 19
        additionalFileButton = 20
        additionalFile = 21
        helpDialog = 22
        reset = 23


# E3


class E3:
    class create:
        id = 5
        name = 6
        period = 7
        fileButton = 8
        file = 9
        vTypes = 10
        nextEdges = 11
        detectPersons = 12
        openEntry = 14
        timeThreshold = 15
        speedThreshold = 16
        expectArrival = 17
        parameters = 19
        parametersButton = 20
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

    class inspect:
        id = 1
        pos = 2
        name = 3
        period = 4
        fileButton = 5
        file = 6
        vTypes = 7
        nextEdges = 8
        detectPersons = 9
        openEntry = 11
        timeThreshold = 12
        speedThreshold = 13
        expectArrival = 14
        parameters = 16
        parametersButton = 17
        frontButton = 19
        additionalFileButton = 20
        additionalFile = 21
        helpDialog = 22
        reset = 23

    class inspectSelection:
        name = 1
        period = 2
        fileButton = 3
        file = 4
        vTypes = 5
        nextEdges = 6
        detectPersons = 7
        openEntry = 9
        timeThreshold = 10
        speedThreshold = 11
        expectArrival = 12
        parameters = 14
        parametersButton = 15
        frontButton = 17
        additionalFileButton = 18
        additionalFile = 19
        helpDialog = 20
        reset = 21

# entryExit


class entryExit:
    class create:
        friendlyPos = 5
        parameters = 7
        parametersButton = 8
        parent = 10

    class inspect:
        lane = 1
        moveLane = 2
        pos = 3
        friendlyPos = 4
        parameters = 6
        parametersButton = 7
        frontButton = 9
        additionalFileButton = 10
        additionalFile = 11
        helpDialog = 12
        reset = 13

    class inspectSelection:
        friendlyPos = 1
        parameters = 3
        parametersButton = 4
        frontButton = 6
        additionalFileButton = 7
        additionalFile = 8
        helpDialog = 9
        reset = 10

# E1Instant


class E1Instant:
    class create:
        id = 5
        friendlyPos = 6
        name = 7
        fileButton = 8
        file = 9
        vTypes = 10
        nextEdges = 11
        detectPersons = 12
        parameters = 15
        parametersButton = 16
        additionalFileButton = 18
        additionalFile = 19
        helpDialog = 20
        reset = 21

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        pos = 4
        friendlyPos = 5
        name = 6
        fileButton = 7
        file = 8
        vTypes = 9
        nextEdges = 10
        detectPersons = 11
        parameters = 14
        parametersButton = 15
        frontButton = 17
        additionalFileButton = 18
        additionalFile = 19
        helpDialog = 20
        reset = 21

    class inspectSelection:
        friendlyPos = 1
        name = 2
        fileButton = 3
        file = 4
        vTypes = 5
        nextEdges = 6
        detectPersons = 7
        parameters = 10
        parametersButton = 11
        frontButton = 13
        additionalFileButton = 14
        additionalFile = 15
        helpDialog = 16
        reset = 17

# calibrator (FALTA CENTER XXXX)


class calibrator:
    class create:
        id = 5
        name = 6
        period = 7
        routeProbe = 8
        outputButton = 9
        output = 10
        jamTreshold = 11
        vTypes = 12

    class inspect:
        id = 1
        edge = 2
        pos = 3
        frequency = 4
        name = 5
        routeProbe = 6
        output = 7
        jamTreshold = 8
        vTypes = 9
        parameters = 12

    class inspectLane:
        id = 1
        lane = 2
        laneMoveUp = 3
        pos = 4
        frequency = 5
        name = 6
        routeProbe = 7
        output = 8
        jamTreshold = 9
        vTypes = 10
        parameters = 13

    class inspectSelection:
        pos = 1
        frequency = 2
        name = 3
        routeProbe = 4
        output = 5
        jamTreshold = 6
        vTypes = 7
        parameters = 10

# routeProbe (FALTA CENTER XXXX)


class routeProbe:
    class create:
        id = 5
        frequency = 6
        name = 7
        file = 8
        begin = 9
        vTypes = 10
        center = 14

    class inspect:
        id = 1
        edge = 2
        frequency = 3
        name = 4
        file = 5
        begin = 6
        vTypes = 7
        parameters = 10

    class inspectSelection:
        frequency = 1
        name = 2
        file = 3
        begin = 4
        vTypes = 5
        parameters = 8

# vaporizer (FALTA CENTER XXXX)


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


class poly:
    class create:
        id = 5
        name = 6
        colorButton = 7
        color = 8
        fill = 9
        lineWidth = 10
        layer = 11
        type = 12
        imgFileButton = 13
        imgFile = 14
        angle = 15
        geo = 16
        parameters = 18
        parametersButton = 19
        closeShape = 21
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

    class inspect:
        id = 1
        shape = 2
        name = 3
        colorButton = 4
        color = 5
        fill = 6
        lineWidth = 7
        layer = 8
        type = 9
        imgFileButton = 10
        imgFile = 11
        angle = 12
        geo = 15
        geoshape = 15
        parameters = 17
        parametersButton = 18
        frontButton = 20
        closeShape = 21
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

    class inspectSelection:
        name = 1
        colorButton = 2
        color = 3
        fill = 4
        lineWidth = 5
        layer = 6
        type = 7
        imgFileButton = 8
        imgFile = 9
        angle = 11
        geo = 13
        parameters = 15
        parametersButton = 16
        frontButton = 18
        closeShape = 19
        additionalFileButton = 20
        additionalFile = 21
        helpDialog = 22
        reset = 23

# POI


class poi:
    class create:
        id = 5
        name = 6
        colorButton = 7
        color = 8
        type = 9
        icon = 10
        layer = 12
        width = 13
        height = 14
        imgFileButton = 15
        imgFile = 16
        angle = 17
        parameters = 19
        parametersButton = 20
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

    class inspect:
        id = 1
        pos = 2
        name = 3
        colorButton = 4
        color = 5
        type = 5
        icon = 6
        layer = 8
        width = 9
        height = 10
        imgFileButton = 11
        imgFile = 12
        angle = 13
        parameters = 15
        parametersButton = 16
        frontButton = 18
        additionalFileButton = 19
        additionalFile = 20
        helpDialog = 21
        reset = 22

    class inspectSelection:
        name = 1
        colorButton = 2
        color = 3
        type = 4
        icon = 5
        layer = 7
        width = 8
        height = 9
        imgFileButton = 10
        imgFile = 11
        angle = 12
        parameters = 14
        parametersButton = 15
        frontButton = 17
        additionalFileButton = 18
        additionalFile = 19
        helpDialog = 20
        reset = 21

# POIGEO


class poiGeo:
    class create:
        id = 5
        name = 6
        colorButton = 7
        color = 8
        type = 9
        icon = 10
        layer = 12
        width = 13
        height = 14
        imgFileButton = 15
        imgFile = 16
        angle = 17
        parameters = 19
        parametersButton = 20
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25
        lonLat = 27
        latLon = 28
        coordinates = 29
        center = 30
        createButton = 31

    class inspect:
        id = 1
        name = 2
        colorButton = 3
        color = 4
        type = 5
        icon = 6
        layer = 7
        width = 8
        height = 9
        imgFileButton = 10
        imgFile = 11
        angle = 12
        lon = 14
        lat = 15
        parameters = 17
        parametersButton = 18
        frontButton = 20
        additionalFileButton = 21
        additionalFile = 22
        helpDialog = 23
        reset = 24

    class inspectSelection:
        name = 1
        colorButton = 2
        color = 3
        type = 4
        icon = 5
        layer = 7
        width = 8
        height = 9
        imgFileButton = 10
        imgFile = 11
        angle = 12
        parameters = 14
        parametersButton = 15
        frontButton = 17
        additionalFileButton = 18
        additionalFile = 19
        helpDialog = 20
        reset = 21

# POILane


class poiLane:
    class create:
        id = 5
        friendlyPos = 6
        posLat = 7
        name = 8
        colorButton = 9
        color = 10
        type = 11
        icon = 12
        layer = 14
        width = 15
        height = 16
        imgFileButton = 17
        imgFile = 18
        angle = 19
        parameters = 21
        parametersButton = 22
        additionalFileButton = 24
        additionalFile = 25
        helpDialog = 26
        reset = 27

    class inspect:
        id = 1
        lane = 2
        moveLane = 3
        pos = 4
        friendlyPos = 5
        posLat = 6
        name = 7
        colorButton = 8
        color = 9
        type = 10
        icon = 11
        layer = 13
        width = 14
        height = 15
        imgFileButton = 16
        imgFile = 17
        angle = 18
        parameters = 20
        parametersButton = 21
        frontButton = 23
        additionalFileButton = 24
        additionalFile = 25
        helpDialog = 26
        reset = 27

    class inspectSelection:
        friendlyPos = 1
        posLat = 2
        name = 3
        colorButton = 4
        color = 5
        type = 6
        icon = 7
        layer = 9
        width = 10
        height = 11
        imgFileButton = 12
        imgFile = 13
        angle = 14
        parameters = 18
        parametersButton = 19
        frontButton = 21
        additionalFileButton = 22
        additionalFile = 23
        helpDialog = 24
        reset = 25

# jps walkableArea


class jpsWalkableArea:
    class create:
        id = 5
        name = 6
        parameters = 8
        parametersButton = 9
        additionalFileButton = 11
        additionalFile = 12
        helpDialog = 13
        reset = 14

    class inspect:
        id = 1
        shape = 2
        name = 3
        parameters = 5
        parametersButton = 6
        frontButton = 8
        additionalFileButton = 9
        additionalFile = 10
        helpDialog = 11
        reset = 12

    class inspectSelection:
        name = 1
        parameters = 3
        parametersButton = 4
        frontButton = 6
        additionalFileButton = 7
        additionalFile = 8
        helpDialog = 9
        reset = 10

# jps obstacle


class jpsObstacle:
    class create:
        id = 5
        name = 6
        parameters = 8
        parametersButton = 9
        additionalFileButton = 11
        additionalFile = 12
        helpDialog = 13
        reset = 14

    class inspect:
        id = 1
        shape = 2
        name = 3
        parameters = 5
        parametersButton = 6
        frontButton = 8
        additionalFileButton = 9
        additionalFile = 10
        helpDialog = 11
        reset = 12

    class inspectSelection:
        name = 1
        parameters = 3
        parametersButton = 4
        frontButton = 6
        additionalFileButton = 7
        additionalFile = 8
        helpDialog = 9
        reset = 10

# --------------------------------
# TAZ ELEMENTS
# --------------------------------

# TAZ


class TAZ:
    class create:
        id = 2
        center = 3
        name = 4
        colorButton = 5
        color = 6
        fill = 7
        parameters = 9
        parametersButton = 10
        edgesWithin = 12
        additionalFileButton = 13
        additionalFile = 14
        helpDialog = 15
        reset = 16

    class inspect:
        id = 1
        shape = 2
        center = 3
        name = 4
        colorButton = 5
        color = 6
        fill = 7
        parameters = 9
        parametersButton = 10
        frontButton = 12
        additionalFileButton = 13
        additionalFile = 14
        helpDialog = 15
        reset = 16

    class inspectSelection:
        center = 1
        name = 2
        colorButton = 3
        color = 4
        fill = 5
        parameters = 7
        parametersButton = 8
        frontButton = 10
        additionalFileButton = 11
        additionalFile = 12
        helpDialog = 13
        reset = 14

# --------------------------------
# DEMAND ELEMENTS
# --------------------------------

# type


class type:

    color = 5

    class buttons:
        create = 2
        delete = 3
        copy = 3
        dialog = 25

    class edit:
        id = 8
        typeDist = 8
        vClass = 9
        vClassEdited = 9
        colorButton = 11
        color = 12
        length = 13
        minGap = 14
        maxSpeed = 15
        desiredMaxSpeed = 16
        parkingBadges = 17
        accel = 18
        decel = 19
        sigma = 20
        tau = 21

    class editDefault:
        vClass = 7
        vClassEdited = 8
        colorButton = 9
        color = 10
        length = 11
        minGap = 12
        maxSpeed = 13
        desiredMaxSpeed = 14
        parkingBadges = 15
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
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        depart = 18
        parameters = 21

# trip (from-to junctions)


class tripJunction:
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
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        depart = 18
        parameters = 21

# trip (from-to TAZs)


class tripTAZ:
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
        fromTAZ = 5
        toTAZ = 6
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
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        depart = 18
        parameters = 21

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
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        depart = 18
        parameters = 21

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
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        depart = 18
        parameters = 21

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
        parameters = 33

    class inspectSelection:
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        begin = 18
        terminate = 21
        spacing = 23
        terminateOption = 25
        spacingOption = 26
        parameters = 27

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
        parameters = 32

    class inspectSelection:
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        begin = 18
        terminate = 21
        spacing = 23
        terminateOption = 25
        spacingOption = 26
        parameters = 27

# flow (from-to TAZs)


class flowTAZ:
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
        fromTAZ = 5
        toTAZ = 6
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
        parameters = 32

    class inspectSelection:
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        begin = 18
        terminate = 21
        spacing = 23
        terminateOption = 25
        spacingOption = 26
        parameters = 27

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
        parameters = 33

    class inspectSelection:
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        begin = 18
        terminate = 21
        spacing = 23
        terminateOption = 25
        spacingOption = 26
        parameters = 27

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
        parameters = 32

    class inspectSelection:
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departLane = 6
        departPos = 7
        departSpeed = 8
        arrivalLane = 9
        arrivalPos = 10
        arrivalSpeed = 11
        line = 12
        personNumber = 13
        containerNumber = 14
        departPosLat = 15
        arrivalPosLat = 16
        insertionChecks = 17
        begin = 18
        terminate = 21
        spacing = 23
        terminateOption = 25
        spacingOption = 26
        parameters = 27

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
        join = 14
        permitted = 14
        parking = 15
        actType = 17
        tripID = 18
        line = 19
        onDemand = 20
        jump = 21
        split = 22

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        triggered = 5
        expected = 7
        join = 7
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13
        jump = 14
        split = 15

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        join = 9
        permitted = 9
        parking = 10
        actType = 12
        tripID = 13
        line = 14
        onDemand = 15
        jump = 16
        split = 17

# stop over train stop


class stopTrainStop:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        triggered = 12
        expected = 14
        join = 14
        permitted = 14
        parking = 15
        actType = 17
        tripID = 18
        line = 19
        onDemand = 20
        jump = 21
        split = 22

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        triggered = 5
        expected = 7
        join = 7
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13
        jump = 14
        split = 15

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        join = 9
        permitted = 9
        parking = 10
        actType = 12
        tripID = 13
        line = 14
        onDemand = 15
        jump = 16
        split = 17

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
        join = 14
        permitted = 14
        parking = 15
        actType = 17
        tripID = 18
        line = 19
        onDemand = 20
        jump = 21
        split = 22

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        triggered = 5
        expected = 7
        join = 7
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13
        jump = 14
        split = 15

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        join = 9
        permitted = 9
        parking = 10
        actType = 12
        tripID = 13
        line = 14
        onDemand = 15
        jump = 16
        split = 17

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
        join = 14
        permitted = 14
        parking = 15
        actType = 17
        tripID = 18
        line = 19
        onDemand = 20
        jump = 21
        split = 22

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        triggered = 5
        expected = 7
        join = 7
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13
        jump = 14
        split = 15

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        join = 9
        permitted = 9
        parking = 10
        actType = 12
        tripID = 13
        line = 14
        onDemand = 15
        jump = 16
        split = 17

# stop over parkings


class stopParkingArea:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        triggered = 12
        expected = 14
        join = 14
        permitted = 14
        actType = 15
        tripID = 16
        line = 17
        onDemand = 18
        jump = 19
        split = 20

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        triggered = 5
        expected = 7
        join = 7
        permitted = 7
        actType = 8
        tripID = 9
        line = 10
        onDemand = 11
        jump = 12
        split = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        triggered = 7
        expected = 9
        join = 9
        permitted = 9
        actType = 10
        tripID = 11
        line = 12
        onDemand = 13
        jump = 14
        split = 15

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
        join = 16
        permitted = 16
        parking = 17
        actType = 19
        tripID = 20
        line = 21
        onDemand = 22
        jump = 23
        split = 24

    class inspect:
        lane = 1
        moveLane = 2
        startPos = 3
        endPos = 4
        friendlyPos = 5
        posLat = 6
        durationEnable = 7
        duration = 8
        untilEnable = 8
        until = 9
        extensionEnable = 9
        extension = 10
        triggered = 11
        expected = 13
        join = 13
        permitted = 13
        parking = 14
        actType = 16
        tripID = 17
        line = 18
        onDemand = 19
        jump = 20
        split = 21

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
        join = 11
        permitted = 11
        parking = 12
        actType = 14
        tripID = 15
        line = 16
        onDemand = 17
        jump = 18
        split = 19

# waypoint over bus stops


class waypointBusStop:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        permitted = 12
        parking = 13
        actType = 15
        tripID = 16
        line = 17
        speed = 18
        jump = 19
        split = 20

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        permitted = 5
        parking = 6
        actType = 8
        tripID = 9
        line = 10
        speed = 11
        jump = 12
        split = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        speed = 13
        jump = 14
        split = 15

# waypoint over train stops


class waypointTrainStop:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        permitted = 12
        parking = 13
        actType = 15
        tripID = 16
        line = 17
        speed = 18
        jump = 19
        split = 20

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        permitted = 5
        parking = 6
        actType = 8
        tripID = 9
        line = 10
        speed = 11
        jump = 12
        split = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        speed = 13
        jump = 14
        split = 15

# waypoint over container stops


class waypointContainerStop:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        permitted = 12
        parking = 13
        actType = 15
        tripID = 16
        line = 17
        speed = 18
        jump = 19
        split = 20

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        permitted = 5
        parking = 6
        actType = 8
        tripID = 9
        line = 10
        speed = 11
        jump = 12
        split = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        speed = 13
        jump = 14
        split = 15

# waypoint over charging stations


class waypointChargingStation:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        permitted = 12
        parking = 13
        actType = 15
        tripID = 16
        line = 17
        speed = 18
        jump = 19
        split = 20

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        permitted = 5
        parking = 6
        actType = 8
        tripID = 9
        line = 10
        speed = 11
        jump = 12
        split = 13

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        permitted = 7
        parking = 8
        actType = 10
        tripID = 11
        line = 12
        speed = 13
        jump = 14
        split = 15

# waypoint over parkings


class waypointParking:
    class create:
        durationEnable = 8
        duration = 9
        untilEnable = 9
        until = 10
        extensionEnable = 10
        extension = 11
        permitted = 12
        actType = 13
        tripID = 14
        line = 15
        speed = 16
        jump = 17
        split = 18

    class inspect:
        durationEnable = 1
        duration = 2
        untilEnable = 2
        until = 3
        extensionEnable = 3
        extension = 4
        permitted = 5
        actType = 6
        tripID = 7
        line = 8
        speed = 9
        jump = 10
        split = 11

    class inspectSelection:
        durationEnable = 1
        duration = 2
        untilEnable = 3
        until = 4
        extensionEnable = 5
        extension = 6
        permitted = 7
        actType = 8
        tripID = 9
        line = 10
        speed = 11
        jump = 12
        split = 13

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
        permitted = 14
        parking = 15
        actType = 17
        tripID = 18
        line = 19
        speed = 20
        jump = 21
        split = 22

    class inspect:
        lane = 1
        moveLane = 2
        startPos = 3
        endPos = 4
        friendlyPos = 5
        posLat = 6
        durationEnable = 7
        duration = 8
        untilEnable = 8
        until = 9
        extensionEnable = 9
        extension = 10
        permitted = 11
        parking = 12
        actType = 14
        tripID = 15
        line = 16
        speed = 17
        jump = 18
        split = 19

    class inspectSelection:
        friendlyPos = 1
        posLat = 2
        durationEnable = 3
        duration = 4
        untilEnable = 5
        until = 6
        extensionEnable = 7
        extension = 8
        permitted = 9
        parking = 10
        actType = 12
        tripID = 13
        line = 14
        speed = 15
        jump = 16
        split = 17

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
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departPos = 6
        depart = 7
        parameters = 10


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
        parameters = 19

    class inspectSelection:
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departPos = 6
        begin = 7
        terminate = 10
        spacing = 12
        terminateOption = 14
        spacingOption = 15
        parameters = 18

# PersonPlans


class personTrip:
    class edge:
        class edge:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                arrivalPos = 1
                vTypes = 2
                modes = 3
                lines = 4
                walkFactor = 5
                group = 6

            class inspectSelection:
                arrivalPos = 1
                vTypes = 2
                modes = 3
                lines = 4
                walkFactor = 5
                group = 6

        class busStop:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

            class inspectSelection:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

        class junction:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

            class inspectSelection:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

    class busStop:
        class edge:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                arrivalPos = 1
                vTypes = 2
                modes = 3
                lines = 4
                walkFactor = 5
                group = 6

            class inspectSelection:
                arrivalPos = 1
                vTypes = 2
                modes = 3
                lines = 4
                walkFactor = 5
                group = 6

        class busStop:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

            class inspectSelection:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

        class junction:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

            class inspectSelection:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

    class junction:
        class edge:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                arrivalPos = 1
                vTypes = 2
                modes = 3
                lines = 4
                walkFactor = 5
                group = 6

            class inspectSelection:
                arrivalPos = 1
                vTypes = 2
                modes = 3
                lines = 4
                walkFactor = 5
                group = 6

        class busStop:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

            class inspectSelection:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

        class junction:
            class create:
                arrivalPos = 8
                vTypes = 9
                modes = 10
                lines = 11
                walkFactor = 12
                group = 13

            class inspect:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5

            class inspectSelection:
                vTypes = 1
                modes = 2
                lines = 3
                walkFactor = 4
                group = 5


class ride:
    class edge:
        class edge:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class busStop:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class junction:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

    class busStop:
        class edge:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class busStop:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class junction:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

    class junction:
        class edge:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class busStop:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class junction:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3


class walk:
    class edge:
        class edge:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class busStop:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class junction:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

    class busStop:
        class edge:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class busStop:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class junction:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

    class junction:
        class edge:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class busStop:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class junction:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3


class walkEdges:
    class create:
        arrivalPos = 8

    class inspect:
        arrivalPos = 1

    class inspectSelection:
        arrivalPos = 1


class walkRoute:
    class create:
        arrivalPos = 8

    class inspect:
        arrivalPos = 1

    class inspectSelection:
        arrivalPos = 1

# Container


class container:
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
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departPos = 6
        depart = 7
        parameters = 10


class containerFlow:
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
        parameters = 19

    class inspectSelection:
        typeButton = 1
        type = 2
        colorButton = 4
        color = 5
        departPos = 6
        begin = 7
        terminate = 10
        spacing = 12
        terminateOption = 14
        spacingOption = 15
        parameters = 18

# PersonPlans


class transport:
    class edge:
        class edge:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class busStop:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                lines = 1
                group = 2

            class inspectSelection:
                lines = 1
                group = 2

        class junction:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                lines = 1
                group = 2

            class inspectSelection:
                lines = 1
                group = 2

    class busStop:
        class edge:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class busStop:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                lines = 1
                group = 2

            class inspectSelection:
                lines = 1
                group = 2

        class junction:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                lines = 1
                group = 2

            class inspectSelection:
                lines = 1
                group = 2

    class junction:
        class edge:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                arrivalPos = 1
                lines = 2
                group = 3

            class inspectSelection:
                arrivalPos = 1
                lines = 2
                group = 3

        class busStop:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                lines = 1
                group = 2

            class inspectSelection:
                lines = 1
                group = 2

        class junction:
            class create:
                arrivalPos = 8
                lines = 9
                group = 10

            class inspect:
                lines = 1
                group = 2

            class inspectSelection:
                lines = 1
                group = 2


class tranship:
    class edge:
        class edge:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class busStop:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                speed = 1
                duration = 2

            class inspectSelection:
                speed = 1
                duration = 2

        class junction:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                speed = 1
                duration = 2

            class inspectSelection:
                speed = 1
                duration = 2

    class busStop:
        class edge:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class busStop:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                speed = 1
                duration = 2

            class inspectSelection:
                speed = 1
                duration = 2

        class junction:
            class create:
                speed = 8
                duration = 9

            class inspect:
                speed = 1
                duration = 2

            class inspectSelection:
                speed = 1
                duration = 2

    class junction:
        class edge:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                arrivalPos = 1
                speed = 2
                duration = 3

            class inspectSelection:
                arrivalPos = 1
                speed = 2
                duration = 3

        class busStop:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                speed = 1
                duration = 2

            class inspectSelection:
                speed = 1
                duration = 2

        class junction:
            class create:
                arrivalPos = 8
                speed = 9
                duration = 10

            class inspect:
                speed = 1
                duration = 2

            class inspectSelection:
                speed = 1
                duration = 2

# stop plans


class stopPlan:
    class edge:
        class create:
            durationEnable = 8
            duration = 9
            untilEnable = 10
            until = 11
            actType = 11
            friendlyPos = 12

        class inspect:
            endPos = 1
            durationEnable = 2
            duration = 3
            untilEnable = 4
            until = 5
            actType = 5
            friendlyPos = 6

        class inspectSelection:
            durationEnable = 1
            duration = 2
            untilEnable = 3
            until = 4
            actType = 5
            friendlyPos = 6

    class stoppingPlace:
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
        parameters = 1

# edgeRelData


class edgeRelData:
    class create:
        parameters = 12

    class inspect:
        fromEdge = 1
        toEdge = 2
        parameters = 5

    class inspectSelection:
        parameters = 1

# TAZRelData


class TAZRelData:
    class create:
        parameters = 12

    class inspect:
        fromTAZ = 1
        toTAZ = 2
        parameters = 5

    class inspectSelection:
        parameters = 1

# edge mean data


class edgeMeanData:

    class inspect:
        id = 6
        file = 7
        period = 8
        begin = 9
        end = 10
        excludeEmpty = 11
        withInternal = 13
        maxTravelTime = 14
        minSamples = 15
        speedThreshold = 16
        vTypes = 17
        trackVehicles = 18
        detectPersons = 19
        writeAttributes = 20
        edges = 21
        edgesFile = 22
        aggregate = 23

# lane mean data


class laneMeanData:

    class inspect:
        id = 6
        file = 7
        period = 8
        begin = 9
        end = 10
        excludeEmpty = 11
        withInternal = 13
        maxTravelTime = 14
        minSamples = 15
        speedThreshold = 16
        vTypes = 17
        trackVehicles = 18
        detectPersons = 19
        writeAttributes = 20
        edges = 21
        edgesFile = 22
        aggregate = 23

# --------------------------------
# Dialogs
# --------------------------------


class dialog:

    class allowVClass:
        allVehicles = 1
        onlyRoadVehicles = 2
        onlyRailVehicles = 3
        disallowAll = 4
        passenger = 5
        private = 6
        taxi = 7
        bus = 8
        coach = 9
        delivery = 10
        truck = 11
        trailer = 12
        emergency = 13
        motorcycle = 14
        moped = 15
        bicycle = 16
        scooter = 17
        pedestrian = 18
        wheelchair = 19
        tram = 20
        rail_electric = 21
        rail_fast = 22
        rail_urban = 23
        rail = 24
        cable_car = 25
        subway = 26
        evehicle = 27
        army = 28
        ship = 29
        authority = 30
        vip = 31
        hov = 32
        container = 33
        aircraft = 34
        drone = 35
        custom1 = 36
        custom2 = 37
        accept = 38
        cancel = 39
        reset = 40
