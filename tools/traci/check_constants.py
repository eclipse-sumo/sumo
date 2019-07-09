#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2019-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    check_constants.py
# @author  Jakob Erdmann
# @date    2019-06-08
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
from collections import defaultdict
import constants

occ = defaultdict(list)
for c, val in constants.__dict__.items():
    if isinstance(val, int):
        occ[val].append(c)

print("Duplicate constant use:")
for val, clist in occ.items():
    if len(clist) > 1:
        print("%s : %s" % (hex(val), ' '.join(sorted(clist))))

print("unused constants:")
for i in range(257):
    if len(occ[i]) == 0:
        print(hex(i))
