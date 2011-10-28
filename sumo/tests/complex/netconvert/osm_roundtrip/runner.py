#!/usr/bin/env python
# -*- coding: utf-8 -*-

# import osm network 
# then import the built network again and check for idempotency


import sys,os,subprocess
import difflib
import StringIO
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "import", "osm"))
from sumolib import checkBinary
import fpdiff

osm_input = 'osm.xml'
net_output = 'from_osm.net.xml'
net_output2 = 'reloaded.net.xml'

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
        '--no-internal-links', 
        '--osm-files',  osm_input, '--proj.utm',
        '-R', '--ramps.guess', 
        '--tls.guess', '--tls.join',
        '--junctions.join',
        '--output', net_output]

args2 = [netconvert,
        '--sumo-net-file', net_output,
        '--no-internal-links', 
        '--offset.disable-normalization',
        '--output', net_output2]

subprocess.call(args1)
subprocess.call(args2)

fromlines = filter(open(net_output, 'U').readlines())
tolines = filter(open(net_output2, 'U').readlines())
out = StringIO.StringIO()
fpdiff.fpfilter(fromlines, tolines, out, 0.0201)
out.seek(0)
tolines = out.readlines()
sys.stderr.writelines(difflib.unified_diff(fromlines, tolines))
