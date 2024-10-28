#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Giuliana Armellini
# @date

import os
import sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
from sumolib.xml import parse_fast, parse_fast_nested  # noqa

for f in ("data/ptlines.xml", "data/ptlines2.xml.gz"):
    print(f)
    for ptline, ptline_route in parse_fast_nested(f, "ptLine", ("id", "name", "line", "type"), "route", "edges"):
        print(",".join((ptline.id, ptline.attr_name, ptline.line, ptline.type)))
        print(ptline_route.edges)
    for ptline in parse_fast(f, "ptLine", ("id", "line", "type")):
        print(",".join(ptline))
