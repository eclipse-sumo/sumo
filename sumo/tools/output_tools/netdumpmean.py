#!/usr/bin/env python
"""
@file    netdumpdiff.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007-10-25
@version $Id: netdumpdiff.py 625 2008-03-08 14:04:01Z behr_mi $


This script reads two network dumps,
 computes the mean values
 and writes the results into the output file

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os, string, sys
from optparse import OptionParser
from xml.sax import saxutils, make_parser, handler
from datetime import datetime


class WeightsReader(handler.ContentHandler):
    """Reads the dump file"""

    def __init__(self):
        self._id = ''
        self._edgeValues = {}
        self._intervalBegins = []
        self._intervalEnds = []

    def startElement(self, name, attrs):
        if name == 'interval':
            self._beginTime = int(attrs['begin'])
            self._intervalBegins.append(self._beginTime)
            self._intervalEnds.append(int(attrs['end']))
            self._edgeValues[self._beginTime] = {}
        if name == 'edge':
            self._id = attrs['id']
            self._edgeValues[self._beginTime][self._id] = {}
            for attr in attrs.getQNames():
                if attr!="id":
                    self._edgeValues[self._beginTime][self._id][attr] = float(attrs[attr])
    
    def mean(self, weights):
         for t in self._edgeValues:
             for e in self._edgeValues[t]:
                 for a in self._edgeValues[t][e]:
                     self._edgeValues[t][e][a] = (self._edgeValues[t][e][a] + weights._edgeValues[t][e][a]) / 2.
 
    def write(self, options):
        fd = open(options.output, "w")
        fd.write("<?xml version=\"1.0\"?>\n\n")
        fd.write("<!-- generated on %s by netdumpdiv.py \n" % datetime.now())
        fd.write("   -1 %s\n" % options.dump1)
        fd.write("   -2 %s\n" % options.dump2)
        fd.write("   -o %s\n" % options.output)
        fd.write("-->\n\n")
        fd.write("<netstats>\n")
        for i in range(0, len(self._intervalBegins)):
            fd.write("   <interval begin=\"%s\" end=\"%s\">\n" % (self._intervalBegins[i], self._intervalEnds[i]))
            t = self._intervalBegins[i]
            for e in self._edgeValues[t]:
                fd.write("      <edge id=\"%s\"" % e)
                for a in self._edgeValues[t][e]:
                    fd.write(" %s=\"%s\"" % (a, self._edgeValues[t][e][a]))
                fd.write("/>\n")
            fd.write("   </interval>\n")
        fd.write("</netstats>\n")


# initialise 
optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
    # i/o
optParser.add_option("-1", "--dump1", dest="dump1",
                     help="First dump (mandatory)", metavar="FILE")
optParser.add_option("-2", "--dump2", dest="dump2",
                     help="Second  dump (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output", dest="output",
                     help="Name for the output", metavar="FILE")
# parse options
(options, args) = optParser.parse_args()


parser = make_parser()
# read dump1
if options.verbose:
    print "Reading dump1..."
weights1 = WeightsReader()
parser.setContentHandler(weights1)
parser.parse(options.dump1)
# read dump2
if options.verbose:
    print "Reading dump2..."
weights2 = WeightsReader()
parser.setContentHandler(weights2)
parser.parse(options.dump2)
# process
if options.verbose:
    print "Computing mean..."
weights1.mean(weights2)
# save
if options.verbose:
    print "Writing..."
weights1.write(options)

