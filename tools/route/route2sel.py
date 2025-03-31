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

# @file    route2sel.py
# @author  Jakob Erdmann
# @date    2015-08-05

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.output import parse, parse_fast  # noqa
from sumolib.options import ArgumentParser  # noqa


def parse_args():
    ap = ArgumentParser()
    ap.add_option("routefiles", nargs="+", category="input", type=ap.file_list, help="route files")
    ap.add_option("-o", "--outfile", category="output", type=ap.file, help="name of output file")
    options = ap.parse_args()
    if options.outfile is None:
        options.outfile = options.routefiles[0] + ".sel.txt"
    return options


def main():
    options = parse_args()
    edges = set()

    for routefile in options.routefiles:
        for route in parse_fast(routefile, 'route', ['edges']):
            edges.update(route.edges.split())
        for walk in parse_fast(routefile, 'walk', ['edges']):
            edges.update(walk.edges.split())

        # warn about potentially missing edges
        for trip in parse(routefile, ['trip', 'flow'], heterogeneous=True):
            for attr in ['attr_from', 'to', 'via']:
                if trip.attr_from:
                    edges.update(trip.attr_from)
                if trip.to:
                    edges.update(trip.to)
                if trip.via:
                    edges.update(trip.via.split())
        for walk in parse_fast(routefile, 'walk', ['from', 'to']):
            edges.update([walk.attr_from, walk.to])

    with open(options.outfile, 'w') as outf:
        for e in sorted(list(edges)):
            outf.write('edge:%s\n' % e)


if __name__ == "__main__":
    main()
