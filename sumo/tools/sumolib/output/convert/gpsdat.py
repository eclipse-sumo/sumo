"""
@file    shawn.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

This module includes functions for converting SUMO's fcd-output into
data files read by Shawn.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
import datetime
import sumolib.output
import sumolib.net

TAXI_STATUS_FREE_FLOW = "70"

def fcd2gpsdat(inpFCD, outSTRM, further):
  date = further["base-date"]
  for timestep in inpFCD:
    if timestep.vehicle:
      mtime = str(date + datetime.timedelta(seconds=int(float(timestep.time)))) # does not work with subseconds
      for v in timestep.vehicle:
        print('\t'.join([v.id, mtime, str(v.x), str(v.y), TAXI_STATUS_FREE_FLOW,str(float(v.speed)*3.6)]), file=outSTRM)
