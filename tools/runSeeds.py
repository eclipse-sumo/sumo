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
from threading import Thread
if sys.version_info.major < 3:
    from Queue import Queue
else:
    from queue import Queue

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    ap.add_option("-k", "--configuration", category="input", metavar="FILE", required=True,
                  type=ap.sumoconfig_file_list,
                  help="configuration to run or comma-separated list of configurations")
    ap.add_option("-a", "--application", metavar="FILE",
                  help="application to run or comma-separated list of applications")
    ap.add_option("-p", "--output-prefix",  default="SEED.", dest="prefix",
                  help="output prefix",)
    ap.add_option("--no-folders", action="store_true", category="output", default=False, dest="noFolders",
                  help=("do not create folders to distinguish multiple configurations or applications" +
                        "but use prefixes instead"))
    ap.add_option("--seeds", default="0:10",
                  help="which seeds to run")
    ap.add_option("--threads", type=int, default=1,
                  help="number of parallel processes")
    # parse options
    options, unknown_args = ap.parse_known_args(args=args)
    options.unknown_args = unknown_args

    if ":" in options.seeds:
        options.seeds = range(*map(int, options.seeds.split(":")))
    else:
        options.seeds = list(map(int, options.seeds.split(",")))

    if options.application is None:
        options.application = [sumolib.checkBinary("sumo")]
    else:
        options.application = options.application.split(',')
    for app in options.application:
        if hasattr(shutil, "which") and not shutil.which(app):
            sys.stderr.write("Error: application '%s' not found\n" % app)
            sys.exit()

    options.configuration = options.configuration.split(',')
    for cfg in options.configuration:
        if not os.path.exists(cfg):
            sys.stderr.write("Error: configuration '%s' not found\n" % cfg)
            sys.exit()

    if "SEED" not in options.prefix:
        sys.stderr.write("Warning: --output-prefix should contain 'SEED' to prevent overwriting\n")

    return options


def getUniqueFolder(options, app, cfg, folders):
    key = []
    if len(options.application) > 1:
        key.append(os.path.basename(app))
    if len(options.configuration) > 1:
        key.append(os.path.basename(cfg))
    if not key:
        return ""

    folder = '_'.join(key)
    if folder in folders:
        i = 0
        folder = '_'.join(key + [str(i)])
        while folder in folders:
            i += 1
            folder = '_'.join(key + [str(i)])

    folders.add(folder)
    if not options.noFolders and not os.path.exists(folder):
        os.makedirs(folder)
    return folder


def getCommExtensionLength(names):
    if not names or len(names) == 1:
        return 0
    common = reversed(names[0])
    for n in names[1:]:
        common2 = []
        for c1, c2 in zip(common, reversed(n)):
            if c1 == c2:
                common2.append(c1)
            else:
                break
        common = ''.join(common2)
    return len(common)


def main(options):
    q = Queue()

    def runSim():
        while True:
            app, cfg, seed, folder = q.get()
            prefix = options.prefix.replace("SEED", str(seed))
            if folder:
                if options.noFolders:
                    prefix = "%s_%s" % (folder, prefix)
                else:
                    prefix = os.path.join(folder, prefix)
            if options.verbose:
                print("running seed %s" % seed)
            args = [app,
                    '-c', cfg,
                    '--seed', str(seed),
                    '--output-prefix', prefix]
            subprocess.call(args + options.unknown_args)
            q.task_done()

    for i in range(options.threads):
        t = Thread(target=runSim)
        t.daemon = True
        t.start()

    folders = set()

    cEL = getCommExtensionLength(options.configuration)
    for app in options.application:
        for cfg in options.configuration:
            folder = getUniqueFolder(options, app, cfg[:-cEL], folders)
            for seed in options.seeds:
                q.put((app, cfg, seed, folder))
    q.join()


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
