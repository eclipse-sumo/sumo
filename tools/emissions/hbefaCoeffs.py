#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    hbefaCoeffs.py
# @author  Michael Behrisch
# @date    2023-06-06

"""
This script converts the CSV from the coefficient fit to a C++ table.
"""
from __future__ import print_function

import sys
import collections
import csv

params = collections.defaultdict(dict)
for line in csv.DictReader(open(sys.argv[1])):
    factor = 1000.
    if line["e"] == "FC_MJ":
        factor /= 3.6  # MJ to Wh
    params[line["Subsegment"]][line["e"]] = [factor * float(x) for x in (line['E0'], line['V'], line['A'], line['V2'], line['V3'], line['AV'], line['AV2'])]  # noqa
for segment, data in params.items():
    data["PM + PM (non-exhaust)"] = [a + b for a, b in zip(data["PM"], data["PM (non-exhaust)"])]
    if data["FC"] != 7 * [0]:
        data["FC_MJ"] = 7 * [0]
    s = segment.replace("<=", "le").replace(">=", "ge").replace("<", "lt").replace(">", "gt")
    print("    {\n        // %s" % s.translate(str.maketrans(" ,'/*", "____s")))
    for e in ("CO2(total)", "CO", "HC", "FC", "NOx", "PM + PM (non-exhaust)", "FC_MJ"):
        print("        { %.3e, %.3e, %.3e, %.3e, %.3e, %.3e, %.3e }, // %s" % tuple(data[e] + [e]))
    print("    },")
