#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    patchVClasses.py
# @author  Jakob Erdmann
# @date    2024-04-30

"""
Removes vclasses from the network so it can be loaded with older versions of SUMO
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa


def parse_args():
    optParser = ArgumentParser()
    optParser.add_option("net", category="input", type=optParser.net_file,
                         help="the network to patch")
    optParser.add_option("-o", "--output-file", dest="outfile", category="output", type=optParser.net_file,
                         help="the output network file")
    optParser.add_option("-d", "--delete-classes", dest="deleteClasses",
                         default="subway,cable_car,aircraft,wheelchair,scooter,drone,container",
                         help="list of classes to remove")
    options = optParser.parse_args()
    if not options.outfile:
        options.outfile = "patched." + options.net

    return options


def main(options):
    classes = options.deleteClasses.split(',')
    with open(options.outfile, 'w') as outf:
        for line in open(options.net):
            for c in classes:
                line = line.replace(c + " ", "")
                line = line.replace(" " + c, "")
            outf.write(line)


if __name__ == "__main__":
    main(parse_args())
