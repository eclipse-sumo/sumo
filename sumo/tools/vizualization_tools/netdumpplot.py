#!/usr/bin/python
# This script reads a network and a dump file and
#  draws the network, coloring it by the values
#  found within the dump-file
# matplotlib has to be installed for this purpose

from matplotlib import rcParams
rcParams['text.fontname'] = 'cmr10'
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
    r = 255. - (255. * val)
    g = 255. * val
    b = 0
    return "#" + toHex(r) + toHex(g) + toHex(b)




class NetReader(handler.ContentHandler):
    """Reads a network, storing the edge geometries, lane numbers and max. speeds"""

    def __init__(self):
        self._id = ''
        self._edge2lanes = {}
        self._edge2speed = {}
        self._edge2shape = {}
        self._edge2from = {}
        self._edge2to = {}
        self._node2x = {}
        self._node2y = {}
        self._currentShapes = []
        self._parseLane = False

    def startElement(self, name, attrs):
        self._parseLane = False
        if name == 'edge':
            if not attrs.has_key('function') or attrs['function'] != 'internal':
                self._id = attrs['id']
                self._edge2from[attrs['id']] = attrs['from']
                self._edge2to[attrs['id']] = attrs['to']
                self._edge2lanes[attrs['id']] = 0
                self._currentShapes = []
            else:
                self._id = ""
        if name == 'lane' and self._id!="":
            self._edge2speed[self._id] = float(attrs['maxspeed'])
            self._edge2lanes[self._id] = self._edge2lanes[self._id] + 1
            self._parseLane = True
            self._currentShapes.append("")
        if name == 'junction':
            self._id = attrs['id']
            if self._id[0]!=':':
                self._node2x[attrs['id']] = attrs['x']
                self._node2y[attrs['id']] = attrs['y']
            else:
                self._id = ""

    def characters(self, content):
        if self._parseLane:
            self._currentShapes[-1] = self._currentShapes[-1] + content

    def endElement(self, name):
        if self._parseLane:
            self._parseLane = False
        if name == 'edge' and self._id!="":
            noShapes = len(self._currentShapes)
            if noShapes%2 == 1 and noShapes>0:
                self._edge2shape[self._id] = self._currentShapes[int(noShapes/2)]
            elif noShapes%2 == 0 and len(self._currentShapes[0])!=2:
                cshapes = []
                minLen = -1
                for i in self._currentShapes:
                    cshape = []
                    es = i.split(" ")
                    for e in es:
                        p = e.split(",")
                        cshape.append((float(p[0]), float(p[1])))
                    cshapes.append(cshape)
                    if minLen==-1 or minLen>len(cshape):
                        minLen = len(cshape)
                self._edge2shape[self._id] = ""
                if minLen>2:
                    for i in range(0, minLen):
                        x = 0.
                        y = 0.
                        for j in range(0, noShapes):
                            x = x + cshapes[j][i][0]
                            y = y + cshapes[j][i][1]
                        x = x / float(noShapes)
                        y = y / float(noShapes)
                        if self._edge2shape[self._id] != "":
                            self._edge2shape[self._id] = self._edge2shape[self._id] + " "
                        self._edge2shape[self._id] = self._edge2shape[self._id] + str(x) + "," + str(y)
    


    def plotData(self, weights, options, values1, values2, saveName):
        edge2plotLines = {}
        edge2plotColors = {}
        edge2plotWidth = {}
        for edge in self._edge2from:
           # compute shape
           xs = []
           ys = []
           if edge not in self._edge2shape or self._edge2shape[edge]=="":
               xs.append(float(self._node2x[self._edge2from[edge]]))
               xs.append(float(self._node2x[self._edge2to[edge]]))
               ys.append(float(self._node2y[self._edge2from[edge]]))
               ys.append(float(self._node2y[self._edge2to[edge]]))
           else:
               shape = self._edge2shape[edge].split(" ")
               l = []
               for s in shape:
                   p = s.split(",")
                   xs.append(float(p[0]))
                   ys.append(float(p[1]))
           # save shape
           edge2plotLines[edge] = (xs, ys)
           # compute color
           c = values2[edge]
           if options.tendency_coloring:
               if c<0:
                   c = 0
               else:
                   c = 1
           else:
               if options.percentage_speed:
                   c = c / self._edge2speed[edge]
               else:
                   c = (c-self._minValue2) / (self._maxValue2-self._minValue2)
           print str(self._maxValue2) + " -> " + str(self._minValue2)
           print str(values2[edge]) + " -> " + str(c)
           edge2plotColors[edge] = toColor(c)
           # compute width
           w = values1[edge]
           if w!=0:
               edge2plotWidth[edge] = math.log(values1[edge]) + options.min_width
           else:
               edge2plotWidth[edge] = options.min_width
           if edge2plotWidth[edge]>options.max_width:
               edge2plotWidth[edge] = options.max_width
        for edge in edge2plotLines:
           plot(edge2plotLines[edge][0], edge2plotLines[edge][1], color=edge2plotColors[edge], linewidth=edge2plotWidth[edge])
        if options.show:
           show()
        savefig();


    def plot(self, weights, options):
        self._minValue1 = weights._minValue1
        self._minValue2 = weights._minValue2
        self._maxValue1 = weights._maxValue1
        self._maxValue2 = weights._maxValue2

        if options.join:
            self.plotData(weights, options, weights._edge2value1, weights._edge2value2, options.output)
        else:
            for i in weights._intervalBegins:
                self.plotData(weights, options, weights._unaggEdge2value1[i], weights._unaggEdge2value1[i], options.output % i)


    def knowsEdge(self, id):
        return id in self._edge2from





class WeightsReader(handler.ContentHandler):
    """Reads the dump file"""

    def __init__(self, net, value1, value2):
        self._id = ''
        self._edge2value2 = {}
        self._edge2value1 = {}
        self._edge2no = {}
        self._net = net
        self._intervalBegins = []
        self._unaggEdge2value2 = {}
        self._unaggEdge2value1 = {}
        self._beginTime = -1
        self._value1 = value1
        self._value2 = value2

    def startElement(self, name, attrs):
        if name == 'interval':
            self._beginTime = int(attrs['begin'])
            self._intervalBegins.append(self._beginTime)
            self._unaggEdge2value2[self._beginTime] = {}
            self._unaggEdge2value1[self._beginTime] = {}
        if name == 'edge':
            if self._net.knowsEdge(attrs['id']):
                self._id = attrs['id']
                if self._id not in self._edge2value2:
                    self._edge2value2[self._id] = 0
                    self._edge2value1[self._id] = 0
                    self._edge2no[self._id] = 0
                self._edge2value2[self._id] = self._edge2value2[self._id] + float(attrs[self._value2])
                self._edge2value1[self._id] = self._edge2value1[self._id] + float(attrs[self._value1])
                self._edge2no[self._id] = self._edge2no[self._id] + 1
                self._unaggEdge2value2[self._beginTime][self._id] = float(attrs[self._value2])
                self._unaggEdge2value1[self._beginTime][self._id] = float(attrs[self._value1])


    def updateExtrema(self, values1ByEdge, values2ByEdge):
        for edge in values1ByEdge:
            if self._minValue1==-1 or self._minValue1>values1ByEdge[edge]:
                self._minValue1 = values1ByEdge[edge]
            if self._maxValue1==-1 or self._maxValue1<values1ByEdge[edge]:
                self._maxValue1 = values1ByEdge[edge]
            if self._minValue2==-1 or self._minValue2>values2ByEdge[edge]:
                self._minValue2 = values2ByEdge[edge]
            if self._maxValue2==-1 or self._maxValue2<values2ByEdge[edge]:
                self._maxValue2 = values2ByEdge[edge]
        if self._minValue2<options.min_color:
            self._minValue2 = options.min_color
        if self._maxValue2>options.max_color:
            self._maxValue2 = options.max_color

    def norm(self):
        self._minValue1 = -1
        self._maxValue1 = -1
        self._minValue2 = -1
        self._maxValue2 = -1
        if options.join:
            for edge in self._edge2value2:
                if float(self._edge2no[edge])!=0:
                    self._edge2value2[edge] = float(self._edge2value2[edge]) / float(self._edge2no[edge])
                    self._edge2value1[edge] = float(self._edge2value1[edge]) / float(self._edge2no[edge])
                else:
                    self._edge2value2[edge] = float(self._edge2value2[edge])
                    self._edge2value1[edge] = float(self._edge2value1[edge])
            self.updateExtrema(self._edge2value1, self._edge2value2)
        else:
            for i in weights._intervalBegins:
                self.updateExtrema(self._unaggEdge2value1[i], self._unaggEdge2value2[i])

    

# initialise 
optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
    # i/o
optParser.add_option("-n", "--net-file", dest="net",
                     help="SUMO network to use (mandatory)", metavar="FILE")
optParser.add_option("-d", "--dump", dest="dump",
                     help="dump file to use", metavar="FILE")
optParser.add_option("-o", "--output", dest="output",
                     help="(base) name for the output", metavar="FILE")
    # data handling
optParser.add_option("-j", "--join", action="store_true", dest="join",
                     default=False, help="sums up values from all read intervals")
optParser.add_option("-w", "--min-width", dest="min_width",
                     type="float", default=0., help="sets minimum line width")
optParser.add_option("-W", "--max-width", dest="max_width",
                     type="float", default=100., help="sets maximum line width")
optParser.add_option("-c", "--min-color", dest="min_color",
                     type="float", default=0., help="sets minimum color (between 0 and 1)")
optParser.add_option("-C", "--max-color", dest="max_color",
                     type="float", default=1., help="sets maximum color (between 0 and 1)")
optParser.add_option("--tendency-coloring", action="store_true", dest="tendency_coloring",
                     default=False, help="show only 0/1 color for egative/positive values")
optParser.add_option("--percentage-speed", action="store_true", dest="percentage_speed",
                     default=False, help="speed is normed to maximum allowed speed on an edge")
optParser.add_option("--values", dest="values", 
                     type="string", default="entered,speed", help="which values shall be parsed")

    # processing
optParser.add_option("-s", "--show", action="store_true", dest="show",
                     default=False, help="shows each plot after generating it")
# parse options
(options, args) = optParser.parse_args()


# read network
if options.verbose:
    print "Reading net..."
parser = make_parser()
net = NetReader()
parser.setContentHandler(net)
parser.parse(options.net)
# read weights
if options.verbose:
    print "Reading weights..."
mValues = options.values.split(",")
weights = WeightsReader(net, mValues[0], mValues[1])
parser.setContentHandler(weights)
parser.parse(options.dump)
# process
if options.verbose:
    print "Norming weights..."
weights.norm()
if options.verbose:
    print "Plotting..."
net.plot(weights, options)

