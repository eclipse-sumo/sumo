# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2013-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    trj.py
# @author  Mirko Barthauer
# @date    2021-03-31

"""
This module includes functions for converting SUMO's fcd-output into
data files read by SSAM.
See the SSAM TRJ standard at
https://github.com/OSADP/SSAM/blob/master/Documentation/SSAM%20File%20Format%20v1.04.pdf
https://github.com/OSADP/SSAM/blob/master/Documentation/Open%20Source%20SSAM%20File%20Format%20v3.0.pdf
"""

import sys
import struct
import math


def fcd2trj(inpFCD, outSTRM, further):
    endian = b'B' if sys.byteorder == "big" else b'L'

    # write FORMAT block
    outSTRM.write(struct.pack("=c", chr(0).encode()))
    outSTRM.write(struct.pack("=c", endian))
    outSTRM.write(struct.pack("f", 3.0))
    outSTRM.write(struct.pack("=c", chr(0).encode()))

    # write DIMENSIONS block
    outSTRM.write(struct.pack("=c", chr(1).encode()))
    outSTRM.write(struct.pack("=c", chr(1).encode()))
    outSTRM.write(struct.pack("=f", 1.0))
    outSTRM.write(struct.pack("=i", int(further["bbox"][0][0])))
    outSTRM.write(struct.pack("=i", int(further["bbox"][0][1])))
    outSTRM.write(struct.pack("=i", int(further["bbox"][1][0])))
    outSTRM.write(struct.pack("=i", int(further["bbox"][1][1])))

    # go through fcd output and encode links and vehicle IDs
    edgeDict = {}
    trafficPartDict = {}
    prevSpeed = {}
    edgeCounter = 0
    trafficPartCounter = 0

    for timestep in inpFCD:
        # write TIMESTEP block
        outSTRM.write(struct.pack("=c", chr(2).encode()))
        outSTRM.write(struct.pack("=f", timestep.time))

        for v in timestep.vehicle:
            speed = float(v.speed)
            if v.id not in trafficPartDict:
                trafficPartDict[v.id] = trafficPartCounter
                trafficPartCounter += 1
                prevSpeed[v.id] = speed
            numericID = trafficPartDict[v.id]
            accel = (speed - prevSpeed[v.id])/further["timestep"]
            if "_" not in v.lane:
                edge = v.lane
                laneIndex = 0
            else:
                edge, laneIndex = v.lane.rsplit("_", 1)
                laneIndex = min(int(laneIndex), 9)

            if edge not in edgeDict:
                edgeDict[edge] = edgeCounter
                edgeCounter += 1
            edgeNumericID = edgeDict[edge]

            # calculated values
            x = float(v.x)
            y = float(v.y)
            angle = float(v.angle)
            rearX = x - math.cos(angle)*further["length"]
            rearY = y - math.sin(angle)*further["length"]

            # write VEHICLE block
            outSTRM.write(struct.pack("=c", chr(3).encode()))
            outSTRM.write(struct.pack("=i", numericID))
            outSTRM.write(struct.pack("=i", edgeNumericID))
            outSTRM.write(struct.pack("=c", chr(laneIndex).encode()))
            outSTRM.write(struct.pack("=f", x))
            outSTRM.write(struct.pack("=f", y))
            outSTRM.write(struct.pack("=f", rearX))
            outSTRM.write(struct.pack("=f", rearY))
            outSTRM.write(struct.pack("=f", further["length"]))
            outSTRM.write(struct.pack("=f", further["width"]))
            outSTRM.write(struct.pack("=f", speed))
            outSTRM.write(struct.pack("=f", accel))
            outSTRM.write(struct.pack("=f", 0.0))  # front z coord
            outSTRM.write(struct.pack("=f", 0.0))  # back z coord
            # remember value for next timestep
            prevSpeed[numericID] = speed
