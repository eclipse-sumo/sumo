#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    testscore.py
# @author  Jakob Erdmann
# @date    2025-05-21

"""
This script test the game scoring function
"""
from __future__ import absolute_import
from __future__ import print_function
import sys

import runner
from runner import _SCORING_FUNCTION


def main():
    gamename = sys.argv[1]
    runner._DEBUG = True
    score, totalArrived, complete = _SCORING_FUNCTION[gamename](gamename)
    print("score=%s totalCounted=%s complete=%s" % (
        score, totalArrived, complete))


if __name__ == "__main__":
    main()
