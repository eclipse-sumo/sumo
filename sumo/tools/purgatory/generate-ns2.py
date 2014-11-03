#!/usr/bin/env python
"""
@file    generate-ns2.py
@author  Thimor Bohn
@author  Axel Wegener
@author  Michael Behrisch
@date    2006-08-07
@version $Id$

automating generation of ns2-tracefiles

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

exporter   = "H:\\itm\\sumo\\tools\\traceExporter\\traceExporter.jar"
sumo       = "H:\\itm\\sumo\\bin\\sumo.exe"
netconvert = "H:\\itm\\sumo\\bin\\netconvert.exe"
#
#END: change to your needs
#

import os
import sys
from   optparse import OptionParser

#
# parse parameters
#

usage = "usage: %prog [options]"
parser = OptionParser()
parser.add_option("--node",        "-N", action="store",  type="string", dest="nodefile",    help="name of nodesfile to be read")
parser.add_option("--edge",        "-E", action="store",  type="string", dest="edgefile",    help="name of edgesfile to be read")
parser.add_option("--route",       "-r", action="store",  type="string", dest="routefile",   help="name of routesfile to be read")
parser.add_option("--net",         "-n", action="store",  type="string", dest="netfile",     help="name of netfile to be read, you need either to specify this or node and edge")
parser.add_option("--begin",       "-b", action="store",  type="int", dest="begintime",   help="time at which simulation starts")
parser.add_option("--end",         "-e", action="store",  type="int", dest="endtime",     help="time at which simulation ends")
parser.add_option("--penetration", "-p", action="append", type="float",  dest="penetration", help="penetration factor of vehicles in [0,1]")
parser.add_option("--seed",        "-s", action="store",  type="int",    dest="seed", help="seed value for random generator")
(options, args) = parser.parse_args()

#
# check: correct parameter combination?
#
ok = True
print "checking parameters..."
if (options.routefile==None):
    print "you have to specify route"
    ok = False

if (options.netfile==None and (options.nodefile==None or options.edgefile==None)):
    print "you have to specify either net or node and edge"
    ok = False

if (options.begintime==None):
    print "you have to specify begin"
    ok = False

if (options.endtime==None):
    print "you have to specify end"
    ok = False
    
if (options.penetration==None):
    print "you have to specify penetration"
    ok = False

if (options.seed==None):
    print "no seed specified - defaulting to 0"
    options.seed = 0
    
if (ok == False):
    sys.exit(1)
    
print "done"

#
# check: all files exist / parameters correct?
#
print "files exist?"
if (options.nodefile!=None):
    if (os.path.isfile(options.nodefile)==False):
        print "file does not exist:", options.nodefile
        ok = False
    
if (options.edgefile!=None):
    if (os.path.isfile(options.edgefile)==False):
        print "file does not exist:", options.edgefile
        ok = False

if (options.routefile!=None):
    if (os.path.isfile(options.routefile)==False):
        print "file does not exist:", options.routefile
        ok = False

if (options.netfile!=None):
    if (os.path.isfile(options.netfile)==False):
        print "file does not exist:", options.netfile
        ok = False

if (os.path.isfile(netconvert)==False):
    print "file does not exist:", netconvert
    ok = False

if (os.path.isfile(sumo)==False):
    print "file does not exist:", sumo
    ok = False

if (os.path.isfile(exporter)==False):
    print "file does not exist:", exporter
    ok = False

for val in options.penetration:
    if (val < 0 or val > 1):
        print "penetration must be in [0,1]"
        ok = False

if (ok == False):
    sys.exit(1)
     
print "done"

#
# do the work
#

#
# create netfile if does not exist
#
if (options.netfile==None):
    os.system(netconvert + " -n=" + options.nodefile + " -e=" + options.edgefile + " --output-file=net.xml --disable-normalize-node-positions")
    if (os.path.isfile("net.xml")==False):
        print "error creating net.xml"
        sys.exit(1)
    os.system(netconvert + " -n=" + options.nodefile + " -e=" + options.edgefile + " --output-file=net-normalized.xml")
    if (os.path.isfile("net-normalized.xml")==False):
        print "error creating net-normalized.xml"
        sys.exit(1)

#
# create netstate
#
if (options.netfile==None):
    netfile="net.xml"
else:
    netfile=options.netfile
os.system(sumo + " -n " + netfile + " -r " + options.routefile + " --netstate-dump netstate.xml -b " + str(options.begintime) + " -e " + str(options.endtime))
if (os.path.isfile("netstate.xml")==False):
    print "error creating netstate.xml"
    sys.exit(1)

#
# create mobility, activity
#
for penetration in options.penetration:
    print "start: generation tracefile with penetration level of " + str(penetration)
    os.system("java -jar " + exporter + " ns2 -n " + netfile + " -t netstate.xml -m mobility_" +  str(penetration) + ".tcl -a activity_" +  str(penetration) + ".tcl -c config_" +  str(penetration) + ".tcl -p " + str(penetration) + " -s " + str(options.seed) + " -b " + str(options.begintime) + " -e " + str(options.endtime))
    if (os.path.isfile("mobility_" + str(penetration) + ".tcl")==False or os.path.isfile("activity_" + str(penetration) + ".tcl")==False or os.path.isfile("config_" + str(penetration)+".tcl")==False):
        print "error creating mobility, activity, config"
        sys.exit(1)
    else:
        print "finished: generation tracefile with penetration level of " + str(penetration)
    

#
# remove netstate
#
os.remove("netstate.xml")
sys.exit(0)
