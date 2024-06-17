# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    testPositions.py
# @author  Pablo Alvarez Lopez
# @date    2023-07-13

# --------------------------------
# GENERAL
# --------------------------------

class additionalElements:

    class centralEdge0:
        x = 324
        y = 215

    class centralEdge1:
        x = 361
        y = 215

    class centralEdge2:
        x = 401
        y = 215

    class centralEdge3:
        x = 439
        y = 215

    class centralEdge4:
        x = 554
        y = 215

    class inspectBusStop:
        x = 325
        y = 205

    class inspectTrainStop:
        x = 325
        y = 205

    class inspectContainerStop:
        x = 326
        y = 198

    class inspectChargingStation:
        x = 306
        y = 258

    class inspectParkingArea:
        x = 324
        y = 190

    class squaredAdditionalA:
        x = 440
        y = 140

    class squaredAdditionalB:
        x = 440
        y = 140


class demandElements:

    class edge0:
        x = 450
        y = 440

    class edge1:
        x = 850
        y = 230

    class edge2:
        x = 500
        y = 28

    class edge3:
        x = 500
        y = 55

    class edge4:
        x = 816
        y = 230

    class edge5:
        x = 450
        y = 410

    class edgeCenter1:
        x = 360
        y = 212

    class edgeCenter2:
        x = 360
        y = 250

    class junction0:
        x = 155
        y = 430

    class junction1:
        x = 825
        y = 422

    class junction2:
        x = 825
        y = 60

    class junction3:
        x = 170
        y = 60

    class TAZGreen:
        x = 525
        y = 175

    class TAZRed:
        x = 720
        y = 175

    class busStop:
        x = 295
        y = 10

    class trainStop:
        x = 720
        y = 10

    class containerStop:
        x = 400
        y = 0

    class chargingStation:
        x = 620
        y = 25

    class parkingArea:
        x = 525
        y = 0

    class route:
        x = 440
        y = 440

    class routeB:
        x = 470
        y = 410

    class containerX:
        x = 470
        y = 400

    class containerPlanEdgeX:
        x = 197
        y = 37

    class planTopEdgeX:
        x = 750
        y = 3

    class planTopBusStopX:
        x = 750
        y = 33

    class planTopRideX:
        x = 750
        y = 33

    class planTopX:
        x = 720
        y = 3

    # vehicle (over edge/route)
    class vehicleEdge:
        x = 185
        y = 440

    # click over single trip or flow over junctions
    class vehicleJunction:
        x = 162
        y = 405

    # click over single trip or flow over TAZ
    class vehicleTAZ:
        x = 525
        y = 230

    # click over single person / flow over edges
    class person:
        x = 150
        y = 470

    # click stop/waypoint over edge
    class stopEdgeX:
        x = 256
        y = 35

    # click over plan that ends in junction
    class planJunctionX:
        x = 134
        y = 52

    # click over plan in busStop
    class planBusStopX:
        x = 318
        y = 14

    # click over plan in edge
    class planEdgeX:
        x = 197
        y = 36
