# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _platoonmode.py
# @author Leonhard Luecken
# @date   2017-04-09
# @version $Id$


from enum import Enum

# Platoon modes


class PlatoonMode(Enum):
    NONE = 0
    LEADER = 1
    FOLLOWER = 2
    CATCHUP = 3
    CATCHUP_FOLLOWER = 4
