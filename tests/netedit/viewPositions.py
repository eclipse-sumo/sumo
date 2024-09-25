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

# @file    viewPositions.py
# @author  Pablo Alvarez Lopez
# @date    2023-07-13

# --------------------------------
# GENERAL
# --------------------------------

# temporal position (used to mark non finished tests)
class tmp:
    x = 0
    y = 0

# reference position


class reference:
    x = 0
    y = 0

# extern lane bot (common for all tests)


class externLaneBot:
    x = 42
    y = 332

# extern lane Top (common for all tests)


class externLaneTop:
    x = 42
    y = 136

# network


class selection:

    class edge:
        x = 513
        y = 92

    class lane:
        x = 586
        y = 213

    class junction:
        x = 498
        y = 236

    class rectangleSmallA:
        x = 410
        y = 147

    class rectangleSmallB:
        x = 587
        y = 323

    class rectangleMediumA:
        x = 304
        y = 94

    class rectangleMediumB:
        x = 694
        y = 373

    class rectangleLargeA:
        x = 129
        y = 4

    class rectangleLargeB:
        x = 867
        y = 463


class network:

    class junction:

        class positionA:
            x = 257
            y = 424

        class positionA_2:
            x = 308
            y = 377

        class positionB:
            x = 734
            y = 426

        class positionB_2:
            x = 691
            y = 384

        class positionC:
            x = 737
            y = 42

        class positionC_2:
            x = 691
            y = 90

        class positionD:
            x = 259
            y = 42

        class positionD_2:
            x = 309
            y = 92

        class cross:

            class center:
                x = 498
                y = 236

            class left:
                x = 278
                y = 237

            class right:
                x = 706
                y = 232

            class top:
                x = 495
                y = 23

            class bot:
                x = 495
                y = 441

        class merge:

            class fromA:
                x = 400
                y = 235

            class fromB:
                x = 400
                y = 45

            class toA:
                x = 590
                y = 235

            class toB:
                x = 590
                y = 45

    class edge:

        class centerA:
            x = 500
            y = 429

        class centerB:
            x = 500
            y = 30

        class center:
            x = 500
            y = 250

        class top:
            x = 497
            y = 150

        class bot:
            x = 497
            y = 324

        class left:
            x = 409
            y = 237

        class right:
            x = 587
            y = 237

        class leftTop:
            x = 400
            y = 220

        class leftBot:
            x = 400
            y = 250

        class rightTop:
            x = 600
            y = 220

        class rightBot:
            x = 600
            y = 250

        class topLeft:
            x = 460
            y = 130

        class topRight:
            x = 520
            y = 130

        class botLeft:
            x = 460
            y = 320

        class botRight:
            x = 520
            y = 320

    class crossing:

        class left:
            x = 450
            y = 225

        class right:
            x = 543
            y = 232

        class left_norec:
            x = 470
            y = 230

        class right_norec:
            x = 510
            y = 230

        class center:
            x = 400
            y = 400

    class connection:

        class connectionA:
            x = 400
            y = 194

        class connectionB:
            x = 446
            y = 220

        class laneUpLeft:
            x = 460
            y = 10

        class laneUpRight:
            x = 530
            y = 10

        class laneDownLeft:
            x = 450
            y = 450

        class laneDownRight:
            x = 530
            y = 450

        class laneLeftTop:
            x = 240
            y = 195

        class laneLeftBot:
            x = 240
            y = 270

        class laneRightTop:
            x = 750
            y = 195

        class laneRightBot:
            x = 750
            y = 270


class elements:

    class edge0Ped:
        x = 402
        y = 470

    class edge0:
        x = 381
        y = 443

    class edge0_dataMode:
        x = 381
        y = 443 + 30

    class edge1Ped:
        x = 874
        y = 238

    class edge1:
        x = 850
        y = 222

    # class edge1_dataMode:
    #   x = 850
    #   y = 222 + 30

    class edge2Ped:
        x = 463
        y = -3

    class edge2:
        x = 494
        y = 26

    # class edge2_dataMode:
    #    x = 494
    #    y = 26 + 30

    class edge3Ped:
        x = 409
        y = 57

    class edge3:
        x = 477
        y = 56

    # class edge3_dataMode:
    #    x = 477
    #    y = 56 + 30

    class edge4Ped:
        x = 791
        y = 236

    class edge4:
        x = 817
        y = 235

    # class edge4_dataMode:
    #    x = 817
    #    y = 235 + 30

    class edge5Ped:
        x = 459
        y = 383

    class edge5:
        x = 515
        y = 413

    # class edge5_dataMode:
    #    x = 515
    #    y = 413 + 30

    class edgeBike0:
        x = 145
        y = 125

    class edgeBike1:
        x = 180
        y = 140

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
        x = 155
        y = 45

    class junction4:
        x = 159
        y = 218

    class junction5:
        x = 503
        y = 233

    # movement radius
    class movementRadius:
        up = 70
        down = -70
        left = -70
        right = 70

    class movementRadiusB:
        up = 100
        down = -100
        left = -100
        right = 100

    class movementRadiusC:
        up = 200
        down = 0
        left = 0
        right = 190

    # additional (for inspect, delete, etc.)
    class additionals:

        class busStop:
            x = 344
            y = 204

        class trainStop:
            x = 343
            y = 204

        class containerStop:
            x = 345
            y = 195

        class chargingStation:
            x = 300
            y = 221

        class parkingArea:
            x = 340
            y = 185

        class parkingSpace:
            x = 437
            y = 120

        class e2Detector:
            x = 310
            y = 218

        class e2MultilaneDetector:
            x = 560
            y = 440

        class calibrator:
            x = 120
            y = 470

        class calibratorLane:
            x = 335
            y = 440

        class routeProbe:
            x = 488
            y = 168

        class vaporizer:
            x = 467
            y = 168

        class squaredA:
            x = 437
            y = 137

        class squaredB:
            x = 534
            y = 137

        class shapeA:
            x = 304
            y = 133

        class shapeB:
            x = 459
            y = 133

        class shapeC:
            x = 650
            y = 133

        class shapeD:
            x = 650
            y = 233

        shapeSize = 200

    # demand elements (for inspect, delete, etc.)
    class demands:

        class TAZGreen:
            x = 557
            y = 231

        class TAZRed:
            x = 708
            y = 232

        class busStop:
            x = 296
            y = 11

        class busStopB:
            x = 340
            y = 200

        class trainStop:
            x = 718
            y = 13

        class trainStopB:
            x = 284
            y = 265

        class containerStop:
            x = 411
            y = -5

        class containerStopB:
            x = 411
            y = 194

        class chargingStation:
            x = 623
            y = 23

        class chargingStationB:
            x = 287
            y = 220

        class parkingArea:
            x = 525
            y = -5

        class parkingAreaB:
            x = 352.00
            y = 282

        class route:
            x = 420
            y = 439

        class routeB:
            x = 536
            y = 409

        class container:
            x = 161
            y = 476

        class vehicleEdge:
            x = 185
            y = 440

        class vehicleJunction:
            x = 159
            y = 405

        class vehicleTAZ:
            x = 565
            y = 230

        class person:
            x = 161
            y = 474

        class stopLane:
            x = 415
            y = 30

        class planEdge1:
            x = 848
            y = 160

        class planEdge1Ped:
            x = 872
            y = 160

        class planEdge2:
            x = 775
            y = 27

        class planEdge2Ped:
            x = 600
            y = 4

        class planEdgeBike0Ped:
            x = 112
            y = 130

        class planBusStop:
            x = 112
            y = 130

        class planContainerStop:
            x = 112
            y = 130

        class planStopEdge:
            x = 493
            y = -15

        class planStopBusStop:
            x = 250
            y = -5
