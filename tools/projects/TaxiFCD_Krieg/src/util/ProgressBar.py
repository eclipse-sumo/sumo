# -*- coding: Latin-1 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    ProgressBar.py
# @author  Sascha Krieg
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2008-06-19
# @version $Id$

from __future__ import print_function


def startTask(list, fct):
    """its a simple progress bar for the console.
    list=list of elements over which would be iterated
    fct= function which is called for every element of the list
    """
    listLen = len(list)
    lastProz = 0
    for i in range(5, 105, 5):
        s = "%02d" % i
        print(s, end=' ')
    print("%")

    for i in range(listLen):
        actProz = (100 * i / listLen)
        if actProz != lastProz and actProz % 5 == 0:
            print("**", end=' ')
            lastProz = actProz
        # call the function
        fct(list[i])
