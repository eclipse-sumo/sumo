#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    plot_net_speeds.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2014-02-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib  # noqa
from sumolib.visualization import helpers  # noqa
from sumolib.options import ArgumentParser  # noqa
import matplotlib.pyplot as plt  # noqa
import matplotlib  # noqa


def main(args=None):
    """The main function; parses options and plots"""
    # ---------- build and read options ----------
    ap = ArgumentParser()
    ap.add_argument("-n", "--net", dest="net", category="input", type=ap.net_file, metavar="FILE",
                    required=True, help="Defines the network to read")
    ap.add_argument("--edge-width", dest="defaultWidth", category="visualization",
                    type=float, default=1, help="Defines the edge width")
    ap.add_argument("--edge-color", dest="defaultColor", category="visualization",
                    default='k', help="Defines the edge color")
    ap.add_argument("--minV", dest="minV",
                    type=float, default=None, help="Define the minimum value boundary")
    ap.add_argument("--maxV", dest="maxV",
                    type=float, default=None, help="Define the maximum value boundary")
    ap.add_argument("-v", "--verbose", dest="verbose", action="store_true",
                    default=False, help="If set, the script says what it's doing")
    # standard plot options
    helpers.addInteractionOptions(ap)
    helpers.addPlotOptions(ap)
    # parse
    options = ap.parse_args(args=args)

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
        cmap=helpers.getColorMap(options), norm=matplotlib.colors.Normalize(vmin=minV, vmax=maxV))
    if sys.version_info.major < 3:
        # "fake up the array of the scalar mappable. Urgh..."
        # (pelson, http://stackoverflow.com/questions/8342549/matplotlib-add-colorbar-to-a-sequence-of-line-plots)
        sm._A = []
    plt.colorbar(sm, ax=ax)
    options.nolegend = True
    helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
    try:
        main()
    except ValueError as e:
        sys.exit(e)
