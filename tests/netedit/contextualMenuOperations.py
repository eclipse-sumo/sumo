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

# @file    contextualMenuOperations.py
# @author  Pablo Alvarez Lopez
# @date    2024-07-24

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
    mainMenuPosition = 7
    subMenuAPosition = 0
    subMenuBPosition = 0

# copy cursor geo position
class copyCursorGeoPosition:
    mainMenuPosition = 8
    subMenuAPosition = 0
    subMenuBPosition = 0
   
# show cursor geo position in geo hack
class showCursorGeoPositionGeoHack:
    mainMenuPosition = 7
    subMenuAPosition = 1
    subMenuBPosition = 0 

# show cursor geo position in google maps
class showCursorGeoPositionGoogleMaps:
    mainMenuPosition = 7
    subMenuAPosition = 2
    subMenuBPosition = 0 

# show cursor geo position in OSM
class showCursorGeoPositionOSM:
    mainMenuPosition = 7
    subMenuAPosition = 3
    subMenuBPosition = 0 

# junctions
class junction:
    
    # add TLS
    class addTLS:
        mainMenuPosition = 11
        subMenuAPosition = 1
        subMenuBPosition = 0 

    # add TLS
    class addJoinedTLS:
        mainMenuPosition = 11
        subMenuAPosition = 2
        subMenuBPosition = 0 

    # reset edge endPoints
    class resetEdgePoints:
        mainMenuPosition = 12
        subMenuAPosition = 0
        subMenuBPosition = 0 

    # set custom junction shape
    class setCustomJunctionShape:
        mainMenuPosition = 13
        subMenuAPosition = 0
        subMenuBPosition = 0 

    # reset custom junction shape
    class resetCustomJunctionShape:
        mainMenuPosition = 14
        subMenuAPosition = 0
        subMenuBPosition = 0 

    # replace junction by geometry point
    class replaceJunctionByGeometryPoint:
        mainMenuPosition = 15
        subMenuAPosition = 0
        subMenuBPosition = 0

    # split junction
    class splitJunction:
        mainMenuPosition = 15
        subMenuAPosition = 0
        subMenuBPosition = 0

    # split junction and reconnect
    class splitJunctionAndReconnect:
        mainMenuPosition = 16
        subMenuAPosition = 0
        subMenuBPosition = 0

    # convert to round about
    class convertToRoundAbout:
        mainMenuPosition = 15
        subMenuAPosition = 0
        subMenuBPosition = 0

    # clear connections
    class clearConnections:
        mainMenuPosition = 16
        subMenuAPosition = 0
        subMenuBPosition = 0

    # reset connections
    class resetConnections:
        mainMenuPosition = 17
        subMenuAPosition = 0
        subMenuBPosition = 0