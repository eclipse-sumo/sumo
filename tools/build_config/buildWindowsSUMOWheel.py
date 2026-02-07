#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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

import logging
import os
import sys
from os.path import join, dirname

import status
import version


def main():
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    SUMO_HOME = os.environ.get("SUMO_HOME", dirname(dirname(dirname(os.path.abspath(__file__)))))
    for package in ("eclipse-sumo", "sumo-data", "libsumo"):
        version.filter_pep440(join(SUMO_HOME, "build_config", "pyproject", package + ".toml"),
                              join(SUMO_HOME, "pyproject.toml"), package == "sumo-data")
        status.log_subprocess([sys.executable, "-m", "build", "--wheel"], cwd=SUMO_HOME)


if __name__ == "__main__":
    main()
