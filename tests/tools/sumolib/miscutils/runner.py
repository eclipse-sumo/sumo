#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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

# @file    runner.py
# @author  Michael Behrisch
# @date    2019-01-09

from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import unittest

# Do not use SUMO_HOME here to ensure you are always testing the
# functions from the same tree the test is in
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools'))

from sumolib import miscutils  # noqa


class Test_Init(unittest.TestCase):

    def test_parseTime(self):
        self.assertEqual(42, miscutils.parseTime('00:00:42'))
        self.assertEqual(3661, miscutils.parseTime('01:01:01'))
        self.assertEqual(25 * 3600 + 61, miscutils.parseTime('1:1:1:1'))
        self.assertEqual(-1, miscutils.parseTime('-00:00:01'))

    def test_humanReadableTime(self):
        hrt = miscutils.humanReadableTime
        self.assertEqual(hrt(42), '00:00:42')
        self.assertEqual(hrt(3661), '01:01:01')
        self.assertEqual(hrt(25 * 3600 + 61), '1:01:01:01')
        self.assertEqual(hrt(-1), '-00:00:01')


if __name__ == '__main__':
    unittest.main()
