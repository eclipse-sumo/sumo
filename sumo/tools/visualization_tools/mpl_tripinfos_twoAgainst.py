#!/usr/bin/env python
"""
@file    mpl_tripinfos_twoAgainst.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007-10-25
@version $Id: mpl_tripinfos_twoAgainst.py 625 2008-03-08 14:04:01Z behr_mi $


This script reads two tripinfos files and plots one of the values
 stored therein as an x-/y- plot.

matplotlib has to be installed for this purpose

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

from matplotlib import rcParams
from pylab import *
import os, string, sys, StringIO
import math
from optparse import OptionParser
from xml.sax import saxutils, make_parser, handler



def toHex(val):
    """Converts the given value (0-255) into its hexadecimal representation"""
    hex = "0123456789abcdef"
    return hex[int(val/16)] + hex[int(val - int(val/16)*16)]

def toColor(val):
    """Converts the given value (0-1) into a color definition as parseable by matplotlib"""
    g = 255. * val
    return "#" + toHex(g) + toHex(g) + toHex(g)



def updateMinMax(min, max, value):
    if min==None or min>value:
        min = value
    if max==None or max<value:
        max = value
    return (min, max)


class VehroutesReader(handler.ContentHandler):
    """Reads the vehroutes file"""

    def __init__(self, value):
        self._id = ''
        self._veh2value = {}
        self._veh2time = {}
        self._value = value

    def startElement(self, name, attrs):
        if name == 'tripinfo':
            id = attrs['vehicle_id']
            self._veh2value[id] = float(attrs[self._value])
            self._veh2time[id] = float(attrs["wished"])



# initialise 
optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
    # i/o
optParser.add_option("-1", "--tripinfos1", dest="tripinfos1",
                     help="First tripinfos (mandatory)", metavar="FILE")
optParser.add_option("-2", "--tripinfos2", dest="tripinfos2",
                     help="Second tripinfos (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output", dest="output",
                     help="Name of the image to generate", metavar="FILE")
optParser.add_option("--size", dest="size",type="string", default="",
                     help="defines the output size")
    # processing
optParser.add_option("--value", dest="value", 
                     type="string", default="duration", help="which value shall be used")
optParser.add_option("-s", "--show", action="store_true", dest="show",
                     default=False, help="shows plot after generating it")
optParser.add_option("-C", "--time-coloring", action="store_true", dest="time_coloring",
                     default=False, help="colors the points by the time")
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
if options.verbose:
    print "Reading tripinfos1..."
r1 = VehroutesReader(options.value)
parser.setContentHandler(r1)
parser.parse(options.tripinfos1)
# read dump2
if options.verbose:
    print "Reading tripinfos2..."
r2 = VehroutesReader(options.value)
parser.setContentHandler(r2)
parser.parse(options.tripinfos2)
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
xs = []
ys = []
    # compute values and color(s)
c = 'k'
min = None
max = None
#if options.time_coloring:
c = []
for veh in r1._veh2value:
    if veh in r2._veh2value:
        if options.time_coloring:
            cc = 1. - ((float(r1._veh2time[veh]) / 86400.) * .8 + .2)
            c.append(toColor(cc))
        else:
            c.append('k')
        xs.append(r1._veh2value[veh])
        ys.append(r2._veh2value[veh])
        (min, max) = updateMinMax(min, max, r1._veh2value[veh])
        (min, max) = updateMinMax(min, max, r2._veh2value[veh])
     # plot
print "data range: " + str(min) + " - " + str(max)
if options.verbose:
    print "Plotting..."
if options.time_coloring:
    scatter(xs, ys, color=c, s=1)
else:
    plot(xs, ys, ',k')
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
    xlim(min, max)
if options.ylim!="":
    (yb, ye) = options.ylim.split(",")
    ylim(yb, ye)
else:
    ylim(min, max)
# show/save
if options.show:
    show()
if options.output:
    savefig(options.output);


