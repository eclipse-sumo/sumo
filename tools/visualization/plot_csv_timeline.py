#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    plot_csv_timeline.py
# @author  Daniel Krajzewicz
# @author  Laura Bieker
# @date    2014-01-14

"""

This script plots selected columns from a given .csv file (';'-separated).
The loaded time lines are visualised as lines.
matplotlib (http://matplotlib.org/) has to be installed for this purpose

"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import csv

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
from sumolib.visualization import helpers  # noqa
from sumolib.options import ArgumentParser  # noqa
import matplotlib.pyplot as plt  # noqa


def readValues(file, verbose, columns):
    ret = {}
    with open(file) as f:
        if verbose:
            print("Reading '%s'..." % f)
        reader = csv.reader(f, delimiter=';')
        for row in reader:
            if columns is None:
                columns = range(0, len(row))
            for i in columns:
                if i not in ret:
                    ret[i] = []
                ret[i].append(float(row[i]))
    return ret


def main(args=None):
    """The main function; parses options and plots"""
    # ---------- build and read options ----------
    ap = ArgumentParser()
    ap.add_argument("-i", "--input", dest="input", category="input", type=ap.file, metavar="FILE",
                    required=True, help="Defines the input file to use")
    ap.add_argument("-v", "--verbose", dest="verbose", action="store_true",
                    default=False, help="If set, the script says what it's doing")
    ap.add_argument("--columns", dest="columns",
                    default=None, help="Defines which columns shall be plotted")
    # standard plot options
    helpers.addInteractionOptions(ap)
    helpers.addPlotOptions(ap)
    # parse
    options = ap.parse_args(args=args)

    if options.input is None:
        print("Error: an input file must be given")
        sys.exit(1)

    minV = 0
    maxV = 0
    if options.columns is not None:
        options.columns = [int(i) for i in options.columns.split(",")]
    nums = readValues(options.input, options.verbose, options.columns)
    for f in nums:
        maxV = max(maxV, len(nums[f]))
    ts = range(minV, maxV + 1)

    fig, ax = helpers.openFigure(options)
    for i in nums:
        v = nums[i]
        ci = i
        if options.columns is not None:
            ci = options.columns.index(i)
        c = helpers.getColor(options, ci, len(nums))
        addArgs = {"linestyle": options.linestyle, "color": c}
        if options.marker is not None:
            addArgs["marker"] = options.marker
        plt.plot(ts[0:len(v)], v, label=helpers.getLabel(str(i), ci, options), **addArgs)
    helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
    try:
        main(sys.argv[1:])
    except ValueError as e:
        sys.exit(e)
