#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    patchTrafficLights.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2010-03-11
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import re
import sys
from optparse import OptionParser

optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="define the input net file (mandatory)")
optParser.add_option("-o", "--output-net-file", dest="outfile",
                     default="out.net.xml", help="define the output net filename")
optParser.add_option(
    "-s", "--switch", type="int", default=2, help="switch time")
(options, args) = optParser.parse_args()
if not options.netfile:
    optParser.print_help()
    sys.exit()

skip = False
tlid = ""
out = open(options.outfile, "w")
for line in open(options.netfile):
    if tlid and not skip:
        m = re.search('state="([^"]+)"', line)
        if m and len(m.group(1)) == 16:
            skip = True
            print("""      <phase duration="%s" state="rrrrrrrrrrrrrrrr"/>
      <phase duration="10000" state="rrrrGGggrrrrGGgg"/>
   </tlLogic>

   <tlLogic id="%s" type="static" programID="1" offset="0">
      <phase duration="%s" state="rrrrrrrrrrrrrrrr"/>
      <phase duration="10000" state="GGggrrrrGGggrrrr"/>""" % (options.switch, tlid, options.switch), file=out)
    m = re.search('<tlLogic id="([^"]+)"', line)
    if m:
        tlid = m.group(1)
    if re.search('</tlLogic>', line):
        skip = False
        tlid = ""
    if not skip:
        out.write(line)
