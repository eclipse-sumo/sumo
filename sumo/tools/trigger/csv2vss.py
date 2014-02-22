#!/usr/bin/env python
"""
@file    csv2vss.py
@author  Michael Behrisch
@date    2013-06-04
@version $Id$

Create variable speed signs from comma separated detector data.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys, collections
from optparse import OptionParser
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.output

def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <csv> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    optParser.add_option("-d", "--detectorfile", help="name of detector file")
    optParser.add_option("-s", "--scale", default=60, help="scaling factor for time")
    options, args = optParser.parse_args()
    try:
        options.csvfile = args[0]
    except:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.csvfile + ".add.xml"
    return options 

def main():
    options = parse_args()
    detectors = {}
    if options.detectorfile:
        for det in sumolib.output.parse(options.detectorfile, "detectorDefinition"):
            if det.type == "sink":
                detectors[det.id] = det.lane
    timeline = collections.defaultdict(list)
    with open(options.csvfile) as f:
        for line in f:
            data = line.split(";")
            try:
                if float(data[2]) > 0.:
                    timeline[data[0]].append((options.scale*float(data[1]), float(data[4])/3.6))
            except:
                pass
    # maybe we should sort the timeline here
    with open(options.outfile, 'w') as outf:
        outf.write("<additional>\n");
        for det, times in timeline.iteritems():
            if detectors:
                if det in detectors:
                    outf.write('    <variableSpeedSign id="vss_%s" lanes="%s">\n' % (det, detectors[det]))
                else:
                    continue
            else:
                outf.write('    <variableSpeedSign id="vss_%s" lanes="%s">\n' % (det, det))
            for entry in times:
                outf.write('        <step time="%.3f" speed="%.3f"/>\n' % entry)
            outf.write('    </variableSpeedSign>\n')
        outf.write("</additional>\n");
            
if __name__ == "__main__":
    main()
