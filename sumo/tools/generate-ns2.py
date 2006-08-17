#
# automating generation of ns2-tracefiles
# author: thimor bohn <bohn@itm.uni-luebeck.de>
# date: 2006/08/07
#


#
# BEGIN: change to your needs
#
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
parser.add_option("--node",        action="store", type="string", dest="nodefile",    help="name of nodesfile to be read")
parser.add_option("--edge",        action="store", type="string", dest="edgefile",    help="name of edgesfile to be read")
parser.add_option("--route",       action="store", type="string", dest="routefile",   help="name of routesfile to be read")
parser.add_option("--net",         action="store", type="string", dest="netfile",     help="name of netfile to be read, you need either to specify this or node and edge")
parser.add_option("--begin",       action="store", type="string", dest="begintime",   help="time at which simulation starts")
parser.add_option("--end",         action="store", type="string", dest="endtime",     help="time at which simulation ends")
parser.add_option("--penetration", action="store", type="string", dest="penetration", help="penetration factor of vehicles in [0,1]")
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

if (float(options.penetration) < 0 or float(options.penetration) > 1):
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
os.system(sumo + " -n " + netfile + " -r " + options.routefile + " --netstate-dump netstate.xml -b " + options.begintime + " -e " + options.endtime)
if (os.path.isfile("netstate.xml")==False):
    print "error creating netstate.xml"
    sys.exit(1)

#
# create mobility, activity
#
os.system("java -jar " + exporter + " ns2 -n " + netfile + " -t netstate.xml -m mobility -a activity -c config.tcl -p " + options.penetration)
if (os.path.isfile("mobility")==False or os.path.isfile("activity")==False):
    print "error creating mobility, activity"
    sys.exit(1)
    

#
# remove netstate
#
os.remove("netstate.xml")
sys.exit(0)
