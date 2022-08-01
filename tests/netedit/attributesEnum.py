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

    class demand:
        grid = '1'
        junctionShape = '2'

    class data:
        junctionShape = '1'


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
        tlLayout = 11
        tl = 12
        parameters = 17

# edge


class edge:
    class create:
        numLanes = 5
        speed = 6
        allowButton = 7
        allow = 8
        disallowButton = 9
        disallow = 10
        spreadType = 11
        priority = 13
        width = 14

    class createLane:
        add = 20
        remove = 21
        speed = 22
        allowButton = 23
        allow = 24
        disallowButton = 25
        disallow = 26
        width = 27

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
        disallowedButton = 10
        disallowed = 11
        shape = 12
        length = 13
        spreadType = 14
        name = 16
        width = 17
        endOffset = 18
        shapeStart = 19
        shapeEnd = 20
        distance = 21
        stopOffset = 22
        stopExceptionButton = 23
        stopException = 24
        parameters = 27

    class inspectSelection:
        speed = 1
        priority = 2
        numLanes = 3
        type = 4
        allowedButton = 5
        allowed = 6
        disallowedButton = 7
        disallowed = 8
        length = 9
        spreadType = 10
        name = 12
        width = 13
        endOffset = 14
        shapeStart = 15
        shapeEnd = 16
        stopOffset = 17
        stopExceptionButton = 18
        stopException = 19
        parameters = 22

    class template:
        create = 33
        copy = 34
        clear = 35

# lane


class lane:
    class inspect:
        speed = 1
        allowButton = 2
        allow = 3
        disallowButton = 4
        disallow = 5
        width = 6
        endOffset = 7
        acceleration = 8
        customShape = 9
        opposite = 10
        changeLeftButton = 11
        changeLeft = 12
        changeRightButton = 13
        changeRight = 14
        type = 15
        stopOffset = 16
        stopOffsetExceptionButton = 17
        stopOffsetException = 18
        parameters = 21

    class inspectSelection:
        speed = 1
        allowButton = 2
        allow = 3
        disallowButton = 4
        disallow = 5
        width = 6
        endOffset = 7
        acceleration = 8
        changeLeftButton = 9
        changeLeft = 10
        changeRightButton = 11
        changeRight = 12
        type = 13
        stopOffset = 14
        stopOffsetExceptionButton = 15
        stopOffsetException = 16
        parameters = 19

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
        disallowButton = 8
        disallow = 9
        speed = 10
        length = 11
        customShape = 12
        changeLeftButton = 13
        changeLeft = 14
        changeRightButton = 15
        changeRight = 16
        indirect = 17
        type = 18
        parameters = 21

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
        disallowButton = 10
        disallow = 11
        speed = 12
        length = 13
        customShape = 14
        changeLeftButton = 15
        changeLeft = 16
        changeRightButton = 17
        changeRight = 18
        indirect = 19
        type = 20
        parameters = 23

    class inspectSelection:
        passC = 1
        keepClear = 2
        contPos = 3
        uncontrolled = 4
        visibility = 5
        allowButton = 6
        allow = 7
        disallowButton = 8
        disallow = 9
        speed = 10
        length = 11
        changeLeftButton = 12
        changeLeft = 13
        changeRightButton = 14
        changeRight = 15
        indirect = 16
        type = 17
        parameters = 20

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
        disallowButton = 10
        disallow = 11
        speed = 12
        length = 13
        changeLeftButton = 14
        changeLeft = 15
        changeRightButton = 16
        changeRight = 17
        indirect = 18
        type = 19
        parameters = 22

    saveConnections = 4

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

    clearEdges = 3
    clearEdgesSelected = 4
    firstField = 5


# TLS


class TLS:
    class create:
        TLS = 10
        noTLS = 11


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
    class buttons:
        create = 2
        delete = 3
        copy = 3
        dialog = 24

    class edit:
        id = 8
        vTypeDist = 9
        vClass = 10
        colorButton = 12
        color = 13
        length = 14
        minGap = 15
        maxSpeed = 16
        accel = 17
        decel = 18
        sigma = 19
        tau = 20

    class editDefault:
        vTypeDist = 7
        vClass = 8
        colorButton = 10
        color = 11
        length = 12
        minGap = 13
        maxSpeed = 14
        accel = 15
        decel = 16
        sigma = 17
        tau = 18

    class editDialog:
        vClass = 1
        id = 3
        colorButton = 4
        color = 5
        length = 6
        minGap = 7
        maxSpeed = 8
        speedFactor = 9
        emissionClass = 10
        width = 12
        height = 13
        imgFileButton = 14
        imgFile = 15
        osgFileButton = 16
        osgFile = 17
        probability = 18
        laneChangeModel = 19
        guiShape = 21
        personCapacity = 23
        containerCapacity = 24
        boardingDuration = 25
        loadingDuration = 26
        latAlignment = 27
        minGapLat = 29
        maxSpeedLat = 30
        actionStepLength = 31
        carriageLength = 32
        locomotiveLength = 33
        carriageGap = 34
        parametersButton = 35
        parameters = 36

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


class tripFromToEdge:
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
        fromEdge = 4
        toEdge = 5
        via = 6
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
        fromJunction = 4
        toJunction = 5
        colorButton = 6
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
        insertionChecks = 19
        depart = 20
        parameters = 23

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
        route = 4
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
        departEdge = 4
        arrivalEdge = 5
        colorButton = 6
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
        insertionChecks = 19
        depart = 20
        parameters = 23

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

# flow (from-to edges)


class flowFromToEdge:
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
        fromEdge = 4
        toEdge = 5
        via = 6
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
        fromJunction = 4
        toJunction = 5
        colorButton = 6
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
        insertionChecks = 19
        begin = 20
        terminate = 23
        spacing = 25
        end = 26
        terminateOption = 27
        spacingOption = 28
        parameters = 30

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

# flow (over route)


class routeflow:
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
        route = 4
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
        departEdge = 4
        arrivalEdge = 5
        colorButton = 6
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
        insertionChecks = 19
        begin = 20
        terminate = 23
        spacing = 25
        end = 26
        terminateOption = 27
        spacingOption = 28
        parameters = 30

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

# stop (over busStop/containerStop/chargingStation)


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
        tripID = 17
        line = 18
        onDemad = 19

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
        tripID = 10
        line = 11
        onDemad = 12

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
        actType = 12
        tripID = 13
        line = 14
        onDemad = 15

# stop (over parkings)


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
        onDemad = 18

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
        onDemad = 11

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
        onDemad = 14

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
        expected = 16
        permitted = 16
        parking = 17
        actType = 18
        tripID = 19
        line = 20
        onDemad = 21

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
        tripID = 15
        line = 16
        onDemad = 17

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
        actType = 14
        tripID = 15
        line = 16
        onDemad = 17

# waypoint (over busStop/containerStop/chargingStation)


class waypoint:
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
        tripID = 17
        line = 18
        speed = 19

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
        tripID = 10
        line = 11
        speed = 12

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
        actType = 12
        tripID = 13
        line = 14
        speed = 15

# waypoint (over parkings)


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
        actType = 18
        tripID = 19
        line = 20
        speed = 21

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
        tripID = 15
        line = 16
        speed = 17

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
        actType = 14
        tripID = 15
        line = 16
        speed = 17

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
        colorButton = 4
        color = 5
        departPos = 6
        depart = 7
        parameters = 10

    class inspectSelection:
        type = 1
        colorButton = 2
        color = 3
        departPos = 4
        depart = 5
        parameters = 8


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
        colorButton = 4
        color = 5
        departPos = 6
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
        departPos = 4
        begin = 5
        terminate = 8
        spacing = 10
        terminateOption = 12
        spacingOption = 13
        parameters = 15

# Container


class container:
    class create:
        id = 8
        colorButton = 9
        color = 10
        depart = 11

    class inspect:
        id = 1
        type = 2
        type = 3
        colorButton = 4
        color = 5
        depart = 6
        parameters = 9

    class inspectSelection:
        type = 1
        colorButton = 2
        color = 3
        depart = 4
        parameters = 7


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
        colorButton = 4
        color = 5
        begin = 6
        terminate = 9
        spacing = 11
        end = 12
        terminateOption = 13
        spacingOption = 14
        parameters = 16

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
