#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    toolrunner.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2008-03-29
# @version $Id$

import os
import subprocess
import sys
if len(sys.argv) < 2:
    sys.exit('required argument <tool> missing')
tool = [os.path.join(os.path.dirname(sys.argv[0]), "..", sys.argv[-1])]
if tool[0].endswith(".jar"):
    tool = ["java", "-jar"] + tool

if tool[0].endswith(".py"):
    tool = [os.environ.get('PYTHON', 'python')] + tool

subprocess.call(tool + sys.argv[1:-1], env=os.environ,
                stdout=sys.stdout, stderr=sys.stderr)
