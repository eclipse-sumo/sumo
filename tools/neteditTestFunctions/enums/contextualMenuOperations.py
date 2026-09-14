# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    contextualMenuOperations.py
# @author  Pablo Alvarez Lopez
# @date    2024-07-24


class contextualMenu:

    # --------------------------------
    # GENERAL
    # --------------------------------

    # center element in view

    class center:
        mainMenuPosition = 3
        subMenuAPosition = 0
        subMenuBPosition = 0

    # copy name

    class copyName:
        mainMenuPosition = 4
        subMenuAPosition = 0
        subMenuBPosition = 0

    # copy typed name

    class copyTypedName:
        mainMenuPosition = 5
        subMenuAPosition = 0
        subMenuBPosition = 0

    # add to selected

    class addToSelected:
        mainMenuPosition = 6
        subMenuAPosition = 0
        subMenuBPosition = 0

    # copy cursor position

    class copyCursorPosition:
        mainMenuPosition = 8
        subMenuAPosition = 0
        subMenuBPosition = 0

    # copy cursor geo position

    class copyCursorGeoPosition:
        mainMenuPosition = 9
        subMenuAPosition = 0
        subMenuBPosition = 0

    # show cursor geo position in geo hack

    class showCursorGeoPositionGeoHack:
        mainMenuPosition = 8
        subMenuAPosition = 1
        subMenuBPosition = 0

    # show cursor geo position in google maps

    class showCursorGeoPositionGoogleMaps:
        mainMenuPosition = 8
        subMenuAPosition = 2
        subMenuBPosition = 0

    # show cursor geo position in OSM

    class showCursorGeoPositionOSM:
        mainMenuPosition = 8
        subMenuAPosition = 3
        subMenuBPosition = 0

    # junctions

    class junction:

        # add TLS
        class addTLS:
            mainMenuPosition = 12
            subMenuAPosition = 1
            subMenuBPosition = 0

        # add TLS
        class addJoinedTLS:
            mainMenuPosition = 12
            subMenuAPosition = 2
            subMenuBPosition = 0

        # reset edge endPoints
        class resetEdgePoints:
            mainMenuPosition = 13
            subMenuAPosition = 0
            subMenuBPosition = 0

        # set custom junction shape
        class setCustomJunctionShape:
            mainMenuPosition = 14
            subMenuAPosition = 0
            subMenuBPosition = 0

        # reset custom junction shape
        class resetCustomJunctionShape:
            mainMenuPosition = 15
            subMenuAPosition = 0
            subMenuBPosition = 0

        # replace junction by geometry point
        class replaceJunctionByGeometryPoint:
            mainMenuPosition = 16
            subMenuAPosition = 0
            subMenuBPosition = 0

        # split junction
        class splitJunction:
            mainMenuPosition = 16
            subMenuAPosition = 0
            subMenuBPosition = 0

        # split junction and reconnect
        class splitJunctionAndReconnect:
            mainMenuPosition = 17
            subMenuAPosition = 0
            subMenuBPosition = 0

        # convert to round about
        class convertToRoundAbout:
            mainMenuPosition = 16
            subMenuAPosition = 0
            subMenuBPosition = 0

        # clear connections
        class clearConnections:
            mainMenuPosition = 17
            subMenuAPosition = 0
            subMenuBPosition = 0

        # reset connections
        class resetConnections:
            mainMenuPosition = 18
            subMenuAPosition = 0
            subMenuBPosition = 0

    # lanes

    class edges:

        class edgeOperations:

            # split edge
            class splitEdge:
                mainMenuPosition = 14
                subMenuAPosition = 1
                subMenuBPosition = 0

            # split edge in both directions
            class splitEdgeBothDirection:
                mainMenuPosition = 14
                subMenuAPosition = 2
                subMenuBPosition = 0

            # set geometry point
            class setGeometryPoint:
                mainMenuPosition = 14
                subMenuAPosition = 3
                subMenuBPosition = 0

            # restore geometry point
            class restoreGeometryPoint:
                mainMenuPosition = 14
                subMenuAPosition = 4
                subMenuBPosition = 0

            # reverse edge
            class reverseEdge:
                mainMenuPosition = 14
                subMenuAPosition = 4
                subMenuBPosition = 0

            # add reverse direction
            class addReverseDirection:
                mainMenuPosition = 14
                subMenuAPosition = 5
                subMenuBPosition = 0

            # add reverse direction disconnected
            class addReverseDirectionDisconnected:
                mainMenuPosition = 14
                subMenuAPosition = 6
                subMenuBPosition = 0

            # reset length
            class resetLength:
                mainMenuPosition = 14
                subMenuAPosition = 7
                subMenuBPosition = 0

            # straighten
            class straightenEdge:
                mainMenuPosition = 14
                subMenuAPosition = 7
                subMenuBPosition = 0

            # smooth
            class smoothEdge:
                mainMenuPosition = 14
                subMenuAPosition = 8
                subMenuBPosition = 0

            # straighten elevation
            class straightenElevationEdge:
                mainMenuPosition = 14
                subMenuAPosition = 10
                subMenuBPosition = 0

            # smooth elevation
            class smoothElevationEdge:
                mainMenuPosition = 14
                subMenuAPosition = 11
                subMenuBPosition = 0

        class laneOperations:

            # duplicate lane
            class duplicateLane:
                mainMenuPosition = 15
                subMenuAPosition = 1
                subMenuBPosition = 0

            # set custom lane shape
            class setCustomLaneShape:
                mainMenuPosition = 15
                subMenuAPosition = 2
                subMenuBPosition = 0

            # reset custom lane shape
            class resetCustomShape:
                mainMenuPosition = 15
                subMenuAPosition = 3
                subMenuBPosition = 0

            # reset opposite lane
            class resetOppositeLane:
                mainMenuPosition = 15
                subMenuAPosition = 3
                subMenuBPosition = 0

            # add restricted lane
            class addRestrictedLane:

                # sidewalk
                class sidewalk:
                    mainMenuPosition = 15
                    subMenuAPosition = 4
                    subMenuBPosition = 1

                # sidewalk
                class bikeLane:
                    mainMenuPosition = 15
                    subMenuAPosition = 4
                    subMenuBPosition = 2

                # sidewalk
                class busLane:
                    mainMenuPosition = 15
                    subMenuAPosition = 4
                    subMenuBPosition = 3

                # green verge (front)
                class greenVergeFront:
                    mainMenuPosition = 15
                    subMenuAPosition = 4
                    subMenuBPosition = 4

                # green verge (back)
                class greenVergeBack:
                    mainMenuPosition = 15
                    subMenuAPosition = 4
                    subMenuBPosition = 5

            # remove restricted lane
            class removeRestrictedLane:

                # sidewalk
                class sidewalk:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 1

                # sidewalk
                class bikeLane:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 2

                # sidewalk
                class busLane:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 3

                # green verge
                class greenVerge:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 4

            # transform to restricted lane
            class TransformToRestrictedLane:

                # sidewalk
                class sidewalk:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 1

                # sidewalk
                class bikeLane:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 2

                # sidewalk
                class busLane:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 3

                # green verge
                class greenVerge:
                    mainMenuPosition = 15
                    subMenuAPosition = 5
                    subMenuBPosition = 4

        class templateOperations:

            # use edge as template
            class useEdgeAsTemplate:
                mainMenuPosition = 16
                subMenuAPosition = 1
                subMenuBPosition = 0

            # apply template
            class applyTemplate:
                mainMenuPosition = 16
                subMenuAPosition = 2
                subMenuBPosition = 0

    # routes

    class routes:

        class reverse:

            # reverse
            class current:
                mainMenuPosition = 15
                subMenuAPosition = 1
                subMenuBPosition = 0

            # reverse
            class add:
                mainMenuPosition = 15
                subMenuAPosition = 2
                subMenuBPosition = 0

    # vehicles

    class vehicles:

        # reverse
        class reverseCurrent:
            mainMenuPosition = 12
            subMenuAPosition = 1
            subMenuBPosition = 0

        # reverse
        class reverseAdd:
            mainMenuPosition = 12
            subMenuAPosition = 2
            subMenuBPosition = 0

            # reverse
        class reverseTripCurrent:
            mainMenuPosition = 13
            subMenuAPosition = 1
            subMenuBPosition = 0

        # reverse
        class reverseTripAdd:
            mainMenuPosition = 13
            subMenuAPosition = 2
            subMenuBPosition = 0

        # transform vehicles over junctions
        class transformJunctions:
            mainMenuPosition = 13
            subMenuAPosition = 1
            subMenuBPosition = 0

        # transform vehicles over TAZs
        class transformTAZs:
            mainMenuPosition = 13
            subMenuAPosition = 1
            subMenuBPosition = 0

        # transform persons
        class transformPerson:
            mainMenuPosition = 12
            subMenuAPosition = 1
            subMenuBPosition = 0

        # transform containers
        class transformContainer:
            mainMenuPosition = 12
            subMenuAPosition = 1
            subMenuBPosition = 0

        class transform:

            # from vehicle
            class vehicle:

                class vehicleEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 1
                    subMenuBPosition = 0

                class routeFlow:
                    mainMenuPosition = 14
                    subMenuAPosition = 2
                    subMenuBPosition = 0

                class routeFlowEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 3
                    subMenuBPosition = 0

                class trip:
                    mainMenuPosition = 14
                    subMenuAPosition = 4
                    subMenuBPosition = 0

                class flow:
                    mainMenuPosition = 14
                    subMenuAPosition = 5
                    subMenuBPosition = 0

            # from vehicle embedded
            class vehicleEmbedded:

                class vehicle:
                    mainMenuPosition = 14
                    subMenuAPosition = 1
                    subMenuBPosition = 0

                class routeFlow:
                    mainMenuPosition = 14
                    subMenuAPosition = 2
                    subMenuBPosition = 0

                class routeFlowEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 3
                    subMenuBPosition = 0

                class trip:
                    mainMenuPosition = 14
                    subMenuAPosition = 4
                    subMenuBPosition = 0

                class flow:
                    mainMenuPosition = 14
                    subMenuAPosition = 5
                    subMenuBPosition = 0

            # from route flow
            class routeFlow:

                class vehicle:
                    mainMenuPosition = 14
                    subMenuAPosition = 1
                    subMenuBPosition = 0

                class vehicleEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 2
                    subMenuBPosition = 0

                class routeFlowEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 3
                    subMenuBPosition = 0

                class trip:
                    mainMenuPosition = 14
                    subMenuAPosition = 4
                    subMenuBPosition = 0

                class flow:
                    mainMenuPosition = 14
                    subMenuAPosition = 5
                    subMenuBPosition = 0

            # from route flow embedded
            class routeFlowEmbedded:

                class vehicle:
                    mainMenuPosition = 14
                    subMenuAPosition = 1
                    subMenuBPosition = 0

                class vehicleEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 2
                    subMenuBPosition = 0

                class routeFlow:
                    mainMenuPosition = 14
                    subMenuAPosition = 3
                    subMenuBPosition = 0

                class trip:
                    mainMenuPosition = 14
                    subMenuAPosition = 4
                    subMenuBPosition = 0

                class flow:
                    mainMenuPosition = 14
                    subMenuAPosition = 5
                    subMenuBPosition = 0

            # from trip
            class trip:

                class vehicle:
                    mainMenuPosition = 14
                    subMenuAPosition = 1
                    subMenuBPosition = 0

                class vehicleEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 2
                    subMenuBPosition = 0

                class routeFlow:
                    mainMenuPosition = 14
                    subMenuAPosition = 3
                    subMenuBPosition = 0

                class routeFlowEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 4
                    subMenuBPosition = 0

                class flow:
                    mainMenuPosition = 14
                    subMenuAPosition = 5
                    subMenuBPosition = 0

            # from flow
            class flow:

                class vehicle:
                    mainMenuPosition = 14
                    subMenuAPosition = 1
                    subMenuBPosition = 0

                class vehicleEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 2
                    subMenuBPosition = 0

                class routeFlow:
                    mainMenuPosition = 14
                    subMenuAPosition = 3
                    subMenuBPosition = 0

                class routeFlowEmbedded:
                    mainMenuPosition = 14
                    subMenuAPosition = 4
                    subMenuBPosition = 0

                class trip:
                    mainMenuPosition = 14
                    subMenuAPosition = 5
                    subMenuBPosition = 0
