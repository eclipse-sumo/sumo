#!/usr/bin/env python

import os,subprocess,sys,time,shutil
sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
netconvertBinary = os.environ.get("NETCONVERT_BINARY", os.path.join(sumoHome, 'bin', 'netconvert'))

subprocess.call([netconvertBinary, "-n", "input_nodes.nod.xml", "-e", "input_edges.edg.xml"], stdout=sys.stdout, stderr=sys.stderr)
subprocess.call([sumoBinary, "-c", "sumo.sumocfg", "-v"], stdout=sys.stdout, stderr=sys.stderr)
