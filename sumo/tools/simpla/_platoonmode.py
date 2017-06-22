"""
@author Leonhard Luecken
@date   2017-04-09

-----------------------------------
SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
-----------------------------------
"""

from enum import Enum

# Platoon modes


class PlatoonMode(Enum):
    NONE = 0
    LEADER = 1
    FOLLOWER = 2
    CATCHUP = 3
    CATCHUP_FOLLOWER = 4
