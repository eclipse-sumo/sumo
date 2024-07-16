#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @date    2024-07-16

"""
download matsim network then convert to sumo network
"""
import sys
import os
import subprocess
try:
    from urllib.request import urlopen
except ImportError:
    from urllib import urlopen
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', "tools"))
import sumolib  # noqa

urls = ["https://svn.vsp.tu-berlin.de/repos/public-svn/matsim/scenarios/countries/de/stuttgart/stuttgart-v2.0-10pct/input/matsim-stuttgart-v2.0.network.xml.gz",  # noqa
        "https://svn.vsp.tu-berlin.de/repos/public-svn/matsim/scenarios/countries/de/berlin/berlin-v6.3/input/berlin-v6.3-network.xml.gz"]  # noqa

for url in urls:
    with open("matsim.xml.gz", "wb") as out:
        out.write(urlopen(url).read())
    subprocess.call([sumolib.checkBinary('netconvert'), '--matsim', out.name,
                    "--xml-validation", "never", "-o", "tmp.net.xml", "--aggregate-warnings", "0"])
