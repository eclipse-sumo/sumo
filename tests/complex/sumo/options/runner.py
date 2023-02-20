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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2012-08-10

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


sumoBinary = sumolib.checkBinary('sumo')


def call(comment, args):
    print(comment, args)
    sys.stdout.flush()
    subprocess.call([sumoBinary, "-c", "sumo.sumocfg", "-C", "stdout"] + args, stderr=subprocess.STDOUT)
    print("-" * 10)


call("plain", [])
call("replace", ["-a", "new.rou.xml"])
call("replace", ["--a", "new.rou.xml"])
call("append+", ["+a", "new.rou.xml"])
call("append++", ["++a", "new.rou.xml"])
call("append+-", ["+-a", "new.rou.xml"])
call("append-+", ["-+a", "new.rou.xml"])
call("replace", ["-additional", "new.rou.xml"])
call("replace", ["--additional", "new.rou.xml"])
call("append+", ["+additional", "new.rou.xml"])
call("append++", ["++additional", "new.rou.xml"])
call("append+-", ["+-additional", "new.rou.xml"])
call("append-+", ["-+additional", "new.rou.xml"])

call("replace", ["-a=new.rou.xml"])
call("replace", ["--a=new.rou.xml"])
call("append+", ["+a=new.rou.xml"])
call("append++", ["++a=new.rou.xml"])
call("append+-", ["+-a=new.rou.xml"])
call("append-+", ["-+a=new.rou.xml"])
call("replace", ["-additional=new.rou.xml"])
call("replace", ["--additional=new.rou.xml"])
call("append+", ["+additional=new.rou.xml"])
call("append++", ["++additional=new.rou.xml"])
call("append+-", ["+-additional=new.rou.xml"])
call("append-+", ["-+additional=new.rou.xml"])
