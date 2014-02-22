#!/usr/bin/env python
"""
@file    testEdgesInDistrict.py
@author  Michael.Behrisch@dlr.de
@date    2007-07-18
@version $Id$

Simple Unittest to check whether numpy is available and does not have the
polyfit bug showing up in the 1.0.4 release

Copyright (C) 2007-2014 DLR, Germany
All rights reserved
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
