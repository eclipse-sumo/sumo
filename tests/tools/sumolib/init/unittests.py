#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    unittests.py
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

import sumolib  # noqa


class Test_Init(unittest.TestCase):

    def test_checkBinary(self):
        self.assertEqual(os.environ['SUMO_BINARY'], sumolib.checkBinary('sumo'))
        del os.environ['SUMO_BINARY']
        self.assertIn('sumo', sumolib.checkBinary('sumo'))
        del os.environ['SUMO_HOME']
        self.assertEqual('sumo', sumolib.checkBinary('sumo', ''))

    def test_intTime(self):
        self.assertEqual(1, sumolib._intTime('1.0'))
        self.assertEqual(1, sumolib._intTime('1.1'))
        self.assertEqual(1, sumolib._intTime('1.9'))


if __name__ == '__main__':
    unittest.main()
