#!/bin/python
"""
apply runSeeds to a list of tests and compute statistics
use case: tweaking the lane change model and then seeing the result of a
gazilliion tests with reduced noise
"""

import os,sys
import glob
from subprocess import call, check_output

tests, prefix, apps = sys.argv[1:]
SEEDS = "0:100"
THREADS = "16"

EXTRACT = os.path.join(os.environ['SUMO_HOME'], 'tools', 'extractTest.py')
RUNSEEDS = os.path.join(os.environ['SUMO_HOME'], 'tools', 'runSeeds.py')
ASTATS = os.path.join(os.environ['SUMO_HOME'], 'tools', 'output', 'attributeStats.py')

appdirs = [a + "_0" for a in apps.split(',')]

for test in open(tests).readlines()[1:]:
    test = test.strip()
    fullpath = os.path.join(prefix, test)
    call([EXTRACT, fullpath])
    tdir = '_' + test.replace('/', '_')
    os.chdir(tdir)
    call([RUNSEEDS,
        '-k', 'test.sumocfg',
        '-a', apps,
        '--seeds', SEEDS,
        '--threads', THREADS,
        '--no-warnings',
        '--statistic-output', 'stats.xml'])

    outf = open('compare.txt', 'a')
    for ad in appdirs:
        files = glob.glob(os.path.join(ad, '*.stats.xml'))
        args = [ASTATS,
            '-e', 'vehicleTripStatistics',
            '-a', 'timeLoss'] + files
        call(args, stdout=outf)
    outf.close()
    os.chdir('..')



