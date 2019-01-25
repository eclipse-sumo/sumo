#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    removeDirs.py
# @author  Leonhard Luecken
# @date    2018-05-02
# @version $Id$

import os
import shutil

# ~ orig_fn = "/home/leo/repos/sumo_master/sumo/tests/complex/sumo/ToCDevice/spec/options.complex"
# ~ orig_fn = "/home/leo/repos/sumo_master/sumo/tests/complex/sumo/ToCDevice/runner.py"
orig_fn = "/home/leo/repos/sumo_master/sumo/tests/complex/sumo/ToCDevice/input_routes.rou.xml"
# ~ b = "options.complex"
# ~ b = "runner.py"
b = "input_routes.rou.xml"

dl = os.listdir()
for n in dl:
    print(n)
    if(os.path.isdir(n)):
        d = os.path.join(os.path.realpath(n), b)
        shutil.copy(orig_fn, d)
