#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2015-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    templates.py
# @author  Pablo Alvarez Lopez
# @date    April 2023

"""
This script runs python and netgenerate tools in linux.
Is temporal, while the problem with popen/pclose and
FOX toolkit isn't fixed
"""

from __future__ import print_function
from __future__ import absolute_import

import sys
import os
import re
from os.path import dirname, join
from subprocess import check_output, CalledProcessError
from glob import glob

def main():
    if len(sys.argv) == 2:
        sys.exit("Arguments: <pathToSumo> <pathToNetgenerate>")



if __name__ == "__main__":
    main()
