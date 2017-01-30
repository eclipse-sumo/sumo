#!/usr/bin/env python
"""
A script for generating a flow of pedestrians (a long list of person-walks)

@file    pedestrianFlow.py
@author  Jakob Erdmann
@date    2014-01-16
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
import os
import sys
import random
import datetime
from optparse import OptionParser

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
    from sumolib.miscutils import Colorgen
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    optParser = OptionParser()
    optParser.add_option(
        "-w", "--width", type="float", default=0.7, help="pedestrian width, negative numbers denote the center of a uniform distribution [x-0.2, x+0.2]")
    optParser.add_option(
        "-l", "--length", type="float", default=0.5, help="pedestrian length, negative numbers denote the center of a uniform distribution [x-0.2, x+0.2]")
    optParser.add_option(
        "-m", "--minGap", type="float", default=0.2, help="pedestrian min gap, negative numbers denote the center of a uniform distribution [x-0.2, x+0.2]")
    optParser.add_option(
        "-s", "--maxSpeed", type="float", default=1.2, help="pedestrian max speed, negative numbers denote the center of a uniform distribution [x-0.4, x+0.4]")
    optParser.add_option(
        "-d", "--departPos", type="float", default=0, help="depart position")
    optParser.add_option(
        "-a", "--arrivalPos", type="float", default=-1, help="arrival position")
    optParser.add_option(
        "-p", "--prob", type="float", default=0.1, help="depart probability per second")
    optParser.add_option("-r", "--route", help="edge list")
    optParser.add_option("-c", "--color", help="the color to use or 'random'")
    optParser.add_option(
        "-b", "--begin", type="int", default=0, help="begin time")
    optParser.add_option(
        "-e", "--end", type="int", default=600, help="end time")
    optParser.add_option("-i", "--index", type="int",
                         default=0, help="starting index for naming pedestrians")
    optParser.add_option(
        "-n", "--name", default="p", help="base name for pedestrians")
    (options, args) = optParser.parse_args()

    options.output = args[0]
    return options


def randomOrFixed(value, offset=0.2):
    if value >= 0:
        return value
    return random.uniform(-value - offset, -value + offset)


def write_ped(f, index, options, depart, edges):
    if options.color == None:
        color = ''
    elif options.color == "random":
        color = ' color="%s"' % Colorgen(("random", 1, 1))()
    else:
        color = ' color="%s"' % options.color

    f.write('    <vType id="%s%s" vClass="pedestrian" width="%s" length="%s" minGap="%s" maxSpeed="%s" guiShape="pedestrian"%s/>\n' % (
        options.name, index,
        randomOrFixed(options.width),
        randomOrFixed(options.length),
        randomOrFixed(options.minGap),
        randomOrFixed(options.maxSpeed, 0.4), color))
    f.write('    <person id="%s%s" type="%s%s" depart="%s">\n' %
            (options.name, index, options.name, index, depart))
    f.write('        <walk edges="%s" departPos="%s" arrivalPos="%s"/>\n' %
            (edges, options.departPos, options.arrivalPos))
    f.write('    </person>\n')


def main():
    options = get_options()
    with open(options.output, 'w') as f:
        sumolib.writeXMLHeader(
            fouttrips, "$Id$", "routes")
        index = options.index
        for depart in range(options.begin, options.end):
            if random.random() < options.prob:
                write_ped(
                    f, index, options, depart, ' '.join(options.route.split(',')))
                index += 1
        f.write('</routes>')

if __name__ == "__main__":
    main()
