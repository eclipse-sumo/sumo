#!/usr/bin/env python

import os, subprocess, sys
sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))

def call(command):
    retCode = subprocess.call(command, stdout=sys.stdout, stderr=sys.stderr)
    if retCode != 0:
        print >> sys.stderr, "Execution of %s failed." % command
        sys.exit(retCode) 

PERIOD = 5
DEPARTSPEED = "max"

fdo = open("results.csv", "w")
for departPos in "random free random_free base pwagSimple pwagGeneric maxSpeedGap".split():
    print ">>> Building the routes (for departPos %s)" % departPos
    fd = open("input_routes.rou.xml", "w")
    print >> fd, """<routes>
    <flow id="vright" route="right" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
    <flow id="vleft" route="left" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
    <flow id="vhorizontal" route="horizontal" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
</routes>""" % (3*(departPos, DEPARTSPEED, PERIOD))
    fd.close()

    print ">>> Simulating ((for departPos %s)" % departPos
    call([sumoBinary, "-c", "sumo.sumocfg", "-v"])

    dump = sumolib.output.dump.readDump("aggregated.xml", ["entered"])
    print >> fdo, "%s;%s" % (departPos, dump.get("entered")[-1]["1si"])
    
    if os.path.exists(departPos + "_aggregated.xml"):
        os.remove(departPos + "_aggregated.xml")
    os.rename("aggregated.xml", departPos + "_aggregated.xml")
fdo.close()
