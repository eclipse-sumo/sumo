"""
@file    checkFlows.py
@author  Daniel Krajzewicz
@date    2014-09-11
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
@date    2014-09-01
"""

from __future__ import absolute_import
import sys
import sumolib.output
import helpers
from pylab import *


f = {}
pd = sumolib.output.parse(sys.argv[1], "vehicle")
for v in pd:
    t = int(float(v.depart))
    e = v["route"][0].edges.split(" ")[0]
    if e not in f:
        f[e] = [0] * 86400
    f[e][t] = f[e][t] + 1


AGG = 3600
fa = {}
for e in f:
    fa[e] = [0] * (86400 / AGG)
    for th in range(0, 86400 / AGG):
        for tl in range(0, AGG):
            fa[e][th] = fa[e][th] + f[e][th * AGG + tl]
ts = range(0, 86400 / AGG)


for e in f:
    plot(ts, fa[e], 'o-', label=e)
legend()
show()
