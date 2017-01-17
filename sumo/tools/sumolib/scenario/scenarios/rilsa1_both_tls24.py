"""
@file    rilsa1_both_tls24.py
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


from . import *
import os
import shutil
import sumolib.net.generator.demand as demandGenerator
from sumolib.net.generator.network import *


flowsRiLSA1 = [
    ["nmp1", [
        ["ms2", 359, 9],
        ["me2", 59, 9],
        ["mw2", 64, 12]
    ]],

    ["wmp1", [
        ["me2", 508, 10],
        ["mn2", 80, 14],
        ["ms2", 130, 2]
    ]],

    ["emp1", [
        ["mw2", 571, 10],
        ["mn2", 57, 9],
        ["ms2", 47, 3]
    ]],

    ["smp1", [
        ["mn2", 354, 2],
        ["me2", 49, 2],
        ["mw2", 92, 2]
    ]]

]


class Scenario_RiLSA1BothTLS24(Scenario):
    NAME = "RiLSA1BothTLS24"
    THIS_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), NAME)
    TLS_FILE = "tls.add.xml"
    NET_FILE = "rilsa1both.net.xml"

    def __init__(self, params, withDefaultDemand=True):
        Scenario.__init__(self, self.THIS_DIR)
        self.params = params
        if "equipment-rate" not in self.params:
            self.params["equipment-rate"] = 1
        # network
        if fileNeedsRebuild(os.path.join(self.THIS_DIR, self.NET_FILE), "netconvert"):
            netconvert = sumolib.checkBinary("netconvert")
            retCode = subprocess.call(
                [netconvert, "-c", os.path.join(self.THIS_DIR, "build.netc.cfg")])
        # build the demand model (streams)
        if withDefaultDemand:
            self.demand = demandGenerator.Demand()
            for f in flowsRiLSA1:
                for rel in f[1]:
                    prob = rel[2] / 100.
                    iprob = 1. - prob

                    pkwEprob = iprob * self.params["equipment-rate"]
                    pkwNprob = iprob - pkwEprob
                    lkwEprob = prob * self.params["equipment-rate"]
                    lkwNprob = prob - lkwEprob

                    self.demand.addStream(demandGenerator.Stream(f[0] + "__" + rel[0], 0, 3600, rel[1], f[0], rel[0],
                                                                 {"passenger": pkwEprob, "COLOMBO_undetectable_passenger": pkwNprob, "hdv": lkwEprob, "COLOMBO_undetectable_hdv": lkwNprob}))
            if fileNeedsRebuild(self.fullPath("routes.rou.xml"), "duarouter"):
                self.demand.build(
                    0, 3600, self.fullPath(self.NET_FILE), self.fullPath("routes.rou.xml"))
            self.demandName = self.fullPath("routes.rou.xml")

        self.netName = self.sandboxPath(self.NET_FILE)
        shutil.copy(
            self.fullPath(self.NET_FILE), self.sandboxPath(self.NET_FILE))
        #shutil.copy(self.fullPath(self.TLS_FILE), self.sandboxPath(self.TLS_FILE))
