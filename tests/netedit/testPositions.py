# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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

# @file    testPositions.py
# @author  Pablo Alvarez Lopez
# @date    2023-07-13

# --------------------------------
# GENERAL
# --------------------------------

class demandElements:

    class edge0:
        x = 450
        y = 440
    
    class edge1:
        x = 885
        y = 225
    class edge2:
        x = 280
        y = 30

    class edge3:
        x = 280
        y = 75

    class edge4:
        x = 835
        y = 225

    class edge5:
        x = 450
        y = 390

    class junction0:
        x = 135
        y = 415

    class junction1:
        x = 860
        y = 415

    class junction2:
        x = 860
        y = 50

    class junction3:
        x = 135
        y = 50

    # click over single trips, flows, etc... over edges
    class singleVehicleEdge:
        x = 160
        y = 435

    # click over multiple trips, flows, etc... over edges
    class multipleVehiclesEdge:
        x = 186
        y = 465