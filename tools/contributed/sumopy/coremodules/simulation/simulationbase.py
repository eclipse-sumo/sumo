# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2017 University of Bologna - DICAM
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    simulationbase.py
# @author  Joerg Schweizer
# @date
# @version $Id$


import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import random_choice, get_inversemap


class SimobjMixin:
    def prepare_sim(self, process):
        return []  # [(steptime1,func1),(steptime2,func2),...]

    def config_simresults(self, results):
        # tripresults = res.Tripresults(          'tripresults', results,
        #                                        self,
        #                                        self.get_net().edges
        #                                        )
        #
        #
        #results.config(tripresults, groupnames = ['Trip results'])
        pass

    def process_results(self, results, process=None):
        pass
