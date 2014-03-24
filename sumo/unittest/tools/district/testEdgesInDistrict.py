#!/usr/bin/env python
"""
@file    testEdgesInDistrict.py
@author  Michael Behrisch
@date    2013-10-16
@version $Id$

Simple Unittest to check whether numpy is available and does not have the
polyfit bug showing up in the 1.0.4 release

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, sys, unittest
TOOLS_DIR = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'tools', 'district')
sys.path.append(TOOLS_DIR)
import edgesInDistricts

class TestEdgesInDistrict(unittest.TestCase):
    def testPointInPoly(self):
        convex = [(0,0), (2,0), (2,2), (0,2)]
        self.assertEqual(edgesInDistricts.isWithin((1,1), convex), True)
        self.assertEqual(edgesInDistricts.isWithin((-1,-1), convex), False)
        self.assertEqual(edgesInDistricts.isWithin((3,3), convex), False)
        self.assertEqual(edgesInDistricts.isWithin((1.5,1.5), convex), True)
        concave = [(0,0), (2,0), (2,1), (1,1), (1,2), (0,2)]
        self.assertEqual(edgesInDistricts.isWithin((0.5,0.5), concave), True)
        self.assertEqual(edgesInDistricts.isWithin((1.5,1.5), concave), False)
        

if __name__ == '__main__':
    unittest.main()
