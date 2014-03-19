#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2011-06-15
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, subprocess, sys
sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))

def call(command):
    retCode = subprocess.call(command, stdout=sys.stdout, stderr=sys.stderr)
    if retCode != 0:
        print >> sys.stderr, "Execution of %s failed." % command
        sys.exit(retCode) 

PERIOD = 5
DEPARTSPEED = "max"

fdo = open("results.csv", "w")
for departPos in "random free random_free base pwagSimple pwagGeneric maxSpeedGap".split():
    print ">>> Building the routes (for departPos %s)" % departPos
    fd = open("input_routes.rou.xml", "w")
    print >> fd, """<routes>
    <flow id="vright" route="right" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
    <flow id="vleft" route="left" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
    <flow id="vhorizontal" route="horizontal" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
</routes>""" % (3*(departPos, DEPARTSPEED, PERIOD))
    fd.close()

    print ">>> Simulating ((for departPos %s)" % departPos
    call([sumoBinary, "-c", "sumo.sumocfg", "-v"])

    dump = sumolib.output.dump.readDump("aggregated.xml", ["entered"])
    print >> fdo, "%s;%s" % (departPos, dump.get("entered")[-1]["1si"])
    
    if os.path.exists(departPos + "_aggregated.xml"):
        os.remove(departPos + "_aggregated.xml")
    os.rename("aggregated.xml", departPos + "_aggregated.xml")
fdo.close()
