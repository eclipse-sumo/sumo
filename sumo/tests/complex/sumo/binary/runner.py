#!/usr/bin/env python

import os,subprocess,sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
import sumolib

netconvertBinary = sumolib.checkBinary('netconvert')
duarouterBinary = sumolib.checkBinary('duarouter')
sumoBinary = sumolib.checkBinary('sumo')

print ">>> Building the xml network"
sys.stdout.flush()
subprocess.call([netconvertBinary, "-c", "netconvert.netccfg"], stdout=sys.stdout, stderr=sys.stderr)
print ">>> Building the binary network"
sys.stdout.flush()
subprocess.call([netconvertBinary, "-c", "netconvert.netccfg", "-o", "circular.net.sbx"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print ">>> Converting the routes to binary"
subprocess.call([duarouterBinary, "-c", "duarouter.duarcfg"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print ">>> Running Simulation with binary input"
subprocess.call([sumoBinary, "-c", "sumo.sumocfg"], stdout=sys.stdout, stderr=sys.stderr)
