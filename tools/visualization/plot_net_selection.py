#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    plot_net_selection.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2014-02-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.visualization import helpers  # noqa
from sumolib.options import ArgumentParser  # noqa


def main(args=None):
    """The main function; parses options and plots"""
    # ---------- build and read options ----------
    ap = ArgumentParser()
    ap.add_argument("-n", "--net", dest="net", category="input", type=ap.net_file, metavar="FILE",
                    required=True, help="Defines the network to read")
    ap.add_argument("-i", "--selection", dest="selection", category="input", type=ap.file, metavar="FILE",
                    required=True, help="Defines the selection to read")
    ap.add_argument("--selected-width", dest="selectedWidth", category="visualization",
                    type=float, default=1, help="Defines the width of selected edges")
    ap.add_argument("--color", "--selected-color", dest="selectedColor", category="visualization",
                    default='r', help="Defines the color of selected edges")
    ap.add_argument("--edge-width", dest="defaultWidth", category="visualization",
                    type=float, default=.2, help="Defines the width of not selected edges")
    ap.add_argument("--edge-color", dest="defaultColor", category="visualization",
                    default='#606060', help="Defines the color of not selected edges")
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
    selection = sumolib.files.selection.read(options.selection)

    colors = {}
    widths = {}
    for e in selection["edge"]:
        colors[e] = options.selectedColor
        widths[e] = options.selectedWidth

    fig, ax = helpers.openFigure(options)
    ax.set_aspect("equal", None, 'C')
    helpers.plotNet(net, colors, widths, options)
    options.nolegend = True
    helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
    try:
        main()
    except ValueError as e:
        sys.exit(e)
