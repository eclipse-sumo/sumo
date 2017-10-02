#!/usr/bin/env python
# -*- coding: Latin-1 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    GenerateTaxiRoutesMain.py
# @author  Sascha Krieg
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2008-04-17
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

from .GenerateTaxiRoutes import *


def main():
    print("start program")
    readFCD()
    writeRoutes()
    print("end")

# start the program
main()
