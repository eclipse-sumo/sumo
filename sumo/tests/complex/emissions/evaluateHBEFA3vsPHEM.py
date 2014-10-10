#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2013-01-14
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys, os, collections
import matplotlib.pyplot as plt
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', "tools"))

ec = None
hbefa = collections.defaultdict(dict)
phem = collections.defaultdict(dict)
maxVal = collections.defaultdict(float)
for line in open('nefz_hbefa3/output.complex'):
    l = line.split(":")
    if line[:7] == "Running":
        ec = line.split('/')[1][:-2]
    elif len(l) > 1:
        val = float(l[1])
        hbefa[ec][l[0]] = val
        maxVal[l[0]] = max(maxVal[l[0]], val)
for line in open('nefz_phemlightV3/output.complex'):
    l = line.split(":")
    if line[:7] == "Running":
        ec = line.split('/')[1][:-2]
        if ec[:3] == "PKW":
            ec = "PC" + ec[3:]
    elif len(l) > 1:
        val = float(l[1])
        phem[ec][l[0]] = val
        maxVal[l[0]] = max(maxVal[l[0]], val)

marker = {"PC_G": "s", "PC_D": "^"}
color = {"0": "k", "1": "r", "2": "y", "3": "g", "4": "m", "5": "b", "6": "c"}
limit = {"CO": 100000, "CO2": 5e6}
        
for idx, emission in enumerate(["CO", "CO2"]):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    plt.xlim(0, limit[emission])
    plt.ylim(0, limit[emission])
    for ec in sorted(phem.iterkeys()):
        if ec in hbefa and hbefa[ec][emission] > 0:
            print(ec, [phem[ec][emission]], [hbefa[ec][emission]])
            ax1.scatter([phem[ec][emission]], [hbefa[ec][emission]], s=30, c=color[ec[-1]], marker=marker[ec[:4]], label=ec)
    ax1.plot((0, limit[emission]), (0, limit[emission]))
    plt.xlabel("PHEMlight")
    plt.ylabel("HBEFA3")
    plt.legend(scatterpoints=1, fontsize=8)
    plt.savefig(emission+".png")
