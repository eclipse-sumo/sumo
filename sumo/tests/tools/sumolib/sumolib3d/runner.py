#!/usr/bin/env python
"""
"""

from __future__ import absolute_import
from __future__ import print_function


import os
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))

import unittest

from unittest_sumolib_3d import Test_3D_Coords

if __name__ == '__main__':
    unittest.main()
