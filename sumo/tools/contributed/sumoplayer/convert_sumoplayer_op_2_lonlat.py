#!/usr/bin/env python
"""
@file    convert_sumoplayer_op_2_lonlat.py
@author  Sandesh Uppoor
@author  Laura Bieker
@author  Michael Behrisch
@date    2011-05-30
@version $Id$

This code takes the sumoplayer output as input file . The sumoplayer output has the syntax : time, vehicle_id , x, y , speed(meters). if anyone want to check the real location on the openstreetmap, he need to convert x,y back to lon lat format , This code does that for you . 

O/P syntax : time , vehicle_id , lon ,lat ,speed(meters)

IMP-->Makesure you have installed python and pyproj package to run this code
command : python convert_sumoplayer_op_2_lonlat.py sumoplayer_output.txt

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys
from pyproj import Proj
from string import split
if __name__=="__main__":

    f=open(sys.argv[1],'r')
    while True:
        
        x=f.readline()
        if x is None:
            break
        else:
            r=split(x,' ')
            player_x=r[2]
            player_y=r[3]
            time_=r[0]
            vid=r[1]
            speed=r[4]
            offset_x=-342498.94
            offset_y=-5630725.14
            actual_x= float(player_x) - offset_x
            actual_y= float(player_y) - offset_y

            p=Proj(proj='utm',zone=32,ellps='WGS84')
            lat,lon=p(actual_x,actual_y,inverse=True)
            print time_,vid,lon,lat,speed
    exit
