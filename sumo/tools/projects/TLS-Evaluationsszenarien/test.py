#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    test.py
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2009-06-30
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

from evaluator import *
from plotter import *


plotDiagram('data\\000')
plotDiagram('data\\001')
plotDiagram('data\\002')
plotDiagram('data\\003')
compareData('data\\000', 'data\\002', 'data\\000vs002')
compareData('data\\001', 'data\\003', 'data\\001vs003')


print(getSaturationFlow())
print(getMinTravelTime())
print(getAvgGreenTime(6, 6))


filename = "data\\001.dat"

X = load(filename)

flowWEA = X[:, 2]
flowNSA = X[:, 3]
avgDelayWEA = X[:, 4]
avgDelayNSA = X[:, 5]

[X, Y] = meshgrid(range(300, 1300, 100), range(300, 1300, 100))

flow = X[0]

Z = griddata(flowWEA, flowNSA, avgDelayWEA, X, Y)
delayVAWE = Z[0]
Z = griddata(flowWEA, flowNSA, avgDelayNSA, X, Y)
delayVANS = Z[0]


filename = "data\\003.dat"

X = load(filename)

flowWEA = X[:, 2]
flowNSA = X[:, 3]
avgDelayWEA = X[:, 4]
avgDelayNSA = X[:, 5]

[X, Y] = meshgrid(range(300, 1300, 100), range(300, 1300, 100))

Z = griddata(flowWEA, flowNSA, avgDelayWEA, X, Y)
delayFCWE = Z[0]
Z = griddata(flowWEA, flowNSA, avgDelayNSA, X, Y)
delayFCNS = Z[0]


figure(figsize=(12, 6))
subplot(1, 2, 1)
plot(flow, delayFCWE, flow, delayVAWE)
ylim(0, 60)
xlabel("Flow")
ylabel("Average Delay WE")
legend(("FC", "VA"), loc='upper left')
subplot(1, 2, 2)
plot(flow, delayFCNS, flow, delayVANS)
ylim(0, 60)
xlabel("Flow")
ylabel("Average Delay NS")
legend(("FC", "VA"), loc='upper left')
show()
