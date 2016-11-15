import sys
import os
import subprocess

import unittest

sys.path.append(os.path.join(os.environ.get("SUMO_HOME"), 'tools'))

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

