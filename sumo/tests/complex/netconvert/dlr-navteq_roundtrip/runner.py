#!/usr/bin/env python
# -*- coding: utf-8 -*-

# import sumo network 
# then exprot to dlr-navteq and import dlr-navteq again and check for idempotency with the original sumo network


import sys,os,subprocess
import difflib
import StringIO
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "import", "osm"))
from sumolib import checkBinary
import fpdiff

net_input = 'input_net.net.xml'
dlr_navteq_prefix = 'elmar'
net_output = 'output_net.net.xml' # do not collate

netconvert = checkBinary('netconvert')
assert(netconvert)

# filter header and projection clause
def filter(lines):
    skippedHeader = False
    result = []
    for l in lines:
        if '<net>' in l:
            skippedHeader = True
        if not skippedHeader:
            continue
        if '<location' in l:
            continue
        if '<projection' in l:
            continue
        result.append(l)
    return result


args1 = [netconvert,
        '--sumo-net-file', net_input,
        '--dlr-navteq-output', dlr_navteq_prefix]

args2 = [netconvert,
        '--dlr-navteq-prefix', dlr_navteq_prefix,
        '--output', net_output]

subprocess.call(args1)
subprocess.call(args2)

fromlines = filter(open(net_input, 'U').readlines())
tolines = filter(open(net_output, 'U').readlines())
out = StringIO.StringIO()
fpdiff.fpfilter(fromlines, tolines, out, 0.0201)
out.seek(0)
tolines = out.readlines()
sys.stderr.writelines(difflib.unified_diff(fromlines, tolines))
