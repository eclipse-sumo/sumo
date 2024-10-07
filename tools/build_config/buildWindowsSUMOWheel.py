#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

# @file    buildWindowsSUMOWheel.py
# @author  Michael Behrisch
# @date    2024-03-07

import glob
import os
import shutil
import subprocess
import sys
from os.path import join, dirname

import version


def main():
    SUMO_HOME = os.environ.get("SUMO_HOME", dirname(dirname(dirname(os.path.abspath(__file__)))))
    BUILD_CONFIG = join(SUMO_HOME, "tools", "build_config")
    shutil.copy(join(SUMO_HOME, "build_config", "pyproject.toml"), SUMO_HOME)
    version.filter_setup_py(join(BUILD_CONFIG, "setup-sumo.py"), join(SUMO_HOME, "setup.py"))
    subprocess.call([sys.executable, "-m", "build", "--wheel", "--config-setting=-G=Ninja"], cwd=SUMO_HOME)
    f = glob.glob(join(SUMO_HOME, "dist", "eclipse_sumo-*"))[0]
    os.rename(f, f.replace("cp3%s-cp3%s" % (sys.version_info.minor, sys.version_info.minor), "py2.py3-none"))


if __name__ == "__main__":
    main()
