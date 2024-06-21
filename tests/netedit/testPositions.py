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

from turtle import down, left, up


class elements:

    class edge0Ped:
        x = 402
        y = 470

    class edge0:
        x = 381
        y = 443

    class edge1Ped:
        x = 874
        y = 238
    
    class edge1:
        x = 850
        y = 222

    class edge2Ped:
        x = 463
        y = -3

    class edge2:
        x = 494
        y = 26

    class edge3Ped:
        x = 409
        y = 89

    class edge3:
        x = 477
        y = 56

    class edge4Ped:
        x = 791
        y = 236

    class edge4:
        x = 817
        y = 235

    class edge5Ped:
        x = 459
        y = 383

    class edge5:
        x = 515
        y = 413

    class edgeCenter0Ped:
        x = 341 
        y = 276

    class edgeCenter0:
        x = 332
        y = 248

    class edgeCenter1Ped:
        x = 345
        y = 191

    class edgeCenter1:
        x = 345
        y = 221

    class junction0:
        x = 154
        y = 430

    class junction1:
        x = 839
        y = 428

    class junction2:
        x = 837
        y = 38

    class junction3:
        x = 154
        y = 35

    class junction4:
        x = 157
        y = 237
        
    class junction5:
        x = 503
        y = 233

    # movement radius
    class movementRadius:
        up = 70
        down = -70
        left = -70
        right = 70       

    # additional (for inspect, delete, etc.)
    class additionals:

        class busStop:
            x = 322 
            y = 195

        class trainStop:
            x = 343
            y = 200

        class containerStop:
            x = 345
            y = 195

        class parkingArea:
            x = 343
            y = 190

        class parkingSpace:
            x = 437
            y = 120

        class e2Detector:
            x = 310
            y = 218
  
        class squaredA:
            x = 437
            y = 137

        class squaredB:
            x = 534
            y = 137

    # demand elements (for inspect, delete, etc.)
    class demands:

        class TAZGreen:
            x = 518
            y = 160

        class TAZRed:
            x = 730
            y = 177

        class busStop:
            x = 296
            y = 11

        class trainStop:
            x = 718
            y = 8

        class containerStop:
            x = 411
            y = 4

        class chargingStation:
            x = 623
            y =  23

        class parkingArea:
            x = 525
            y = 0

        class route:
            x = 420
            y = 439

        class routeB:
            x = 536
            y = 409
##############################
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
