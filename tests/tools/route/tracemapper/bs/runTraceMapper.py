#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runTraceMapper.py
@author  Yun-Pang Floetteroed
@date    2017-11-29
@version $Id: runTraceMapper.py  $

Run traceMapper.py to generate a route file which
corresponds to the given trajectories

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2015 DLR (http://www.dlr.de/) and contributors

"""
import os, sys, subprocess,glob
from optparse import OptionParser

SUMO_HOME = os.environ['SUMO_HOME']
ROOT_DIR = os.path.join(os.path.dirname(__file__), "..")
DEBUG_DIR = os.path.join(os.path.dirname(__file__))
OUT_DIR = os.path.join(os.path.dirname(__file__),"new")
traceMapperPy = os.path.join(SUMO_HOME, "tools", "route", "tracemapper.py")
route2PolyPy = os.path.join(SUMO_HOME, "tools", "route", "route2poly.py")

tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
sys.path.append(tools)

import sumolib
from sumolib.output import parse

# parameters


code2file = [os.path.join(DEBUG_DIR, "20140707_t0.txt"),
            # os.path.join(DEBUG_DIR, "20140707_t1.txt"),
            # os.path.join(DEBUG_DIR, "20140707_t2.txt"),
            # os.path.join(DEBUG_DIR, "20140707_t3.txt"),
            # os.path.join(DEBUG_DIR, "20140707_t4.txt"),
            # os.path.join(DEBUG_DIR, "20140707_t5.txt"),
            ]

if __name__ == "__main__":

    optParser = OptionParser()
    optParser.add_option("-d", "--delta", default=30,
                         type="int", help="maximum distance between edge and trace points")
    optParser.add_option("--standalone", action="store_true",
                     default=False, help="Parse stand-alone routes that are not define as child-element of a vehicle")

    (options, args) = optParser.parse_args()

    dateList = ["t1","t2", "t3", "t4", "t5"]
    routefiles= []
    netfile = os.path.join(DEBUG_DIR, "bs_osm_route.net.xml")

    tracefiles = ','.join(code2file)
    outRoutes = os.path.join(DEBUG_DIR, "routes_%s.xml" % (options.delta))
    
    cmd = [
        'python',
        traceMapperPy,
        '--net', netfile,
        '--trace', tracefiles,
        '--output', outRoutes,
        '--delta', str(options.delta),
        '--geo', 
        '--fill-gaps', "true",
        '--poi-output', "test.poi.xml",
        '--polygon-output', "test.poly.xml",
        #'--gap-penalty', "5",
        '--verbose',
        ]
    print("calling %s" % ' '.join(cmd))
    subprocess.call(cmd, stdout=sys.stdout, stderr=sys.stderr)


