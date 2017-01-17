#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    readPlotOLD.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-07-26
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import sys
from pylab import *


#index = int(sys.argv[2])

# 0: Frequenz; ->belegt
# 1: Ausstattungsgrad; ->belegt
# 2: ausgewaehlterFhzg; ->nein
# 3: Proben; ->?was ist das
# 4: erkannter Kanten; ->abs Wert rel Besser
# 5: nichtErkannterKanten; ->abs Wert rel Besser
# 6: edgeSmaples(immer gleich);> ->?was ist das
# 7: FhzgSamples; ->nein
# 8: GeschwEdge; ->nein
# 9: GeschwFhzg; ->nein
# 10: absDiffGeschw; ->rel besser
# 11: relDiffGeschw; ->ja
# 12: %erkannter Kanten; ->darstellung 13 besser doch 12
# 13: %nicht erkannterKanten ->ja
index = 11
fd = open(sys.argv[1])
# absSwitch=int(sys.argv[3])
absSwitch = 1
vDict = {}
xt = []
yt = []
mmin = None
mmax = None
for line in fd:
    line = line.strip()
    vals = line.split(";")
    i1 = float(vals[0])
    i2 = float(vals[1])
    if i1 not in vDict:
        vDict[i1] = {}
    v = float(vals[index])
    if absSwitch == 1:
        v = abs(v)
    vDict[i1][i2] = v

    if i1 not in xt:
        xt.append(i1)
    if len(xt) == 1:
        yt.append(i2)
    if not mmin or mmin > v:
        mmin = v
    if not mmax or mmax > v:
        mmax = v

marr = []
for i1 in xt:
    marr.append([])
    for i2 in yt:
        marr[-1].append(vDict[i1][i2])


textsize = 18
# levels=arange(mmin-mmin*.1, mmax+mmax*.1, (mmax-mmin)/10.))
contourf(marr, 20)
# colorbar()
# set fontsize:
cb = colorbar()  # grab the Colorbar instance
for t in cb.ax.get_yticklabels():
    t.set_fontsize(textsize)

yticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10], xt, size=textsize)
ylabel("Periode [s]", size=textsize)
xticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10], yt, size=textsize)
xlabel("Ausstattung [%]", size=textsize)
title("Abweichung der Geschwindigkeit zwischen FCD und des simulierten Verkehrs",
      size=textsize)
#title("Relative Anzahl erfasster Kanten", size=textsize)
figtext(0.7865, 0.92, '[%]', size=textsize)
show()
