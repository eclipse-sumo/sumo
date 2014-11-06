#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2013-01-14
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys,os,subprocess
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', "tools"))
from sumolib import checkBinary

EDC = checkBinary("emissionsDrivingCycle", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', "bin"))
if len(sys.argv) > 2:
    PHEMLIGHTp = os.path.join(os.environ["SUMO_HOME"], "data", "emissions", sys.argv[2])
else:
    PHEMLIGHTp = os.path.join(os.environ["SUMO_HOME"], "data", "emissions", "PHEMlight")
if not os.path.exists(PHEMLIGHTp):
    PHEMLIGHTp = os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', "data", "emissions", "PHEMlight")

fd = open("classes.txt")
emissionClasses = fd.readlines()
fd.close() 

fdo = open("results.csv", "w")
for i,ec in enumerate(emissionClasses):
    ec = ec.strip()
    if len(ec) == 0:
      continue
    drivingCycle = sys.argv[1]
    print "Running '%s'" % ec
    sys.stdout.flush()
    sys.stderr.flush()
    call = [EDC, "-e", ec, "-t", drivingCycle, "-o", "tmp.csv", "--phemlight-path", PHEMLIGHTp, "--kmh", "--compute-a"]
    retCode = subprocess.call(call)
    sys.stdout.flush()
    sys.stderr.flush()
    if retCode != 0:
        print "Error on building PHEMlight measurements"
        sys.exit(1)
    fd = open("tmp.csv")
    out = fd.readlines()
    fd.close()
    fdo.write("%s\n" % ec)
    for l in out:
      fdo.write(l)
    fdo.write("-----\n\n")
fdo.close()

