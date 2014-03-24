"""
@file    poi.py
@author  Jakob Erdmann
@author  Laura Bieker
@date    2014-02-13
@version $Id$

This module includes functions for converting SUMO's fcd-output into
pois (useful for showing synthetic GPS disturtbances)

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from collections import defaultdict
from sumolib.shapes.poi import PoI
from sumolib.color import RGBAColor
from sumolib.miscutils import Colorgen

LAYER = 100 # show above everything else

def fcd2poi(inpFCD, outSTRM, ignored):
    colors = defaultdict(lambda : RGBAColor(*Colorgen(("random", 1, 1)).byteTuple()))
    outSTRM.write("<pois>\n")
    for timestep in inpFCD:
        for v in timestep.vehicle:
            outSTRM.write("    %s\n" % PoI("%s_%s" % (v.id, timestep.time), v.id, LAYER, colors[v.id], v.x, v.y, lonLat=True).toXML())
    outSTRM.write("</pois>\n")


