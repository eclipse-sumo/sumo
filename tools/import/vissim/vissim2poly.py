#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    vissim2poly.py
# @author  Jakob Erdmann
# @date    2020-01-30

"""
Parses a vissim .inpx file and exports link geometrys as <poly> elements
Usage: vissim2poly.py inpxFile outputFile
"""
import os,sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))

import sumolib
vissimfile, outfile = sys.argv[1:]
with open(outfile, 'w') as outf:
    outf.write('<add>\n')
    for link in sumolib.xml.parse(vissimfile, 'link'):
        if link.geometry is None or link.geometry[0].linkPolyPts is None:
            print("no polypoints for link %s" % link.no)
        points = [(p.x, p.y, p.z) for p in link.geometry[0].linkPolyPts[0].linkPolyPoint]
        shape = ' '.join([','.join(filter(None, xyz)) for xyz in points])
        outf.write('    <poly id="%s" shape="%s" color="blue"/>\n' % (
            link.no, shape))
    outf.write('</add>\n')
