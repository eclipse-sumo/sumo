#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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


import optparse
import os
import subprocess
import logging


def run(suffix, args, guiTests=False, chrouter=True):
    if type(args) is list:
        args = " ".join(args)
    if os.name != "posix":
        suffix += ".exe"
    env = os.environ
    root = os.path.abspath(os.path.dirname(__file__))
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
    apps = "sumo.extra,sumo.meso,sumo.ballistic,sumo.idm,sumo.sublanes,sumo.astar,sumo.parallel,"
    apps += "netconvert.gdal,polyconvert.gdal,complex.meso,duarouter.astar,complex.libsumo,complex.libtraci"
    if chrouter:
        apps += ",duarouter.chrouter,duarouter.chwrapper"
    try:
        if os.name == "posix":
            subprocess.call(['python2', '-V'], stdout=open(os.devnull, "w"))
        apps += ',complex.python2,tools.python2'
    except Exception:
        pass
    if guiTests:
        apps += ",sumo.meso.gui,sumo.gui.osg"
#        if os.name == "posix":
#            apps += ",complex.libsumo.gui"
    process = subprocess.Popen("%s %s -a %s" % ("texttest", args, apps), env=env,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    with process.stdout:
        for line in process.stdout:
            logging.info(line)
    process.wait()


if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.add_option("-s", "--suffix", default="",
                         help="suffix to the fileprefix")
    optParser.add_option("-g", "--gui", default=False,
                         action="store_true", help="run gui tests")
    (options, args) = optParser.parse_args()
    run(options.suffix, ["-" + a for a in args], options.gui)
