#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    analyzePersonPlans.py
# @author  Jakob Erdmann
# @date    2019-09-13
# @version $Id$

"""
Count the number if different person plans in a route file
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from argparse import ArgumentParser
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse
from sumolib.miscutils import Statistics

def get_options(args=None):
    parser = ArgumentParser(description="Analyze person plans")
    parser.add_argument("-r", "--route-files", dest="routeFiles", help="Input route files")
    parser.add_argument("-w", "--merge-walks", dest="mergeWalks", action="store_true", help="merge subsequent walks")
    parser.add_argument("-p", "--public-prefixes", dest="public", help="Distinguis public transport modes by prefix")
    options = parser.parse_args(args=args)
    if options.routeFiles is None:
        parser.print_help()
        sys.exit()
    if options.public:
        options.public = options.public.split(',')
    else:
        options.public = []
    return options

def stageName(options, person, stage):
    if stage.name == 'ride':
        if stage.lines.startswith(person.id):
            return 'car'
        else:
            if stage.intended is not None:
                for pType in options.public:
                    if pType in stage.intended:
                        return pType
            return 'public'
    else:
        return stage.name


def main(options):
    counts = defaultdict(lambda : 0)
    for routeFile in options.routeFiles.split(','):
        for person in parse(routeFile, 'person'):
            stages = tuple([stageName(options, person, s) for s in person.getChildList()])
            if options.mergeWalks:
                filtered = []
                for s in stages:
                    if len(filtered) == 0 or s != filtered[-1]:
                        filtered.append(s)
                stages = tuple(filtered)
            counts[stages] += 1

    reverseCounts = [(count, k) for k,count in counts.items()]
    for count, k in sorted(reverseCounts):
        print("%s: %s" % (count, ' '.join(k)))

if __name__ == "__main__":
    main(get_options())
