#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-10-28
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import vehicleControl
import simpleManager
import agentManager
import createNet  # build/check network

# perform simple scenario
vehicleControl.init(simpleManager.SimpleManager(), True)
# perform agent scenario
vehicleControl.init(agentManager.AgentManager(), True)
