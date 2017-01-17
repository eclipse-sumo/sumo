#!/usr/bin/env python
"""
@file    transportationTestProblems.py
@author  Michael Behrisch
@date    2015-02-24
@version $Id$

Create a network and trips for inputs found at http://www.bgu.ac.il/~bargera/tntp/.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess
import csv
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib

csvfile = csv.DictReader(open(sys.argv[1]), delimiter='\t')
with open("nodes.nod.xml", "w") as nodeout:
    nodeout.write("<nodes>\n")
    for l in csvfile:
        nodeout.write('    <node id="%s" x="%s" y="%s"/>\n' %
                      (l['node'], l['X'], l['Y']))
    nodeout.write("</nodes>\n")

csvfile = csv.reader(open(sys.argv[2]), delimiter='\t')
with open("edges.edg.xml", "w") as edgeout:
    edgeout.write("<edges>\n")
    for l in csvfile:
        if l and l[0][0] not in "~<":
            edgeout.write(
                '    <edge id="%s_%s" from="%s" to="%s"/>\n' % (2 * (l[0], l[1])))
    edgeout.write("</edges>\n")

subprocess.call(
    [sumolib.checkBinary("netconvert"), "-n", nodeout.name, "-e", edgeout.name])
