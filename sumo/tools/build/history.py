#!/usr/bin/env python
"""
@file    history.py
@author  Michael Behrisch
@date    2014-06-21
@version $Id$

This script builds all sumo versions in a certain revision range
and tries to eliminate duplicates afterwards.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import subprocess, optparse, shutil, os

optParser = optparse.OptionParser()
optParser.add_option("-b", "--begin", type="int",
                     default=16000, help="start revision")
optParser.add_option("-e", "--end", type="int",
                     default=100000, help="end revision")
optParser.add_option("-s", "--step", type="int",
                     default=1, help="increment")
options, args = optParser.parse_args()

for rev in range(options.begin, options.end, options.step):
    if not os.path.exists('bin%s' % rev):
        ret = subprocess.call('svn up --ignore-externals -r %s sumo' % rev, shell=True)
        if ret != 0:
            break
        subprocess.call('cd sumo; make clean; make -j 16; cd ..', shell=True)
        subprocess.call('strip -R .note.gnu.build-id sumo/bin/*', shell=True)
        subprocess.call("sed -i 's/dev-SVN-r%s/dev-SVN-r00000/' sumo/bin/*" % rev, shell=True)
        shutil.copytree('sumo/bin', 'bin%s' % rev)
for line in subprocess.check_output('fdupes -1 -q bin*', shell=True).splitlines():
    dups = line.split()
    for d in dups[1:]:
        subprocess.call('ln -sf ../%s %s' % (dups[0], d), shell=True)
