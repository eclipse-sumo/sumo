"""
@file    real_world.py
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
import sumolib.net.generator.cross as netGenerator
import sumolib.net.generator.demand as demandGenerator
from sumolib.net.generator.network import *


class Scenario_RealWorld(Scenario):
    NAME = "RealWorld"
    THIS_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), NAME)

    def __init__(self, which, withDefaultDemand=True):
        Scenario.__init__(self, self.NAME + "/" + which)
        self.THIS_DIR = os.path.join(self.THIS_DIR, which)
        self.NAME = os.path.join(self.NAME, which)
        self.params = {}
        fd = open(os.path.join(self.THIS_DIR, "meta.txt"))
        for l in fd:
            l = l.strip()
            if len(l) == 0:
                continue
            (key, value) = l.split(":")
            self.params[key] = value
        fd.close()
        self.NET_FILE = os.path.join(self.THIS_DIR, self.params["net"])
        self.TLS_FILE = os.path.join(self.THIS_DIR, self.params["tls"])
        self.netName = self.fullPath(self.NET_FILE)
        self.demandName = os.path.join(self.THIS_DIR, self.params["rou"])
        for add in self.params["add"].split(","):
            self.addAdditionalFile(os.path.join(self.THIS_DIR, add))
        # network
        if fileNeedsRebuild(self.netName, "netconvert"):
            pass
        # demand
        print(withDefaultDemand)
        if not withDefaultDemand:
            print("Not supported")
