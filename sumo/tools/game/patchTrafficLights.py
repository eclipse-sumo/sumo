#!/usr/bin/env python
"""
@file    patchTrafficLights.py
@author  Michael.Behrisch@dlr.de
@date    2010-03-11
@version $Id: runner.py 8386 2010-03-05 22:29:33Z behrisch $

This script patches the traffic lights of an input network for the gaming mode.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""
import re
from optparse import OptionParser

optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                        help="define the input net file (mandatory)")
optParser.add_option("-o", "--output-net-file", dest="outfile",
                     default="out.net.xml", help="define the output net filename")
optParser.add_option("-s", "--switch", type="int", default=2, help="switch time")
(options, args) = optParser.parse_args()
if not options.netfile:
    optParser.print_help()
    sys.exit()

skip = False
out = open(options.outfile, "w")
for line in open(options.netfile):
    m = re.search('<tl-logic id="([^"])"', line)
    if m:
        skip = True
        print >> out, line + """
      <phase duration="%s" state="rrrrrrrrrrrrrrrr"/>
      <phase duration="10000" state="rrrrGGggrrrrGGgg"/>
   </tl-logic>

   <tl-logic id="%s" type="static" programID="1" offset="0">
      <phase duration="%s" state="rrrrrrrrrrrrrrrr"/>
      <phase duration="10000" state="GGggrrrrGGggrrrr"/>""" % (options.switch, m.group(1), options.switch)
    if re.search('</tl-logic>', line):
        skip = False
    if not skip:
        out.write(line)
