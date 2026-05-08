"""
hybridPY_add_odflows is a script to open and add OD flows and do the routing

Usage: python hybridPY_add_odflows.py ../demo_od_turnflows/demo.obj

"""
import sys, os
import numpy as np

# point to the hybridPY directory here
sys.path.append(os.path.join('..','..'))

from coremodules.scenario import scenario
from coremodules.simulation import sumo
from agilepy.lib_base.logger import Logger
import coremodules.demand.demandbase as db

if len(sys.argv)>=2: 
    simfilepath = sys.argv[1]
else:
    print(__doc__)
    sys.exit(0)


myscenario = scenario.load_scenario(simfilepath)
rootfilepath = myscenario.get_rootfilepath()

mylogger = Logger(#filepath = os.path.join(dirpath,logfilepath),
                  is_stdout = True,#False
                  )

mydemand = myscenario.demand
modename = 'delivery'
mylogger.w('Add OD flows with mode '+modename)

from coremodules.demand.origin_to_destination_wxgui import AddOdWizzard
from coremodules.demand.origin_to_destination import OdTripgenerator

odmadder = AddOdWizzard(mydemand.odintervals,
                                t_start = 0,
                                t_end = 3600,
                                id_mode = myscenario.net.modes.get_id_mode(modename),
                                scale = 2.0,
                                )

odmadder.import_csv( os.path.join('..','demo_od_turnflows','demo_dem_od_bikes.csv'), sep = ",")
odmadder.add_demand()
        
OdTripgenerator(mydemand.odintervals, logger = mylogger,
                        is_clear_trips = False,
                        is_refresh_zoneedges = True, # identifies zone edges
                        #is_make_route = True,# will do routing later
                        priority_max = 9,
                        speed_max = 14.0,
                        pos_depart = db.OPTIONMAP_POS_DEPARTURE['last'], 
                        pos_arrival = db.OPTIONMAP_POS_ARRIVAL['random'],
                        ind_lane_depart = db.OPTIONMAP_LANE_DEPART['allowed'],
                        ind_lane_arrival = db.OPTIONMAP_LANE_ARRIVAL['current'],
                        speed_depart = db.OPTIONMAP_SPEED_DEPARTURE['max'],
                        speed_arrival = db.OPTIONMAP_SPEED_ARRIVAL['current'],
                        ).do()
                        
# route with shortest path
mydemand.trips.duaroute()
        
myscenario.save(rootfilepath+'_with_odflows.obj')
