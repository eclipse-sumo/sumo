# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    hybridpy_add_odflows.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

"""
hybridPY_add_odflows is a script to open and add OD flows and do the routing

Usage: python hybridPY_add_odflows.py ../demo_od_turnflows/demo.obj

"""
from coremodules.demand.origin_to_destination import OdTripgenerator
from coremodules.demand.origin_to_destination_wxgui import AddOdWizzard
import coremodules.demand.demandbase as db
from agilepy.lib_base.logger import Logger
from coremodules.simulation import sumo
from coremodules.scenario import scenario
import sys
import os
import numpy as np

# point to the hybridPY directory here
sys.path.append(os.path.join('..', '..'))


if len(sys.argv) >= 2:
    simfilepath = sys.argv[1]
else:
    print(__doc__)
    sys.exit(0)


myscenario = scenario.load_scenario(simfilepath)
rootfilepath = myscenario.get_rootfilepath()

mylogger = Logger(  # filepath = os.path.join(dirpath,logfilepath),
    is_stdout=True,  # False
)

mydemand = myscenario.demand
modename = 'delivery'
mylogger.w('Add OD flows with mode '+modename)


odmadder = AddOdWizzard(mydemand.odintervals,
                        t_start=0,
                        t_end=3600,
                        id_mode=myscenario.net.modes.get_id_mode(modename),
                        scale=2.0,
                        )

odmadder.import_csv(os.path.join('..', 'demo_od_turnflows', 'demo_dem_od_bikes.csv'), sep=",")
odmadder.add_demand()

OdTripgenerator(mydemand.odintervals, logger=mylogger,
                is_clear_trips=False,
                is_refresh_zoneedges=True,  # identifies zone edges
                # is_make_route = True,# will do routing later
                priority_max=9,
                speed_max=14.0,
                pos_depart=db.OPTIONMAP_POS_DEPARTURE['last'],
                pos_arrival=db.OPTIONMAP_POS_ARRIVAL['random'],
                ind_lane_depart=db.OPTIONMAP_LANE_DEPART['allowed'],
                ind_lane_arrival=db.OPTIONMAP_LANE_ARRIVAL['current'],
                speed_depart=db.OPTIONMAP_SPEED_DEPARTURE['max'],
                speed_arrival=db.OPTIONMAP_SPEED_ARRIVAL['current'],
                ).do()

# route with shortest path
mydemand.trips.duaroute()

myscenario.save(rootfilepath+'_with_odflows.obj')
