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
        x = 477
        y = 215
        
    class centralEdge4:
        x = 516
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
        x = 885
        y = 225

    class edge2:
        x = 375
        y = 33

    class edge3:
        x = 280
        y = 75

    class edge4:
        x = 835
        y = 225

    class edge5:
        x = 450
        y = 390

    class edgeRepeat0:
        x = 110
        y = 250

    class edgeRepeat1:
        x = 150
        y = 250

    class edgeBus1:
        x = 150
        y = 220

    class edgeBus2:
        x = 150
        y = 150

    class edgeCenter1:
        x = 350
        y = 250

    class edgeCenter2:
        x = 350
        y = 205

    class junction0:
        x = 135
        y = 390

    class junction1:
        x = 860
        y = 390

    class junction2:
        x = 860
        y = 50

    class junction3:
        x = 135
        y = 50

    class TAZGreen:
        x = 200
        y = 315

    class TAZRed:
        x = 740
        y = 315

    class busStop:
        x = 375
        y = 12

    class trainStop:
        x = 375
        y = 90

    class containerStop:
        x = 615
        y = -9

    class chargingStation:
        x = 500
        y = 30

    class parkingArea:
        x = 615
        y = 110

    class route:
        x = 470
        y = 440

    class routeB:
        x = 470
        y = 400

    class container:
        x = 135
        y = 480

    class containerPlanEdge:
        x = 197
        y = 37

    class planTopEdge:
        x = 750
        y = 3

    class planTopBusStop:
        x = 750
        y = 33

    class planTopRide:
        x = 750
        y = 33

    class planTop:
        x = 720
        y = 3

    # click over single trips, flows, etc... over edges
    class singleVehicleEdge:
        x = 160
        y = 440

    # click over multiple trips, flows, etc... over edges
    class multipleVehiclesEdge:
        x = 190
        y = 440

    # click over single person / flow over edges
    class person:
        x = 133
        y = 479

    # click stop/waypoint over edge
    class stopEdge:
        x = 256
        y = 35

    # click over single trip or flow over junctions
    class singleVehicleJunction:
        x = 160
        y = 435

    # click over single trip or flow over TAZ
    class singleVehicleTAZ:
        x = 280
        y = 315

    # click over plan that ends in junction
    class planJunction:
        x = 134
        y = 52

    # click over plan in busStop
    class planBusStop:
        x = 318
        y = 14

    # click over plan in edge
    class planEdge:
        x = 197
        y = 36
