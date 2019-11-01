# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    keplerjson.py
# @author  Michael Behrisch
# @date    2019-11-01
# @version $Id$

"""
This module includes functions for converting SUMO's fcd-output into
kepler.gl JSON format (https://github.com/keplergl/kepler.gl/blob/master/docs/user-guides/c-types-of-layers/k.trip.md)
"""
from __future__ import absolute_import
from collections import defaultdict
import json


def makeFeature(vehId, coords):
    return {
        "type": "Feature",
        "properties": {
            "vehId": vehId
        },
        "geometry": {
            "type": "LineString",
            "coordinates": coords
        }
    }


def fcd2keplerjson(inpFCD, outStream, ignored):
    tracks = defaultdict(list)
    for timestep in inpFCD:
        for v in timestep.vehicle:
            tracks[v.id].append([v.x, v.y, v.z, timestep.time])
    jsonRoot = {
        "type": "FeatureCollection",
        "features": [makeFeature(*veh) for veh in tracks.items()]
    }
    json.dump(jsonRoot, outStream)
