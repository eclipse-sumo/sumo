#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2013-01-14
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
hbefa = collections.defaultdict(lambda:collections.defaultdict(list))
phem = collections.defaultdict(lambda:collections.defaultdict(list))
maxVal = collections.defaultdict(float)
for line in open('nefz_hbefa3/output.complex'):
    l = line.split(":")
    if line[:7] == "Running":
        ec = line.split('/')[1][:-2]
    elif len(l) > 1:
        val = float(l[1])
        hbefa[ec][l[0]].append(val)
        maxVal[l[0]] = max(maxVal[l[0]], val)
for line in open('nefz_phemlightV4/output.complex'):
    l = line.split(":")
    if line[:7] == "Running":
        ec = line.split('/')[1][:-2]
        if ec[:3] == "PKW":
            ec = "PC" + ec[3:]
        if ec[:3] == "LNF":
            ec = "LDV" + ec[3:ec.rfind("_")]
    elif len(l) > 1:
        val = float(l[1])
        phem[ec][l[0]].append(val)
        if ec in hbefa and len(hbefa[ec][l[0]]) < len(phem[ec][l[0]]):
            hbefa[ec][l[0]].append(hbefa[ec][l[0]][0])
        maxVal[l[0]] = max(maxVal[l[0]], val)

marker = {"PC_G_": "s", "PC_D_": "^", "LDV_G": "o", "LDV_D": "v"}
#color = {"0": "k", "1": "r", "2": "y", "3": "g", "4": "m", "5": "b", "6": "c"}
color = {"0": "0", "1": "0.3", "2": "0.45", "3": "0.6", "4": "0.75", "5": "0.9", "6": "1"}
limit = {"CO": 100000, "CO2": 5e6, "HC": 30000, "PMx": 6000, "fuel": 1700}

for emission in ["CO", "CO2", "HC", "PMx", "fuel"]:
    print(emission)
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    if emission in limit:
        m = limit[emission]
    else:
        m = maxVal[emission]
    if emission == "fuel":
        plt.xlim(500, m)
        plt.ylim(500, m)
    else:
        plt.xlim(0, m)
        plt.ylim(0, m)
    for ec in sorted(phem.iterkeys()):
        if ec in hbefa and hbefa[ec][emission][0] > 0:
            print(ec, phem[ec][emission], hbefa[ec][emission])
            ax1.scatter(phem[ec][emission], hbefa[ec][emission], s=80, c=color[ec[-1]], marker=marker[ec[:5]], label=ec)
    ax1.plot((0, m), (0, m))
    plt.title(emission + (" [ml]" if emission == "fuel" else " [mg]"))
    plt.xlabel("PHEMlight")
    plt.ylabel("HBEFA3")
    plt.legend(scatterpoints=1, fontsize=8, bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0.)
    plt.savefig(emission+".png", bbox_inches="tight")
    plt.xlim(0, m/5)
    plt.ylim(0, m/5)
    plt.savefig(emission+"Zoom.png", bbox_inches="tight")
