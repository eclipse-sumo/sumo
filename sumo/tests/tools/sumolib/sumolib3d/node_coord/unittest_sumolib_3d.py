#!/usr/bin/env python
"""
@file    unittest_sumolib_3d.py
@author  Marek Heinrich
@version $Id$

This script tests sumolib 3D functions

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2016-2016 DLR (http://www.dlr.de/) and contributors

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
import subprocess

import unittest

# Do not use SUMO_HOME here to ensure you are always testing the
# functions from the same tree the test is in
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', '..', 'tools'))

import sumolib 

NODEFILE = 'input_nodes.nod.xml'
EDGEFILE = 'input_edges.edg.xml'
NETFILE  = 'input_net.net.xml'

class Test_3D_Coords(unittest.TestCase):
    """ Tests to check inport of sumo elements with z coords. """
        
    @classmethod
    def setUpClass(cls):
        """ setup generates all sumo files - once. """
        
        netcon_bin = sumolib.checkBinary('netconvert')
        
        command    = [netcon_bin,
                      "-n", NODEFILE,
                      "-e", EDGEFILE,
                      "-o", NETFILE,
                      "--offset.disable-normalization"]

        netconvertProcess = subprocess.call(
            command,
            stdout=sys.stdout,
            stderr=sys.stderr)
    
        cls.sumo_net = sumolib.net.readNet(
            NETFILE,
            withInternal=True)

    @classmethod
    def tearDownClass(cls):
        """ remove the generated net file, once all tests ran """

        if os.path.exists(NETFILE):
            os.remove(NETFILE)
        
        
    @unittest.skipIf(False, '')
    def test_check_node_x_y(self):
        """ test to retrive the coords from a node with z=0 
            
            - should return a 2d coords tuple eventhough the net 
              is with z coords"""

        self.assertEqual(
            self.sumo_net.getNode('first').getCoord(),
            (100.0, 0.0))
        
    
    @unittest.skipIf(False, '')
    def test_check_node_x_y_z(self):
        """ test to retrive the coords from a node with z!=0

            - should be a 3d coords tuple"""

        self.assertEqual(
            self.sumo_net.getNode('second').getCoord(),
            (200.0, 0.0, 10.0))

        


if __name__ == '__main__':
    unittest.main()

