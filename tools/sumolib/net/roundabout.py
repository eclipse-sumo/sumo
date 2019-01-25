# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    roundabout.py
# @author  Daniel Krajzewicz
# @author  Laura Bieker
# @author  Karol Stosiek
# @author  Michael Behrisch
# @date    2011-11-28
# @version $Id$


class Roundabout:

    def __init__(self, nodes, edges=None):
        self._nodes = nodes
        self._edges = edges

    def getNodes(self):
        return self._nodes

    def getEdges(self):
        return self._edges
