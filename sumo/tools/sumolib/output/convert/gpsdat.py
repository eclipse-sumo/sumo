"""
@file    shawn.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

This module includes functions for converting SUMO's fcd-output into
data files read by Shawn.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sumolib.output
import sumolib.net
from datetime import datetime, timedelta
TAXI_STATUS_FREE_FLOW = "70"

def fcd2gpsdat(inpFCD, outSTRM, further):
  date = further["base-date"]
  for timestep in inpFCD:
    if timestep.vehicle:
      mtime = str(date + timedelta(seconds=int(float(timestep.time)))) # does not work with subseconds
      for v in timestep.vehicle:
        print >> outSTRM, '\t'.join([v.id, mtime, str(v.x), str(v.y), TAXI_STATUS_FREE_FLOW,str(float(v.speed)*3.6)])
