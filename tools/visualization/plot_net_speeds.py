#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    plot_net_speeds.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2014-02-19
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.visualization import helpers

import matplotlib.pyplot as plt
import matplotlib


def main(args=None):
    """The main function; parses options and plots"""
    # ---------- build and read options ----------
    from optparse import OptionParser
    optParser = OptionParser()
    optParser.add_option("-n", "--net", dest="net", metavar="FILE",
                         help="Defines the network to read")
    optParser.add_option("--edge-width", dest="defaultWidth",
                         type="float", default=1, help="Defines the edge width")
    optParser.add_option("--edge-color", dest="defaultColor",
                         default='k', help="Defines the edge color")
    optParser.add_option("--minV", dest="minV",
                         type="float", default=None, help="Define the minimum value boundary")
    optParser.add_option("--maxV", dest="maxV",
                         type="float", default=None, help="Define the maximum value boundary")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="If set, the script says what it's doing")
    # standard plot options
    helpers.addInteractionOptions(optParser)
    helpers.addPlotOptions(optParser)
    # parse
    options, remaining_args = optParser.parse_args(args=args)

    if options.net is None:
        print("Error: a network to load must be given.")
        return 1
    if options.verbose:
        print("Reading network from '%s'" % options.net)
    net = sumolib.net.readNet(options.net)

    speeds = {}
    minV = None
    maxV = None
    for e in net._id2edge:
        v = net._id2edge[e]._speed
        if minV is None or minV > v:
            minV = v
        if maxV is None or maxV < v:
            maxV = v
        speeds[e] = v
    if options.minV is not None:
        minV = options.minV
    if options.maxV is not None:
        maxV = options.maxV
    # if options.logColors:
#    helpers.logNormalise(colors, maxColorValue)
#  else:
#    helpers.linNormalise(colors, minColorValue, maxColorValue)

    helpers.linNormalise(speeds, minV, maxV)
    for e in speeds:
        speeds[e] = helpers.getColor(options, speeds[e], 1.)
    fig, ax = helpers.openFigure(options)
    ax.set_aspect("equal", None, 'C')
    helpers.plotNet(net, speeds, {}, options)

    # drawing the legend, at least for the colors
    print("%s -> %s" % (minV, maxV))
    sm = matplotlib.cm.ScalarMappable(
        cmap=matplotlib.cm.get_cmap(options.colormap), norm=matplotlib.colors.normalize(vmin=minV, vmax=maxV))
    # "fake up the array of the scalar mappable. Urgh..." (pelson, http://stackoverflow.com/questions/8342549/matplotlib-add-colorbar-to-a-sequence-of-line-plots)
    sm._A = []
    plt.colorbar(sm)
    options.nolegend = True
    helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
    sys.exit(main(sys.argv))
