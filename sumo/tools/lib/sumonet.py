"""
@file    sumonet.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Michael Behrisch
@date    2008-03-27
@version $Id$

Deprecated version of sumolib/net.py.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, sys
print >> sys.stderr, "Using this library is deprecated, please use sumolib.net instead!"
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.net import *
