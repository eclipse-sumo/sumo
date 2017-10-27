#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    generate-ns2.py
# @author  Thimor Bohn
# @author  Axel Wegener
# @author  Michael Behrisch
# @date    2006-08-07
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

exporter = "H:\\itm\\sumo\\tools\\traceExporter\\traceExporter.jar"
sumo = "H:\\itm\\sumo\\bin\\sumo.exe"
netconvert = "H:\\itm\\sumo\\bin\\netconvert.exe"
#
# END: change to your needs
#

import os
import sys
from optparse import OptionParser

#
# parse parameters
#

usage = "usage: %prog [options]"
parser = OptionParser()
parser.add_option("--node",        "-N", action="store",  type="string",
                  dest="nodefile",    help="name of nodesfile to be read")
parser.add_option("--edge",        "-E", action="store",  type="string",
                  dest="edgefile",    help="name of edgesfile to be read")
parser.add_option("--route",       "-r", action="store",  type="string",
                  dest="routefile",   help="name of routesfile to be read")
parser.add_option("--net",         "-n", action="store",  type="string", dest="netfile",
                  help="name of netfile to be read, you need either to specify this or node and edge")
parser.add_option("--begin",       "-b", action="store",  type="int",
                  dest="begintime",   help="time at which simulation starts")
parser.add_option("--end",         "-e", action="store",  type="int",
                  dest="endtime",     help="time at which simulation ends")
parser.add_option("--penetration", "-p", action="append", type="float",
                  dest="penetration", help="penetration factor of vehicles in [0,1]")
parser.add_option("--seed",        "-s", action="store",
                  type="int",    dest="seed", help="seed value for random generator")
(options, args) = parser.parse_args()

#
# check: correct parameter combination?
#
ok = True
print("checking parameters...")
if (options.routefile is None):
    print("you have to specify route")
    ok = False

if (options.netfile is None and (options.nodefile is None or options.edgefile is None)):
    print("you have to specify either net or node and edge")
    ok = False

if (options.begintime is None):
    print("you have to specify begin")
    ok = False

if (options.endtime is None):
    print("you have to specify end")
    ok = False

if (options.penetration is None):
    print("you have to specify penetration")
    ok = False

if (options.seed is None):
    print("no seed specified - defaulting to 0")
    options.seed = 0

if (not ok):
    sys.exit(1)

print("done")

#
# check: all files exist / parameters correct?
#
print("files exist?")
if (options.nodefile is not None):
    if (os.path.isfile(options.nodefile) is False):
        print("file does not exist:", options.nodefile)
        ok = False

if (options.edgefile is not None):
    if (os.path.isfile(options.edgefile) is False):
        print("file does not exist:", options.edgefile)
        ok = False

if (options.routefile is not None):
    if (os.path.isfile(options.routefile) is False):
        print("file does not exist:", options.routefile)
        ok = False

if (options.netfile is not None):
    if (os.path.isfile(options.netfile) is False):
        print("file does not exist:", options.netfile)
        ok = False

if (os.path.isfile(netconvert) is False):
    print("file does not exist:", netconvert)
    ok = False

if (os.path.isfile(sumo) is False):
    print("file does not exist:", sumo)
    ok = False

if (os.path.isfile(exporter) is False):
    print("file does not exist:", exporter)
    ok = False

for val in options.penetration:
    if (val < 0 or val > 1):
        print("penetration must be in [0,1]")
        ok = False

if (not ok):
    sys.exit(1)

print("done")

#
# do the work
#

#
# create netfile if does not exist
#
if (options.netfile is None):
    os.system(netconvert + " -n=" + options.nodefile + " -e=" +
              options.edgefile + " --output-file=net.xml --disable-normalize-node-positions")
    if (os.path.isfile("net.xml") is False):
        print("error creating net.xml")
        sys.exit(1)
    os.system(netconvert + " -n=" + options.nodefile + " -e=" +
              options.edgefile + " --output-file=net-normalized.xml")
    if (os.path.isfile("net-normalized.xml") is False):
        print("error creating net-normalized.xml")
        sys.exit(1)

#
# create netstate
#
if (options.netfile is None):
    netfile = "net.xml"
else:
    netfile = options.netfile
os.system(sumo + " -n " + netfile + " -r " + options.routefile +
          " --netstate-dump netstate.xml -b " + str(options.begintime) + " -e " + str(options.endtime))
if (os.path.isfile("netstate.xml") is False):
    print("error creating netstate.xml")
    sys.exit(1)

#
# create mobility, activity
#
for penetration in options.penetration:
    print(
        "start: generation tracefile with penetration level of " + str(penetration))
    os.system("java -jar " + exporter + " ns2 -n " + netfile + " -t netstate.xml -m mobility_" + str(penetration) + ".tcl -a activity_" + str(penetration) +
              ".tcl -c config_" + str(penetration) + ".tcl -p " + str(penetration) + " -s " + str(options.seed) + " -b " + str(options.begintime) + " -e " + str(options.endtime))
    if (os.path.isfile("mobility_" + str(penetration) + ".tcl") is False or os.path.isfile("activity_" + str(penetration) + ".tcl") is False or os.path.isfile("config_" + str(penetration) + ".tcl") is False):
        print("error creating mobility, activity, config")
        sys.exit(1)
    else:
        print(
            "finished: generation tracefile with penetration level of " + str(penetration))


#
# remove netstate
#
os.remove("netstate.xml")
sys.exit(0)
