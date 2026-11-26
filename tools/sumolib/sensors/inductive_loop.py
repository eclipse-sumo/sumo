# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    inductive_loop.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2010-02-18

"""
Library for reading and storing Inductive Loop detector representations and
 measurements.
"""
from __future__ import absolute_import

from .. import xml as sumolib_xml


def InductiveLoop(id, lane, pos, frequency=60, file="NUL", friendlyPos=True, vTypes="", attributes=None):
    obj = sumolib_xml.makeXMLObject(
        "e1Detector",
        {"id": id, "lane": lane, "pos": pos, "freq": frequency,
         "file": file, "friendlyPos": friendlyPos, "vTypes": vTypes}
    )
    obj.frequency = frequency
    obj.attributes = dict(attributes) if attributes else {}
    return obj


def read(filename):
    loops = []
    for det in sumolib_xml.parse(filename, "e1Detector"):
        params = {p.key: p.value for p in (det.param or [])}
        loops.append(InductiveLoop(
            det.id, det.lane, float(det.pos), float(det.freq), det.file,
            det.getAttributeSecure("friendlyPos", True), det.getAttributeSecure("vTypes", ""), params
        ))
    return loops
