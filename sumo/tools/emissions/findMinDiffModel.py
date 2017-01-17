#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    findMinDiffModel.py
@author  Michael Behrisch
@date    2016-08-24
@version $Id$

This script takes the output file of the emission tests and tries to
find models most similar to a given model concerning the emission values.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2016-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import pprint

emissionFile = sys.argv[1]
for refModel in sys.argv[2:]:
    refValue = {}
    minDiff = {}
    findRef = False
    minRelDiff = None
    for line in open(emissionFile):
        l = line.split(":")
        if line[:7] == "Running":
            model = line.split()[1][1:-1]
            if model == refModel:
                findRef = True
        elif line[:7] == "Success":
            findRef = False
        elif findRef and len(l) > 1:
            refValue[l[0]] = float(l[1])
    for line in open(emissionFile):
        l = line.split(":")
        if line[:7] == "Running":
            model = line.split()[1][1:-1]
            relDiff = 0.
        elif model != refModel and line[:7] == "Success":
            if "HDV" in model:
                print(refModel, model, relDiff / len(minDiff), fuelDiff)
            if minRelDiff is None or relDiff < minRelDiff[0]:
                minRelDiff = (relDiff, model)
        elif model != refModel and len(l) > 1:
            emission = l[0]
            if emission != "length" and emission != "electricity":
                diff = float(l[1]) - refValue[emission]
                relDiff += abs(diff) / refValue[emission]
                if emission == "fuel":
                    fuelDiff = abs(diff) / refValue[emission]
                if emission not in minDiff or abs(diff) < abs(minDiff[emission][0]):
                    minDiff[emission] = (diff, model)
    print(refModel, minRelDiff[1], minRelDiff[0] / len(minDiff))
    pprint.pprint(refValue)
    pprint.pprint(minDiff)
