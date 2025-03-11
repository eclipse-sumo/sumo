#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    scalability.py
# @author  Michael Behrisch
# @date    2024-02-21

import os
import sys
import subprocess
import glob
import timeit

import matplotlib.pyplot as plt

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

NUM_PARTS = 16


def get_options(args=None):
    argparser = sumolib.options.ArgumentParser()
    argparser.add_argument("--threads", type=int, default=os.cpu_count(), help="maximum number of threads")
    argparser.add_argument("--runs", type=int, default=3, help="number of runs for each number of threads")
    argparser.add_argument("-a", "--all", action="store_true", default=False,
                           help="run all thread counts, not only the powers of 2")
    argparser.add_argument("-v", "--verbose", action="store_true", default=False,
                           help="give more information")
    argparser.add_argument("-n", "--number", type=int, default=100, help="number of vehicles")
    argparser.add_argument("--figure", metavar="FILE", help="save figure to FILE")
    return argparser.parse_args(args)


def main():
    options = get_options()
    subprocess.call([sumolib.checkBinary('netgenerate'), "--grid", "--grid.x-number", str(NUM_PARTS),
                    "--grid.y-number", str(options.threads), "--grid.length", "500", "--plain", "p"])
    with open("f.edg.xml", "w", encoding="utf8") as eout:
        sumolib.xml.writeHeader(eout, root="edges")
        for e in sumolib.xml.parse("p.edg.xml", "edge"):
            if e.attr_from[0] < e.to[0]:
                eout.write(e.toXML("    "))
        eout.write("</edges>\n")
    subprocess.call([sumolib.checkBinary('netconvert'), "-n", "p.nod.xml", "-e", eout.name, "--no-internal-links"])
    os.remove(eout.name)
    for f in glob.glob("p.*.xml"):
        os.remove(f)

    with open("flow.rou.xml", "w", encoding="utf8") as rout:
        sumolib.xml.writeHeader(rout, root="routes")
        for i in range(options.threads):
            edges = ["%s%s%s%s" % (chr(c), i, chr(c+1), i) for c in range(65, 64 + NUM_PARTS)]
            rout.write('    <route id="r%s" edges="%s"/>\n' % (i, " ".join(edges)))
            rout.write('    <flow id="flow%s" route="r%s" end="3600.00" number="%s" departSpeed="max"/>\n' %
                       (i, i, options.number))
        rout.write("</routes>\n")

    if options.all:
        tc = range(1, options.threads + 1)
    else:
        tc = [2**p for p in range(20) if 2**p <= options.threads]
    results = []
    for t in tc:
        timer = timeit.Timer('subprocess.call(["%s", "-n", "net.net.xml", "-r", "%s", '
                             '"--threads", "%s", "--no-step-log"])' %
                             (sumolib.checkBinary("sumo").replace("\\", "\\\\"), rout.name, t),
                             setup='import subprocess')
        times = timer.repeat(options.runs, 1)
        if options.verbose:
            print(t, times)
        results.append((t, min(times)))
    plt.plot(*zip(*results))
    if options.figure:
        plt.savefig(options.figure)
    else:
        plt.show()


if __name__ == "__main__":
    main()
