#!/usr/bin/env python
"""
@file    runInternalTests.py
@author  Michael Behrisch
@date    2012-03-29
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import optparse, os, subprocess, sys

def runInternal(suffix, args, out=sys.stdout):
    if os.name != "posix":
        suffix += ".exe"
    env = os.environ
    root = os.path.abspath(os.path.dirname(__file__))
    env["TEXTTEST_HOME"] = root
    env["ACTIVITYGEN_BINARY"] = os.path.join(root, "..", "bin", "activitygenInt" + suffix)
    env["DFROUTER_BINARY"] = os.path.join(root, "..", "bin", "dfrouterInt" + suffix)
    env["DUAROUTER_BINARY"] = os.path.join(root, "..", "bin", "duarouterInt" + suffix)
    env["JTRROUTER_BINARY"] = os.path.join(root, "..", "bin", "jtrrouterInt" + suffix)
    env["NETCONVERT_BINARY"] = os.path.join(root, "..", "bin", "netconvertInt" + suffix)
    env["NETGENERATE_BINARY"] = os.path.join(root, "..", "bin", "netgenerateInt" + suffix)
    env["OD2TRIPS_BINARY"] = os.path.join(root, "..", "bin", "od2tripsInt" + suffix)
    env["SUMO_BINARY"] = os.path.join(root, "..", "bin", "meso" + suffix)
    env["POLYCONVERT_BINARY"] = os.path.join(root, "..", "bin", "polyconvertInt" + suffix)
    env["GUISIM_BINARY"] = os.path.join(root, "..", "bin", "meso-gui" + suffix)
    env["MAROUTER_BINARY"] = os.path.join(root, "..", "bin", "marouter" + suffix)
    ttBin = "/usr/bin/texttest"
    if os.name != "posix" or not os.path.exists(ttBin):
        ttBin = "texttest.py"
    subprocess.call("%s %s -a sumo.internal,sumo.meso,complex.meso,duarouter.astar,duarouter.chrouter,netconvert.internal,marouter" % (ttBin, args),
                    stdout=out, stderr=out, shell=True)

if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.add_option("-s", "--suffix", default="", help="suffix to the fileprefix")
    (options, args) = optParser.parse_args()
    runInternal(options.suffix, " ".join(["-" + a for a in args]))
