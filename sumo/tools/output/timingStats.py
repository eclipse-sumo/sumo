#!/usr/bin/env python
"""
@file    timingStats.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2010-10-15
@version $Id$

This script executes a config repeatedly and measures the execution time,
 computes the mean values and deviation.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

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
import subprocess
import numpy
from optparse import OptionParser
from datetime import datetime

# initialise
optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true",
                     default=False, help="tell me what you are doing")
optParser.add_option("-c", "--configuration",
                     help="sumo configuration to run", metavar="FILE")
optParser.add_option("-r", "--repeat", type="int", default="20",
                     help="how many times to run")
# parse options
(options, args) = optParser.parse_args()

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', 'bin', 'sumo'))
elapsed = []
for run in range(options.repeat):
    before = datetime.now()
    subprocess.call([sumoBinary, '-c', options.configuration])
    td = datetime.now() - before
    elapsed.append(td.microseconds + 1000000 * td.seconds)
a = numpy.array(elapsed)
print("%.4f %.4f" % (a.mean() / 1000, a.std() / 1000))
