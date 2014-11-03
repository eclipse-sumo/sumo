#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@date    2010-09-06
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,subprocess,sys

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', 'bin', 'sumo'))
for dep1 in range(10):
    for dep2 in range(10):
        for stop in range(1,10):
            routes = open("input_routes.rou.xml", 'w')
            print >> routes, """
<routes>
    <route id="left" edges="1i 4o 4i 2o 2i 3o 3i 1o 1i"/>
    <vehicle id="l" route="left" depart="%s"/>
    <route id="vertical" edges="3i 4o 4i 3o 3i"/>
    <vehicle id="v" route="vertical" depart="%s">
        <stop lane="4o_0" endPos="%s"/>
    </vehicle>
</routes>""" % (dep1, dep2, stop)
            routes.close()
            subprocess.call([sumoBinary]+sys.argv[1:], shell=(os.name=="nt"), stdout=sys.stdout, stderr=sys.stderr)
