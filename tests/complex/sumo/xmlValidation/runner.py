#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @date    2022-09-28

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')

subprocess.call([sumolib.checkBinary('netgenerate'), '--grid', '--no-internal-links', '-o', 'n.net.xml'])
env = dict(os.environ)
for scheme, elem in ((None, "trip"), (None, "tri"), ("blub", "trip"), ("blub", "tri")):
    with open("routes.rou.xml", "w") as routes:
        sumolib.xml.writeHeader(routes, root="routes", schemaPath=scheme)
        routes.write('    <%s id="0" depart="0" from="A0A1" to ="B0B1"/>\n</routes>' % elem)
    for home in (None, os.environ["SUMO_HOME"], "http://sumo.dlr.de", "https://sumo.dlr.de", "blafasel"):
        if home is None:
            del env["SUMO_HOME"]
        else:
            env["SUMO_HOME"] = home
        for valid in ("never", "local", "auto", "always"):
            if home == os.environ["SUMO_HOME"]:
                home = "<environ>"
            print("XML schema:", scheme, "XML element:", elem, "SUMO_HOME:", home, "--xml-validation", valid)
            sys.stdout.flush()
            subprocess.call([sumoBinary, "-n", "n.net.xml", "-r", routes.name, "-X", valid, "--no-step-log"],
                            stderr=subprocess.STDOUT, env=env)
            sys.stdout.flush()
