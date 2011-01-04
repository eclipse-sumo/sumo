#!/usr/bin/env python
"""
@file    timingStats.py
@author  Michael.Behrisch@dlr.de
@date    2010-10-15
@version $Id$

This script executes a config repeatedly and measures the execution time,
 computes the mean values and deviation.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, sys, subprocess, numpy
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

sumoBinary = os.environ.get("NETCONVERT_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', 'bin', 'sumo'))
elapsed = []
for run in range(options.repeat):
    before = datetime.now()
    subprocess.call([sumoBinary, '-c', options.configuration])
    td = datetime.now() - before
    elapsed.append(td.microseconds + 1000000 * td.seconds)
a = numpy.array(elapsed)
print "%.4f %.4f" % (a.mean() / 1000, a.std() / 1000)
