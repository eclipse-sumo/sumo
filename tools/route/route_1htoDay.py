#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    route_1htoDay.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    11.09.2009

"""
Uses "route_departOffset.py" for building 24 route files which describe
 a whole day assuming the given route files describes an hour.
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import route_departOffset
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.options import ArgumentParser
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    optParser = ArgumentParser()
    optParser.add_argument("input_network", category='input', help="Provide an input network")
    return optParser.parse_args(args=args)


def main(options):
    for i in range(0, 24):
        out = options.input_network
        out = out[:out.find(".")] + "_" + str(i) + out[out.find("."):]
        print("Building routes for hour " + str(i) + " into '" + out + "'...")
        route_departOffset.main(
            route_departOffset.get_options([
                "--input-file", options.input_network,
                "--output-file", out,
                "--depart-offset", str(i * 3600),
                "--modify-ids"]))


if __name__ == "__main__":
    main(get_options())
