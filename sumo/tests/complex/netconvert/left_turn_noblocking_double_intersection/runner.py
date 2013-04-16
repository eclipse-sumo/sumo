#!/usr/bin/env python
# -*- coding: utf-8 -*-

# call jtrrouter twice and check that the output differs
import sys,os,subprocess,random
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
from sumolib import checkBinary

net_output = 'joined.net.xml'
trips_output = 'trips.log'

netconvert = checkBinary('netconvert')
assert(netconvert)
sumo = checkBinary('sumo')
assert(sumo)

args_netc = [netconvert,
        '--node-files', 'input_nodes.nod.xml',
        '--edge-files', 'input_edges.edg.xml',
        '--output', net_output,
        '--offset.disable-normalization']

args_sumo = [sumo,
        '--net-file', net_output,
        '--route-files', 'input_routes.rou.xml',
        '--end', '50',
        '--no-step-log',
        '--no-duration-log',
        '--tripinfo-output', trips_output]

subprocess.call(args_netc)
subprocess.call(args_sumo)

# vehicles should have completed their trips
complete = False
for line in open(trips_output):
    if 'veh0' in line:
        complete = True

if complete:
    print('test passed. no blocking occured')
else:
    print('test failed. vehicles were blocked')
