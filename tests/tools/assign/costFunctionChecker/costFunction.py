# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    costFunction.py
# @author  Michael Behrisch
# @date    2009-09-08
# @version $Id$



def constant(edge, weight):
    if edge == "middle" or edge == "left":
        return weight
    return 0


def w(edge, weight):
    if edge == "middle":
        return weight + 2
    if edge == "left":
        return weight
    return 1
