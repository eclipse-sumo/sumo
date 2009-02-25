#!/usr/bin/python
"""
@file    rebuild.py
@author  Daniel.Krajzewicz@dlr.de
@date    28-07-2008
@version $Id$

Goes through all files in examples; executes found configurations 
 using the proper application.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, os.path, sys
from optparse import OptionParser


def runConfiguration(app, file, appRoot, verbose, rebuild):
    (path, file) = os.path.split(file)
    oldDir = os.getcwd()
    os.chdir(path)
    print "----------------------------------"
    vS = ""
    if verbose:
        vS = " -v"
    rS = ""
    if rebuild:
        rS = " --save-configuration " + file + " --save-template.commented"
    cmdOptions = "-c " + file + vS + rS
    print "Runnning: " + file
    if(sys.platform=="win32"):
        (cin, cout) = os.popen4(appRoot + "/" + app + " " + cmdOptions)
    else:
        if app!="sumo":
            app = "sumo-" + app
        (cin, cout) = os.popen4(appRoot + "/" + app + " " + cmdOptions)
    for line in cout:
        print line[:-1]
    print "----------------------------------\n"
    os.chdir(oldDir)


def scanAndRunConfigs(exRoot, cfgDefs, appRoot, verbose, rebuild):
    for root, dirs, files in os.walk(exRoot):
        for file in files:
            for d in cfgDefs:
                if file.endswith(d):
                    runConfiguration(cfgDefs[d], os.path.join(root, file), appRoot, verbose, rebuild)


optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("--no-nets", action="store_true", dest="no_nets",
                     default=False, help="Omits network building configs (.netc.cfg, .netg.cfg)")
optParser.add_option("--no-routes", action="store_true", dest="no_routes",
                     default=False, help="Omits router configs (.df.cfg, .dua.cfg, .jtr.cfg)")
optParser.add_option("--no-sims", action="store_true", dest="no_sims",
                     default=False, help="Omits simulation configs (.sumo.cfg)")
optParser.add_option("--rebuild", action="store_true", dest="rebuild",
                     default=False, help="Rebuilds the configuration file if set")
(options, args) = optParser.parse_args()


if(sys.platform=="win32"):
    appRoot = "../../bin/"
else:
    appRoot = "../../src/"
appRoot = os.path.abspath(appRoot)
exRoot = os.path.join(os.path.dirname(sys.argv[0]), "../../data/examples/")
if not options.no_nets:
    cfgDefs = {}
    cfgDefs[".netc.cfg"] = "netconvert"
    cfgDefs[".netg.cfg"] = "netgen"
    scanAndRunConfigs(exRoot, cfgDefs, appRoot, options.verbose, options.rebuild)
if not options.no_routes:
    cfgDefs = {}
    cfgDefs[".df.cfg"] = "dfrouter"
    cfgDefs[".dua.cfg"] = "duarouter"
    cfgDefs[".jtr.cfg"] = "jtrrouter"
    scanAndRunConfigs(exRoot, cfgDefs, appRoot, options.verbose, options.rebuild)
if not options.no_sims:
    cfgDefs = {}
    cfgDefs[".sumo.cfg"] = "sumo"
    scanAndRunConfigs(exRoot, cfgDefs, appRoot, options.verbose, options.rebuild)


