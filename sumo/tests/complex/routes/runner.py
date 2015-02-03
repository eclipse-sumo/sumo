#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2015-02-03
@version $Id$

This script is a test runner for the route input tests.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
import os, subprocess, sys
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', "tools"))
from sumolib import checkBinary

if sys.argv[1] == "sumo":
    call = [checkBinary('sumo'), "--no-step-log", "--no-duration-log",
            "-a", "input_additional.add.xml"]
elif sys.argv[1] == "dfrouter":
    call = [checkBinary('dfrouter'),
            "--detector-files", "input_additional.add.xml"]
elif sys.argv[1] == "duarouter":
    call = [checkBinary('duarouter'), "--no-step-log",
            "-o", "dummy.xml", "-d", "input_additional.add.xml"]
elif sys.argv[1] == "jtrrouter":
    call = [checkBinary('jtrrouter'), "--no-step-log",
            "-o", "dummy.xml", "-a", "input_additional.add.xml"]
else:
    print >> sys.stderr, "Unsupported application defined"
call += sys.argv[2:]

elements = [('vehicle', 'depart="0"'), ('flow', 'begin="0" end="1" number="1"'), ('trip', 'depart="0"')]
attrs = [' from="1fi"', ' to="2si"', ' fromTaz="1"', ' toTaz="2"', ' route="a"']
childs = ['<route edges="1fi 1si 2o 2fi 2si"/>\n', '<stop lane="1fi_0" duration="10"/>\n',
          '<stop lane="1si_0" duration="10"/>\n', '<stop lane="2si_0" duration="10"/>\n']

# check route processing
for elem, depart in elements:
    for attrSet in range(2 ** len(attrs)):
        for childSet in range(2 ** len(childs)):
            print(elem, attrSet, childSet)
            sys.stdout.flush()
            with open("routes.xml", "w") as routes:
                routes.write('<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/routes_file.xsd">\n    <route id="a" edges="1fi 1si 2o 2fi 2si"/>\n    <%s id="v" %s' % (elem, depart))
                for idx, attr in enumerate(attrs):
                    if attrSet & idx:
                        routes.write(attr)
                routes.write('>\n')
                for idx, child in enumerate(childs):
                    if childSet & idx:
                        routes.write(child)
                routes.write('    </%s>\n</routes>\n' % elem)
            retCode = subprocess.call(call + ["-n", "input_net.net.xml", "-r", routes.name], stdout=sys.stdout, stderr=sys.stdout)
            if retCode < 0:
                sys.stdout.write(open(routes.name).read())
                sys.exit()
