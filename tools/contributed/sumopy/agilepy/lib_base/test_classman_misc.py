# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2017 University of Bologna - DICAM
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test_classman_misc.py
# @author  Joerg Schweizer
# @date
# @version $Id$


from classman import *
c = ColConf('streetname', 'xxx',
            groupnames=['state'],
            perm='rw',
            is_save=True,
            name='Street name',
            info='Name of the street',
            )
