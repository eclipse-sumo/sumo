#!/usr/bin/env python
"""
@file    route2poly.py
@author  Jakob Erdmann
@date    2012-11-15
@version $Id$

From a sumo network and a route file, this script generates a polygon (polyline) for every route
which can be loaded with sumo-gui for visualization

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2007-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sys
import os
import colorsys
import itertools
from random import random
from optparse import OptionParser
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.output import parse
from sumolib.net import readNet

def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <netfile> <routefile> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    optParser.add_option("-u", "--hue", default="random", help="hue for polygons (float from [0,1] or 'random')")
    optParser.add_option("-s", "--saturation", default=1, help="saturation for polygons (float from [0,1] or 'random')")
    optParser.add_option("-b", "--brightness", default=1, help="brightness for polygons (float from [0,1] or 'random')")
    optParser.add_option("-l", "--layer", default=100, help="layer for generated polygons")
    options, args = optParser.parse_args()
    try:
        options.net, options.routefile = args
        options.colorgen = Colorgen((options.hue, options.saturation, options.brightness))
    except:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.routefile + ".poly.xml"
    return options 


class Colorgen:
    def __init__(self, hsv):
        self.hsv = hsv 

    def get_value(self, opt):
        if opt == 'random':
            return random()
        else:
            return float(opt)
        
    def __call__(self):
        """return constant or randomized rgb-color string"""
        return ','.join(map(str, colorsys.hsv_to_rgb(*map(self.get_value, self.hsv))))


def generate_poly(net, id, color, layer, edges, outf):
    shape = list(itertools.chain(*list(net.getEdge(e).getShape() for e in edges))) 
    shapeString = ' '.join('%s,%s' % (x,y) for x,y in shape)
    outf.write('<poly id="%s" color="%s" layer="%s" type="route" shape="%s"/>\n' % (
        id, color, layer, shapeString))

def main():
    options = parse_args()
    net = readNet(options.net)
    with open(options.outfile, 'w') as outf:
        outf.write('<polygons>\n')
        for vehicle in parse(options.routefile, 'vehicle'):
            generate_poly(net, vehicle.id, options.colorgen(), options.layer, vehicle.route[0].edges.split(), outf)
        outf.write('</polygons>\n')

if __name__ == "__main__":
    main()
