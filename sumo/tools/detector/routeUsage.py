#!/usr/bin/env python
"""
@file    routeUsage.py.py
@author  Jakob Erdmann
@date    2017-03-30
@version $Id: routeUsage.py.py 23688 2017-03-28 12:16:12Z behrisch $

This script computes route usage statistics from emitters.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2007-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
from optparse import OptionParser

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    import sumolib
    from sumolib.output import parse, parse_fast
    from sumolib.miscutils import Statistics
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")



def get_options():
    USAGE = """Usage %prog <emitters.xml>"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--threshold", type="int", default=0, 
            help="Output routes that are used less than the threshold value")
    optParser.add_option("--unused-output", 
            help="Output route ids that are used less than the threshold value to file")
    options, args = optParser.parse_args()

    if len(args) != 1:
        sys.exit(USAGE)
    options.emitters = args[0]
    return options

def main():
    options = get_options()
    routeUsage = defaultdict(lambda: 0)
    for flow in parse(options.emitters, 'flow'): 
        num = int(flow.number)
        if flow.route is None:
            dist = flow.routeDistribution[0]
            probs = map(float, dist.probabilities.split())
            probs = [p / sum(probs) for p in probs]
            for rID, p in zip(dist.routes.split(), probs):
                routeUsage[rID] += p * num
        else:
            routeUsage[flow.route] += num

    usage = Statistics("routeUsage")
    for rID, count in routeUsage.items():
        usage.add(count, rID)
    print(usage)
    
    if options.unused_output is not None:
        with open(options.unused_output, 'w') as outf:
            for rID, count in routeUsage.items():
                usage.add(rID, count)
                if count <= options.threshold:
                    outf.write("%s\n" % rID)


if __name__ == "__main__":
    main()

