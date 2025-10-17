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

# @file    runExtraTests.py
# @author  Michael Behrisch
# @date    2012-03-29


import argparse
import glob
import logging
import os
import subprocess


def run(suffix, args, guiTests=False, chrouter=True):
    if type(args) is list:
        args = " ".join(args)
    if os.name != "posix":
        suffix += ".exe"
    env = os.environ
    root = os.path.abspath(os.path.dirname(__file__))
    for d in sorted(glob.glob(os.path.join(root, "*env", "*", "activate"))):
        env_dir = os.path.dirname(os.path.dirname(d))
        if env.get("VIRTUAL_ENV"):
            print("Virtual environment %s already active, ignoring %s." % (env["VIRTUAL_ENV"], env_dir))
        else:
            print("Using virtual environment %s." % env_dir)
            env["VIRTUAL_ENV"] = env_dir
            if os.name != "posix":
                env["PATH"] = "%s\\Scripts;%s" % (env_dir, env["PATH"])
            else:
                env["PATH"] = "%s/bin:%s" % (env_dir, env["PATH"])

    env["TEXTTEST_HOME"] = root
    env["LANG"] = "C"
    if "SUMO_HOME" not in env:
        env["SUMO_HOME"] = os.path.join(root, "..")
    for binary in ("activitygen", "emissionsDrivingCycle", "emissionsMap",
                   "dfrouter", "duarouter", "jtrrouter", "marouter",
                   "netconvert", "netedit", "netgenerate",
                   "od2trips", "polyconvert", "sumo"):
        env[binary.upper() + "_BINARY"] = os.path.join(root, "..", "bin", binary + suffix)
    env["GUISIM_BINARY"] = os.path.join(root, "..", "bin", "sumo-gui" + suffix)
    apps = ("sumo.extra,sumo.extra.gcf,sumo.extra.sf,sumo.meso,"
            "sumo.agg.ballistic,sumo.agg.idm,sumo.agg.sublanes,"
            "sumo.astar,sumo.parallel,duarouter.astar,netconvert.gdal,polyconvert.gdal,"
            "complex.meso,complex.libsumo,complex.libtraci,tools.extra")
    if chrouter:
        apps += ",duarouter.chrouter,duarouter.chwrapper"
    if guiTests:
        apps += ",sumo.meso.gui,sumo.gui.osg"
        if os.name == "posix":
            apps += ",complex.libsumo.gui"
    process = subprocess.Popen("%s %s -a %s" % ("texttest", args, apps), env=env,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    with process.stdout:
        for line in process.stdout:
            logging.info(line)
    process.wait()


if __name__ == "__main__":
    optParser = argparse.ArgumentParser()
    optParser.add_argument("-s", "--suffix", default="", help="suffix to the fileprefix")
    optParser.add_argument("-g", "--gui", default=False, action="store_true", help="run gui tests")
    options, args = optParser.parse_known_args()
    run(options.suffix, ["-" + a for a in args], options.gui)
