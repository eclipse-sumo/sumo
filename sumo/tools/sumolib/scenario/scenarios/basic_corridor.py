"""
@file    basic_corridor.py
@author  Daniel Krajzewicz
@date    2014-09-01
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function


from . import *
import os
import sumolib.net.generator.corridor as netGenerator
import sumolib.net.generator.demand as demandGenerator
from sumolib.net.generator.network import *


class Scenario_BasicCorridor(Scenario):
    NAME = "BasicCorridor"
    THIS_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), NAME)
    TLS_FILE = os.path.join(THIS_DIR, "tls.add.xml")
    NET_FILE = os.path.join(THIS_DIR, "corridor_%s.net.xml")

    def __init__(self, xoff, withDefaultDemand=True):
        Scenario.__init__(self, self.NAME)
        self.NET_FILE = self.NET_FILE % xoff
        self.netName = self.fullPath(self.NET_FILE)
        self.demandName = self.fullPath("routes.rou.xml")
        # network
        if fileNeedsRebuild(self.netName, "netconvert"):
            print("Network in '%s' needs to be rebuild" % self.netName)
            defaultEdge = Edge(numLanes=1, maxSpeed=13.89)
            defaultEdge.addSplit(100, 1)
            defaultEdge.lanes = [Lane(dirs="rs"), Lane(dirs="l")]
            netGen = netGenerator.corridor(5, None, defaultEdge)
            xpos = xoff
            for i in range(1, 6):
                netGen._nodes["%s/0" % i].x = xpos
                netGen._nodes["%s/1" % i].x = xpos
                netGen._nodes["%s/2" % i].x = xpos
                xpos = xpos + xoff
            netGen._nodes["6/1"].x = xpos
            # not nice, the network name should be given/returned
            netGen.build(self.netName)
        # demand
        if withDefaultDemand:
            print("Demand in '%s' needs to be rebuild" % self.demandName)
            self.demand = demandGenerator.Demand()
            # why isn't it possible to get a network and return all possible
            # routes or whatever - to ease the process
            self.demand.addStream(demandGenerator.Stream(
                None, 0, 3600, 1000, "6/1_to_5/1", "1/1_to_0/1", {.2: "hdv", .8: "passenger"}))
            if fileNeedsRebuild(self.demandName, "duarouter"):
                self.demand.build(0, 3600, self.netName, self.demandName)
