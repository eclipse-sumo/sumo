#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2015-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    update_templates.py
# @author  Jakob Erdmann
# @date    Jan 2025

"""
This script finds tools that should be added to templates.py
"""

import os
import sys
from os import path
import glob
from templates import TOOLS, generateToolTemplates

toolDir = path.join(path.dirname(__file__), '..')
pyfiles = glob.glob("**/*.py", root_dir=toolDir, recursive=True)
print("found %s python files" % len(pyfiles))

candidates = []
for fname in pyfiles:
    with open(os.path.join(toolDir, fname)) as f:
        for line in f:
            if "ArgumentParser" in line:
                candidates.append(fname)
                break
print("found %s files that use ArgumentParser" % len(candidates))

candidates = [f for f in candidates if path.dirname(f) not in ('build_config', 'devel', 'game', 'purgatory')]
print("found %s tools in eligble directories" % len(candidates))

usedTools = set(TOOLS)
candidates = [f for f in candidates if f not in usedTools]
print("found %s tools that are not listed in templates.py" % len(candidates))

failed = generateToolTemplates(toolDir, candidates, False, True)
if failed:
    print("%s tools fail at template generation:" % len(failed))
    print('\n'.join(failed), file=sys.stderr)
failedSet = set(failed)
missing = [f for f in candidates if f not in failedSet]
print("found %s usable tools:" % len(missing))
print('\n'.join(missing))
