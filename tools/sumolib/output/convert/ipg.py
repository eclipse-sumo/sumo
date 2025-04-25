# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    ipg.py
# @author  Laura Bieker-Walz
# @date    2020-10-02

"""
This module includes functions for converting SUMO's fcd-output into
data files read by IPG traces.
"""
from __future__ import print_function
from __future__ import absolute_import


def fcd2ipg(inpFCD, outSTRM, further):
    print('# Time tx ty tz rx ry rz', file=outSTRM)
    for timestep in inpFCD:
        if timestep.vehicle:
            for v in timestep.vehicle:
                print('%s %s %s %s %s 0 0'
                      % (timestep.time, v.x, v.y, v.z, v.angle), file=outSTRM)
