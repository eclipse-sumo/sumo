#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runSeeds.py
# @author  Jakob Erdmann
# @date    2022-04-30

"""
This script executes a config repeatedly with different seeds
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import subprocess
import shutil

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    import sumolib  # noqa
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    ap.add_option("-k", "--configuration", metavar="FILE",
                  help="configuration to run")
    ap.add_option("-a", "--application", default="sumo", metavar="FILE",
                  help="application to run")
    ap.add_option("-p", "--output-prefix", default="SEED.", dest="prefix",
                  help="output prefix",)
    ap.add_option("--seeds", default="0:10",
                  help="which seeds to run")
    # parse options
    options = ap.parse_args(args=args)

    if ":" in options.seeds:
        options.seeds = range(*map(int, options.seeds.split(":")))
    else:
        options.seeds = map(int, options.seeds.split(","))

    if not options.configuration:
        sys.stderr.write("Error: option configuration is mandatory\n")
        sys.exit()
    elif not os.path.exists(options.configuration):
        sys.stderr.write("Error: configuration '%s' not found\n" % options.configuration)
        sys.exit()

    if hasattr(shutil, "which") and not shutil.which(options.application):
        sys.stderr.write("Error: application '%s' not found\n" % options.application)
        sys.exit()

    if "SEED" not in options.prefix:
        sys.stderr.write("Warning: --output-prefix should contain 'SEED' to prevent overwriting\n")

    return options


def main(options):
    for seed in options.seeds:
        prefix = options.prefix.replace("SEED", str(seed))
        if options.verbose:
            print("running seed %s" % seed)
        args = [options.application,
                '-c', options.configuration,
                '--seed', str(seed),
                '--output-prefix', prefix]
        subprocess.call(args)


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
