#!/usr/bin/env python
"""
@file    mpl_dump_timeline.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007-10-25
@version $Id: mpl_dump_timeline.py 625 2008-03-08 14:04:01Z behr_mi $


This script reads the value of designated edges from
 a set of given dump-files and displays them.

matplotlib has to be installed for this purpose

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

from matplotlib import rcParams
rcParams['text.fontname'] = 'cmr10'
from pylab import *
import os, string, sys, StringIO
import math
from optparse import OptionParser
from xml.sax import saxutils, make_parser, handler



def updateMinMax(min, max, value):
    if min==None or min>value:
        min = value
    if max==None or max<value:
        max = value
    return (min, max)


class WeightsReader(handler.ContentHandler):
    """Reads the dump file"""

    def __init__(self, value, edges):
        self._edge2value = {}
        self._value = value
        self._edges = edges
        self._intervals = []

    def startElement(self, name, attrs):
        if name == 'interval':
            self._time = int(attrs['begin'])
            self._edge2value[self._time] = {}
            self._intervals.append(self._time)
        if name == 'edge':
            id = attrs['id']
            if id in self._edges:
                self._edge2value[self._time][id] = float(attrs[self._value])

# initialise 
optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
    # i/o
optParser.add_option("-d", "--dumps", dest="dumps",
                     help="The dump files to parse, divided by a ','", metavar="FILE")
optParser.add_option("-e", "--edges", dest="edges",
                     help="The edges to extract, divided by a ','", metavar="FILE")
optParser.add_option("-c", "--colors", dest="colors",
                     help="The colors to use, divided by a ',' (over dumps, then edges)", metavar="FILE")
optParser.add_option("-o", "--output", dest="output",
                     help="Name of the image to generate", metavar="FILE")
optParser.add_option("--size", dest="size",type="string", default="",
                     help="defines the output size")
    # processing
optParser.add_option("--value", dest="value", 
                     type="string", default="speed", help="which value shall be used")
optParser.add_option("-s", "--show", action="store_true", dest="show",
                     default=False, help="shows plot after generating it")
    # axes/legend
optParser.add_option("--xticks", dest="xticks",type="string", default="",
                     help="defines ticks on x-axis")
optParser.add_option("--yticks", dest="yticks",type="string",  default="",
                     help="defines ticks on y-axis")
optParser.add_option("--xlim", dest="xlim",type="string",  default="",
                     help="defines x-axis range")
optParser.add_option("--ylim", dest="ylim",type="string",  default="",
                     help="defines y-axis range")
# parse options
(options, args) = optParser.parse_args()


parser = make_parser()
# read dump1
weights = []
for dump in options.dumps.split(','):
    if options.verbose:
        print "Reading %s..." % dump
    cweights = WeightsReader(options.value, options.edges.split(','))
    weights.append(cweights)
    parser.setContentHandler(cweights)
    parser.parse(dump)
# plot
if options.verbose:
    print "Processing data..."
# set figure size
if not options.show:
    rcParams['backend'] = 'Agg'
if options.size:
    f = figure(figsize=(options.size.split(",")))
else:
    f = figure()
    # compute values and color(s)
xss = []
yss = []
cs = []
colors = options.colors.split(',')
index = 0
xmin = None
xmax = None
ymin = None
ymax = None
for dump in weights:
    for edge in options.edges.split(','):
        xs = []
        ys = []
        for t in dump._intervals:
            xs.append(t)
            ys.append(dump._edge2value[t][edge])
            (xmin, xmax) = updateMinMax(xmin, xmax, t)
            (ymin, ymax) = updateMinMax(ymin, ymax, dump._edge2value[t][edge])
        xss.append(xs)
        yss.append(ys)
        cs.append(colors[index])
        index = index + 1
     # plot
print "data x-range: " + str(xmin) + " - " + str(xmax)
print "data y-range: " + str(ymin) + " - " + str(ymax)
for i in range(0, len(cs)):
    plot(xss[i], yss[i], color=cs[i], )
# set axes
if options.xticks!="":
    (xb, xe, xd, xs) = options.xticks.split(",")
    xticks(arange(xb, xe, xd), size = xs)
if options.yticks!="":
    (yb, ye, yd, ys) = options.yticks.split(",")
    yticks(arange(yb, ye, yd), size = ys)
if options.xlim!="":
    (xb, xe) = options.xlim.split(",")
    xlim(xb, xe)
else:
    xlim(xmin, xmax)
if options.ylim!="":
    (yb, ye) = options.ylim.split(",")
    ylim(yb, ye)
else:
    ylim(ymin, ymax)
# show/save
if options.show:
    show()
if options.output:
    savefig(options.output);


