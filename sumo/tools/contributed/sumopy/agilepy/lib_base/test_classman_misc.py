"""
@file    test_classman_misc.py
@author  Joerg Schweizer
@date    
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2016-2017 DLR (http://www.dlr.de/) and contributors
SUMOPy module
Copyright (C) 2012-2017 University of Bologna - DICAM

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""

from classman import *
c = ColConf('streetname', 'xxx',
            groupnames=['state'],
            perm='rw',
            is_save=True,
            name='Street name',
            info='Name of the street',
            )
