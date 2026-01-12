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

# @file    movements.py
# @author  Pablo Alvarez Lopez
# @date    2025-07-16

class movements:

    # movement radius
    class radius:
        up = 70
        down = -70
        left = -70
        right = 70

    class radiusB:
        up = 100
        down = -100
        left = -100
        right = 100

    class radiusC:
        up = 200
        down = 0
        left = 0
        right = 190

    class movementEdgeTop:
        up = -180
        down = 0
        left = 0
        right = 0

    class movementEdgeBot:
        up = 0
        down = 180
        left = 0
        right = 0
